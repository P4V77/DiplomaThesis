#pragma once

#include "Logger.hpp"
#include "drivers/usart/usart_blocking_same5x.hpp"
#include "drivers/gpio/gpio_same5x.hpp"



namespace nd::utils
{
    class LoggerUart : public Logger
    {
        public:
            LoggerUart(Sercom* sercom_p,
                       const drivers::Gpio_pin rx_pin_p,
                       const drivers::Gpio_pin tx_pin_p,
                       const std::uint32_t baudrate_p,
                       const drivers::Gpio_alt_functions pins_alt_function_p,
                       const std::uint8_t rxpo_p,
                       const std::uint8_t txpo_p,
                       const std::uint8_t clock_generator_p)
                : uart{sercom_p, clock_generator_p, rx_pin_p, tx_pin_p, pins_alt_function_p, rxpo_p, txpo_p, baudrate_p}
            {
            }

            void enable()
            {
                uart.enable();
                info("Logger initialized");
            }

            void disable()
            {
                info("Logger disabled");
                uart.disable();
            }

        public:
            drivers::Usart_blocking_same5x uart;

            void log(buff_t& buffer, const std::uint_fast32_t length) override
            {
                uart.send(buffer.data(), length);
            }
    };
}
