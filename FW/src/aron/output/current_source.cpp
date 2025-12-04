#include "current_source.hpp"
namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void Current_source::init()
    {
        nd::drivers::same5x::Mclk_ctrl::enable(ADC0);
        nd::drivers::same5x::Gclk_pchctrl::enable(ADC0_GCLK_ID, 3); // 30 MHz
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pa06}.enable_alternate_function(nd::drivers::Gpio_alt_functions::B);

        ADC0->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV2; // 15 MHz

        // REFERENCE VREFA
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pa04}.enable_alternate_function(nd::drivers::Gpio_alt_functions::B);
        ADC0->REFCTRL.reg = ADC_REFCTRL_REFSEL_AREFB; // ADC_REFCTRL_REFSEL_INTREF;
        while (ADC0->SYNCBUSY.bit.REFCTRL)
            ;
        // REFERENCE VREFA

        ADC0->CTRLB.reg = ADC_CTRLB_RESSEL_16BIT;
        while (ADC0->SYNCBUSY.bit.CTRLB)
            ;

        // Current Sense Calibration On 0A current
        zero_current_offset_calibration(); // Calibrating dc_offset at zero current

        ADC0->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND;
        while (ADC0->SYNCBUSY.bit.INPUTCTRL)
            ;
        // Average 2 samples
        ADC0->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(0x1) | ADC_AVGCTRL_ADJRES(0x1);
        while (ADC0->SYNCBUSY.bit.AVGCTRL)
            ;

        ADC0->INTENSET.reg = ADC_INTENSET_RESRDY;
        ADC0->EVCTRL.reg = ADC_EVCTRL_STARTEI |
                           ADC_EVCTRL_RESRDYEO;

        ADC0->CTRLA.bit.ENABLE = 1;
        while (ADC0->SYNCBUSY.bit.ENABLE)
            ;

        nd::chrono::wait(nd::chrono::durations::milliseconds{1});

        nd::drivers::same5x::Mclk_ctrl::enable(TCC0);
        nd::drivers::same5x::Gclk_pchctrl::enable(TCC0_GCLK_ID, 4); // 200 MHz
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pa20}.enable_alternate_function(nd::drivers::Gpio_alt_functions::G);
        nd::drivers::Gpio_same5x{nd::drivers ::gpio_pins::pa21}.enable_alternate_function(nd::drivers::Gpio_alt_functions::G);
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pa22}.enable_alternate_function(nd::drivers::Gpio_alt_functions::G);
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pa23}.enable_alternate_function(nd::drivers::Gpio_alt_functions::G);

        TCC0->PER.reg = 2000; // 2000 even divisor of 200 Mhz
        while (TCC0->SYNCBUSY.bit.PER)
            ;

        // single slope PWM, v DUAL nepodporuje SWAP a DEAD TIME
        // SWAP for duty cycle inversion, swap0/1 resulting in setting inverse value to value passed to register
        // if i set duty to ccbuf0/1, then duty on output pins 0/1 will be per-duty
        // thats are low side mosfets
        TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM | TCC_WAVE_SWAP0 | TCC_WAVE_SWAP1;
        // TCC0->DRVCTRL.reg = TCC_DRVCTRL_INVEN0 | TCC_DRVCTRL_INVEN1;
        while (TCC0->SYNCBUSY.bit.WAVE)
            ;

        // povoluje generování eventů
        TCC0->EVCTRL.bit.OVFEO = 1;

        TCC0->WEXCTRL.reg = TCC_WEXCTRL_OTMX(0x0)                                                               // otm[0+2] na CC0 -> T1 a T3, otm[1+3] -> T2 a T4 na CC1 prej vhodný pro H bridge, ...
                            | TCC_WEXCTRL_DTIEN0 | TCC_WEXCTRL_DTIEN1 | TCC_WEXCTRL_DTIEN2 | TCC_WEXCTRL_DTIEN3 // 0,1,2,3 mají DTI enabled
                            | TCC_WEXCTRL_DTHS(0x10)                                                            // DTI HighSide = High2Low -> sestupná hrana
                            | TCC_WEXCTRL_DTLS(0x10);                                                           // DTI LowSide = Low2High -> vzestupná hran*/

        TCC0->CCBUF[0].reg = 1000;
        while (!TCC0->STATUS.bit.CCBUFV0)
            ;

        // NRF enabling output 0,1,2,3 and setting 0 and 1 to high
        // Shorting load on NRF flag
        TCC0->DRVCTRL.reg = TCC_DRVCTRL_NRE0 | TCC_DRVCTRL_NRE1 | TCC_DRVCTRL_NRE2 | TCC_DRVCTRL_NRE3 | TCC_DRVCTRL_NRE4 | TCC_DRVCTRL_NRE5 | TCC_DRVCTRL_NRE6 | TCC_DRVCTRL_NRE7 | TCC_DRVCTRL_NRV0 | TCC_DRVCTRL_NRV1;
        // // Enabling NRF event
        // // Enabling Input Event TCEI1
        TCC0->EVCTRL.bit.EVACT1 = 0x7;
        TCC0->EVCTRL.bit.TCEI1 = 1;

        // EVSYS CHANNEL 0 / ADC START
        EVSYS->Channel[0].CHANNEL.reg |= EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
                                         EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TCC0_OVF);
        EVSYS->USER[EVSYS_ID_USER_ADC0_START].reg |= 0x1;

        // EVSYS CHANNEL 1 / NRF
        EVSYS->Channel[1].CHANNEL.reg |= EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
        EVSYS->USER[EVSYS_ID_USER_TCC0_EV_1].reg |= 0x2;

        switching_frequency = 200000000 / TCC0->PER.reg;
        log().debug("Switching frequency = {}", switching_frequency);

        // Disabeling output by NRF
        TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1 | TCC_CTRLA_ENABLE;
        while (TCC0->SYNCBUSY.bit.ENABLE)
            ;
        EVSYS->SWEVT.bit.CHANNEL1 = 1;
    }

    void Current_source::regulator()
    {
        // PID
        static regulator::PID pid_current{Kp, Ki, Kd, out_limit_min, out_limit_max, static_cast<float>(get_switching_frequency()), f_c};

        if ((!TCC0->INTFLAG.bit.FAULT1) && (enabled))
        {
            // Ensures self recovery from Error Warning Watchdog Timer
            log().info("RECOVERY FROM WDT EW");
            TCC0->STATUS.bit.FAULT1 = 1;
        }

        // Error signal
        float error = (-current_request * 0.68f) - current_actual; // Calculating error

        const float error_th = std::clamp(current_amplitude * 0.1f, error_th_min, error_th_max); // Calculating error threshold for smart driving of Hbridge based on actual current and clamp error_th between 0.25 and 1.0 Amps

        // Calculate output
        float duty = pid_current.out_pid(&error); // Soft gain function added

        // // BRIDGE DRIVING
        if (current_request == 0)
        {
            // For zero requested current
            TCC0->CCBUF[2].reg = 0; // PA22 // AH
            TCC0->CCBUF[0].reg = 0; // PA20 // AL // DUTY INVERSED TO SET VAL
            TCC0->CCBUF[3].reg = 0; // PA23 // BH
            TCC0->CCBUF[1].reg = 0; // PA21 // BL // DUTY INVERSED TO SET VAL
            return;
        }

        // Full voltage on bridge LOAD
        duty_A = static_cast<uint16_t>(duty);
        duty_B = static_cast<uint16_t>(TCC0->PER.reg) - duty_A;

        // // Small error
        if (fabsf(error) < fabsf(error_th))
        {
            // Positive Current
            if ((current_request > error_th) && (current_actual > error_th))
            {
                duty_B = out_limit_min;
            }
            // For negative current
            else if ((current_request < error_th) && (current_actual < error_th))
            {
                duty_A = out_limit_min;
            }
        }

        TCC0->CCBUF[2].reg = duty_A; // PA22 // AH
        TCC0->CCBUF[0].reg = duty_A; // PA20 // AL // DUTY INVERSED TO SET VAL
        TCC0->CCBUF[3].reg = duty_B; // PA23 // BH
        TCC0->CCBUF[1].reg = duty_B; // PA21 // BL // DUTY INVERSED TO SET VAL
    }

    void Current_source::adc_irq_handler()
    {
        get_new_current();
        regulator();
        ADC1->INTFLAG.bit.RESRDY = 1;
    }

    void Current_source::handler()
    {
        if (disable_request)
        {
            disable_force();
            disable_request = false;
        }
    }

    void Current_source::disable()
    {
        if (enabled)
        {
            disable_force();
            log().debug("CS: disabled");
        }
    }

    void Current_source::enable()

    {
        if (aron::TempMeassure::overheated)
        {
            log().error("*E  Can't enable, overheated");
            return;
        }

        if (!enabled)
        {
            enable_force();
            log().debug("CS: enabled");
        }
        if (!aron::Position_control::get_position_control_status())
        {
            ac_output ? log().debug("AC amplitude current set {}A", current_amplitude) : log().debug("DC current set {}A", current_amplitude);
        }
        else
        {
            log().debug("Position control ON");
        }
    }

    void Current_source::enable_force()
    {
        TCC0->STATUS.bit.FAULT1 = 1;
        TCC0->STATUS.bit.FAULT0 = 1;
        enabled = true;
        disable_request = false;
    }

    bool Current_source::is_enabled()
    {
        return enabled;
    }

    void Current_source::disable_force()
    {
        // This event activates NRF
        // Disabeling TCC and setting AL BL high
        EVSYS->SWEVT.bit.CHANNEL1 = 1;
        enabled = false;
    }

    bool Current_source::get_ac_status()
    {
        return ac_output;
    }

    void Current_source::get_new_current()
    {
        voltage_adc = nd::utils::digital_to_analog_voltage(12, adc_vref, ADC0->RESULT.reg);
        float current_measured = (voltage_adc - dc_offset) / U_to_I;                             // Convert to actual current
        const float alpha = 0.533529643f;                                                        // Precomputed for 10kHz cutoff at 100kHz sampling
        act_current_filtered = alpha * act_current_filtered + (1.0f - alpha) * current_measured; // Apply low-pass filter (first-order EMA)
        current_actual.store(act_current_filtered);                                              // Store the filtered value
    }

    float Current_source::get_current_actual()
    {
        return (current_actual.load());
    }

    float Current_source::get_current_request()
    {
        return (current_request.load());
    }

    float Current_source::get_current_amplitude()
    {
        return (current_amplitude.load());
    }

    float Current_source::get_adc_vref()
    {
        return (adc_vref);
    }

    uint32_t Current_source::get_switching_frequency()
    {
        return switching_frequency;
    }

    void Current_source::set_current(const float current)
    {
        current_request.store(current);
    }

    void Current_source::set_current_amplitude(const float current)
    {
        current_amplitude.store(current);
        if (!aron::Current_source::get_ac_status())
        {
            current_request.store(current);
            log().debug("DC current set {}A", current_amplitude);
        }
        else
        {
            log().debug("AC amplitude current set {}A", current_amplitude);
        }
    }

    void Current_source::set_ac_output(bool ac_output_request)
    {
        aron::DDS::reset_phase_accumulator();
        ac_output = ac_output_request;
    }

    void Current_source::zero_current_offset_calibration()
    {
        // Average 1024 samples
        ADC0->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(0xA) | ADC_AVGCTRL_ADJRES(0x4);
        while (ADC0->SYNCBUSY.bit.AVGCTRL)
            ;
        // Current Sense Calibration On 0A current
        ADC0->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND;
        while (ADC0->SYNCBUSY.bit.INPUTCTRL)
            ;
        ADC0->CTRLA.bit.ENABLE = 1;
        while (ADC0->SYNCBUSY.bit.ENABLE)
            ;
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        log().debug("CS: dc_offset={}", dc_offset);
        ADC0->SWTRIG.reg = ADC_SWTRIG_START;
        while (ADC0->SYNCBUSY.bit.SWTRIG)
            ;

        while (!ADC0->INTFLAG.bit.RESRDY)
            ;

        uint32_t adc_voltage = ADC0->RESULT.reg;

        float dc_offset_meassured = nd::utils::digital_to_analog_voltage(12, adc_vref, adc_voltage);

        // dc_offset = dc_offset_meassured + 0.075f;
        // dc_offset = dc_offset_meassured + 0.108f;
        dc_offset = adc_vref - dc_offset_meassured + 0.06f;
        // dc_offset = 1.25f;
        // 500mA = 0.06f v konstante
        log().debug("CS: dc_offset={}", dc_offset);

        ADC0->CTRLA.bit.ENABLE = 0;
        // Average 2 samples
        ADC0->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(0x1) | ADC_AVGCTRL_ADJRES(0x1);
        while (ADC0->SYNCBUSY.bit.AVGCTRL)
            ;

        ADC0->INTENSET.reg = ADC_INTENSET_RESRDY;
        ADC0->EVCTRL.reg = ADC_EVCTRL_STARTEI |
                           ADC_EVCTRL_RESRDYEO;

        ADC0->CTRLA.bit.ENABLE = 1;
        while (ADC0->SYNCBUSY.bit.ENABLE)
            ;
    }
}