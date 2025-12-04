#pragma once

/*
 * Compatible with TCAN334 and TCAN334G
 * - normal, standby and shutdown modes
 */

#include "drivers/gpio/gpio_interface.hpp"



namespace nd::drivers
{
    template<typename Gpio_driver>
    class TCAN334
    {
        public:
            TCAN334(const Gpio_pin& pin_standby, const Gpio_pin& pin_shutdown)
                    : standby{pin_standby}, shutdown{pin_shutdown}
            {}

            void enable() const
            {
                standby.set_as_output();
                shutdown.set_as_output();
                set_normal_mode();
            }

            void disable() const
            {
                standby.set_as_input();
                shutdown.set_as_input();
            }

            void set_normal_mode() const
            {
                standby.low();
                shutdown.low();
            }

            void set_standby_mode() const
            {
                standby.high();
                shutdown.low();
            }

            void set_shutdown_mode() const
            {
                standby.low();
                shutdown.high();
            }

        private:
            Gpio_driver standby;
            Gpio_driver shutdown;
    };
}
