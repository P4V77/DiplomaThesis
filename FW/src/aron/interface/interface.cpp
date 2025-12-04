#include "interface.hpp"

namespace aron
{

    using namespace nd;
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void Interface::init_led()
    {
        led_green.set_as_output();
        led_yellow.set_as_output();
        led_red.set_as_output();

        led_green.high();
        led_yellow.high();
        led_red.low();
    }
    void Interface::init_button()
    {
        button_1.set_as_input();
        button_1.set_pull_up();
        button_1.enable_input_buffer();

        // TC2 for counting button hold time
        nd::drivers::same5x::Mclk_ctrl::enable(TC2);
        nd::drivers::same5x::Gclk_pchctrl::enable(TC2_GCLK_ID, 1); // 1 MHz
        TC2->COUNT16.CTRLA.reg = TC_CTRLA_PRESCALER_DIV64;
        TC2->COUNT16.CC[0].reg = 20000; // aprox. 3s delay
        TC2->COUNT16.INTENSET.bit.MC0 = 1;
    }
    void Interface::init_communication()
    {
        // Static LoggerCan instance
        // Init UART
        can_logger.disable(); // Setting Canlog
        logger.enable();
        utils::Logger::setLogger(logger);
        logger.info("Aron - modular current power supply");
        logger.debug("Init start");

        // Init CAN
        can_xcvr.enable();
        can.init();
    }

    void Interface::init_can_logger()
    {
        logger.disable();                                                            // Disable UART logger
        can_logger.init(aron::Interface::can, aron::Communication::get_source_id()); // Initialize the static can_logger member
        can_logger.enable();                                                         // Setting Canlog
        can_logger.info("Logs on CAN");                                              // Static LoggerCan instance
    }

    void Interface::init_all()
    {
        init_led();
        init_button();
        init_communication();
    }
    void Interface::button_press()
    {

        TC2->COUNT16.COUNT.reg = 0x0; // Reseting button hold time counter value

        TC2->COUNT16.CTRLA.bit.ENABLE = 1; // Start counting Button Hold Timeu
        while (TC2->COUNT16.SYNCBUSY.bit.ENABLE)
            ;
        while (aron::Interface::button_1.is_set())
        {
            if (!WDT->SYNCBUSY.bit.CLEAR) // This function resets the Watchdog Timer to prevent system reset
            {
                WDT->CLEAR.reg = 0xA5; // WDT RESET VALUE (Writing any other value than 0xA5 resets system)
            }
        }
        TC2->COUNT16.CTRLA.bit.ENABLE = 0;
        while (TC2->COUNT16.SYNCBUSY.bit.ENABLE)
            ;

        aron::Current_source::is_enabled() ? aron::Current_source::disable() : aron::Current_source::enable();
        (aron::Current_source::is_enabled()) ? aron::Interface::led_yellow.high() : aron::Interface::led_yellow.low();
    }
}