#pragma once

#include <cstdint>
#include <sam.h>

#include "usart_interface.hpp"
#include "drivers/gpio/gpio_same5x.hpp"



namespace nd::drivers
{
    class Usart_blocking_same5x : public Usart_interface
    {
        public:
            Usart_blocking_same5x(Sercom* const sercom_p,
                         const std::uint8_t clock_generator_num_p,
                         const Gpio_pin rx_pin_p,
                         const Gpio_pin tx_pin_p,
                         const Gpio_alt_functions pins_alt_function_p,
                         const std::uint8_t rxpo_p,
                         const std::uint8_t txpo_p,
                         const std::uint32_t baudrate_p);

            Usart_blocking_same5x(Sercom* const sercom_p,
                         const std::uint8_t clock_generator_num_p,
                         const Gpio_pin rx_pin_p,
                         const Gpio_pin tx_pin_p,
                         const Gpio_alt_functions pins_alt_function_p,
                         const std::uint8_t rxpo_p,
                         const std::uint8_t txpo_p,
                         const std::uint32_t baudrate_p,
                         const Uart_character_size char_size_p,
                         const Uart_stop_bit stop_bit_p,
                         const Uart_parity parity_p,
                         const Uart_data_order data_order_p);

            void enable() const final;
            void disable() const final;
            void send_char(const char c) final;
            [[nodiscard]] std::optional<std::uint8_t> receive_char() final;

        private:
            Sercom* const sercom;
            SercomUsart* const usart;
            const std::uint8_t clock_generator_num;
            const Gpio_same5x rx_pin;
            const Gpio_same5x tx_pin;
            const Gpio_alt_functions pins_alt_function;
            const std::uint8_t rxpo;
            const std::uint8_t txpo;
    };
}
