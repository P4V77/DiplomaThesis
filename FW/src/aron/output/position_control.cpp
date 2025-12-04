#include "position_control.hpp"

namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void Position_control::init()
    {
        // EVSYS CHANNEL 14
        // Event generation on TCC0 CC trigger
        EVSYS->Channel[14].CHANNEL.reg |= EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
                                          EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TCC0_OVF);

        EVSYS->USER[EVSYS_ID_USER_ADC1_START].reg |= 0x0F; // EVSYS Channel 14
    }

    void Position_control::position_calibration()
    {
        WDT->CTRLA.bit.ENABLE = 0; // Disable WDT
        while (WDT->SYNCBUSY.bit.ENABLE)
            ;
        // Disable ADC1 interupt on RESRDY
        ADC1->INTENCLR.bit.RESRDY = 1;

        // MAX position
        aron::Current_source::set_current(2.5f);
        nd::chrono::wait(nd::chrono::durations::milliseconds{100});
        aron::Current_source::enable();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1000});

        ADC1->SWTRIG.bit.FLUSH = 1;
        ADC1->INPUTCTRL.reg = (ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND);

        while (ADC1->SYNCBUSY.bit.SWTRIG)
            ;

        while (!ADC1->INTFLAG.bit.RESRDY)
            ;

        float voltage_adc = nd::utils::digital_to_analog_voltage(12, aron::Current_source::get_adc_vref(), ADC1->RESULT.reg);

        // // Substract max voltage from table and meassured voltage
        LUT_offset = voltage_adc - (lut_voltage_min);
        log().debug("max h_interp {} adc meassured {} LUT offset {}", lut_voltage_min, voltage_adc, LUT_offset);

        aron::Current_source::set_current(0.9f);
        nd::chrono::wait(nd::chrono::durations::milliseconds{500});
        aron::Current_source::set_current(0.0f);

        aron::Current_source::disable();
        ADC1->INTENSET.bit.RESRDY = 1;

        WDT->CTRLA.bit.ENABLE = 1; // Disable WDT
        while (WDT->SYNCBUSY.bit.ENABLE)
            ;
    }

    void Position_control::position_regulator()
    {
        // PID
        static regulator::PID pid_position{Kp, Ki, Kd, Imin, Imax, sampling_freq, f_c};

        // Simple Exponential Moving Average (EMA) filter
        static float filtered_position_actual = 0.0f; // Initialize the filter state
        float raw_position_actual = LUT[get_LUT_index()];

        // Filter parameter (alpha) - Adjust this for smoothing (0.0 < alpha < 1.0)
        float alpha = 0.75f; // Example: Lower alpha for more smoothing// 0.55

        // Apply the EMA filter
        filtered_position_actual = alpha * raw_position_actual + (1.0f - alpha) * filtered_position_actual;

        // Error signal
        // const float error_position = position_request.load() - filtered_position_actual;
        const float error_position = position_request.load() - raw_position_actual;

        aron::Current_source::set_current(pid_position.out_pid(&error_position));

        // log().debug("index {}", position_actual);
        // position_actual = (h_interp[B_index][I_index]);
        // log().debug("h (mm) {}", h_interp[B_index][I_index]);
        // log().debug("Iindex{} Bindex{}", I_index, B_index);
        // log().debug("position I request {}", position_pid_regulation);
        // nd::chrono::wait(nd::chrono::durations::milliseconds{100});
        // log().debug("pos actual (mm) {}, current req {}", position_actual.load(), position_pid_regulation);
        // log().debug("h mm {}, {} pos req {} error {}", position_actual.load(), h_interp[B_index][I_index], position_request.load(), error);
    }

    void Position_control::evaluate()
    {
        // adc1_result.store(nd::utils::digital_to_analog_voltage(12, aron::Current_source::get_adc_vref(), ADC1->RESULT.reg));
        adc1_result.store(nd::utils::digital_to_analog_voltage(12, 2.5f, ADC1->RESULT.reg));
        position_regulator();
        // debug_logger();
    }

    void Position_control::adc1_position_handler()
    {
        // If position control not allowed return
        if (position_control == true)
        {
            aron::Position_control::evaluate();
        }
    }

    void Position_control::debug_logger()
    {
        log().debug("ADC1: {} V", adc1_result);
    }

    float Position_control::get_flux_density()
    {
        float B_mT = (adc1_result.load() - 0.87065f) * 1000.0f * hall_sensitivity; // Calculated Magnetic flux density
        // log().debug("ADC1 {} B_mT {}", adc1_result[connector], B_mT);
        return (B_mT);
    }

    float Position_control::get_voltage()
    {
        return (adc1_result.load());
    }

    float Position_control::get_actual_position()
    {
        return (position_actual.load());
    }

    uint16_t Position_control::get_LUT_index()
    {
        const float adc_voltage = aron::Position_control::get_voltage();
        /* NOT CONVERTING TO TESLA, STAYING INTENTIONALY IN mV */
        // uint16_t voltage_index = static_cast<uint16_t>((adc_voltage - voltage_0mm) / voltage_resolution);
        uint16_t voltage_index = static_cast<uint16_t>((adc_voltage - (lut_voltage_min + LUT_offset)) / ((lut_voltage_max + LUT_offset) - (lut_voltage_min + LUT_offset)) * lut_voltage_range);

        return std::clamp(voltage_index, static_cast<uint16_t>(0), lut_voltage_range);
    }

    void Position_control::set_position(const float position)
    {
        position_request.exchange(position);
    }

    void Position_control::set_position_control(bool request)
    {
        ADC1->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND;
        while (ADC1->SYNCBUSY.bit.INPUTCTRL)
            ;
        position_control = request;
    }

    void Position_control::set_position_logging(bool request)
    {
        position_log_enabled = request;
    }

    bool Position_control::get_position_control_status()
    {
        return position_control;
    }
    void Position_control::log_position_can()
    {
        if (position_log_enabled)
        {
            std::array<char, 64> buffer; // Static buffer
            auto [iterator1, count1] = fmt::format_to_n(buffer.begin(), buffer.size(), "{}ID A.P.: {}  R.P.: {}", aron::Communication::get_source_id(), position_actual.load(), position_request.load());
            const std::string_view msg{buffer.begin(), buffer.begin() + count1};

            aron::Communication::send_can_message(msg);
            // log().debug("Sending CAN message: {}", msg); // Add this before sending the message
            // log().debug("A.P.: {}  R.P.: {}", position_actual.load(), position_request.load());
        }
    }
}