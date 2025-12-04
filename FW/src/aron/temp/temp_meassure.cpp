#include "temp_meassure.hpp"

namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void TempMeassure::init()
    {
        ADC1->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(0x6) | ADC_AVGCTRL_ADJRES(0x4);
        while (ADC1->SYNCBUSY.bit.AVGCTRL)
            ;
        // Temp Sensor Input
        ADC1->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS_AIN0 | ADC_INPUTCTRL_MUXNEG_GND;
        while (ADC1->SYNCBUSY.bit.INPUTCTRL)
            ;
        ADC1->CTRLA.bit.ONDEMAND = 1;
        ADC1->EVCTRL.reg = ADC_EVCTRL_STARTEI;            // Start new conversion
        EVSYS->USER[EVSYS_ID_USER_ADC1_START].reg = 0x0F; // EVSYS Channel 14
        ADC1->CTRLA.bit.ENABLE = 1;
        while (ADC1->SYNCBUSY.bit.ENABLE)
            ;
    }

    void TempMeassure::adc1_temperature_read()
    {
        // If multiplexer is set for Temperature sensor
        auto const adc_voltage = nd::utils::digital_to_analog_voltage(12, aron::Current_source::get_adc_vref(), ADC1->RESULT.reg);
        /* CONSTANTS FOR AD CONVERSION */
        static constexpr float TEMP_V_OUT_0C{0.4f};
        static constexpr float TEMP_SENSITIVITY{0.01f};
        static float temperature{0.0f};
        temperature = (adc_voltage - TEMP_V_OUT_0C) / TEMP_SENSITIVITY;

        std::array<char, 64> buffer; // Static buffer
        auto [iterator1, count1] = fmt::format_to_n(buffer.begin(), buffer.size(),
                                                    "{}ID Temp: {} C",
                                                    aron::Communication::get_source_id(), temperature);
        const std::string_view msg{buffer.begin(), buffer.begin() + count1};

        if (temperature > 115.0f)
        {
            constexpr std::string_view msg_error = {"*E: CRITICAL TEMP 115 °C EXCEEDED !!!!"};
        // if (temperature > 90.0f)
        // {
            // constexpr std::string_view msg_error = {"*E: CRITICAL TEMP 90 °C EXCEEDED !!!!"};
            overheated = true;
            aron::Current_source::disable();
            aron::Interface::led_red.high();
            aron::Interface::led_yellow.low();
            aron::Interface::led_green.low();
            log().debug("{}", msg_error);                     // Send the message to UART
            aron::Communication::send_can_message(msg_error); // Send the message via CAN
        }

        if (temperature < 70.0f)
        {
            overheated = false;
        }

        log().debug("{}", msg);                     // Send the message to UART
        aron::Communication::send_can_message(msg); // Send the message via CAN
    }

    void TempMeassure::adc1_1_handler()
    {
        switch (ADC1->INPUTCTRL.reg)
        {
            // Temp Sensor
        case (ADC_INPUTCTRL_MUXPOS_AIN0 | ADC_INPUTCTRL_MUXNEG_GND):
        {
            adc1_temperature_read();
            // Defualtly set for Connector 1 (position ADC)
            ADC1->SWTRIG.bit.FLUSH = 1;
            ADC1->INPUTCTRL.reg = (ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND);
            break;
        }
            // Connector 1
        case (ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND):
        {
            aron::Position_control::adc1_position_handler();
            break;
        }

            // Connector 2
        case (ADC_INPUTCTRL_MUXPOS_AIN7 | ADC_INPUTCTRL_MUXNEG_GND):
        {
            float adc1_result_temp = (nd::utils::digital_to_analog_voltage(12, aron::Current_source::get_adc_vref(), ADC1->RESULT.reg));
            log().debug("{}", adc1_result_temp);
            ADC1->SWTRIG.bit.FLUSH = 1;
            ADC1->INPUTCTRL.reg = (ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND);
            break;
        }
        default:
        {
            // Defualtly set for Connector 1 (position ADC)
            ADC1->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND;
            ADC1->SWTRIG.bit.FLUSH = 1;
            break;
        }
        }
        ADC1->INTFLAG.bit.RESRDY = 1;
    }
}
