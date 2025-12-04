#pragma once

#include <string_view>
#include <span>
#include <optional>
#include <concepts>



namespace nd::drivers
{
    enum class Uart_character_size
    {
        CHAR_SIZE_5,
        CHAR_SIZE_6,
        CHAR_SIZE_7,
        CHAR_SIZE_8,
        CHAR_SIZE_9,
    };

    enum class Uart_stop_bit
    {
        STOP_BIT_1,
        STOP_BIT_2,
    };

    enum class Uart_parity
    {
        NONE,
        EVEN,
        ODD,
    };

    enum class Uart_data_order
    {
        MSB_FIRST,
        LSB_FIRST,
    };

    class Usart_interface
    {
        public:
            virtual ~Usart_interface() = default;

            explicit Usart_interface(const std::uint32_t baudrate_p)
                : baudrate{baudrate_p}
            {}

            Usart_interface(const std::uint32_t baudrate_p,
                            const Uart_character_size char_size_p,
                            const Uart_stop_bit stop_bit_p,
                            const Uart_parity parity_p,
                            const Uart_data_order data_order_p)
                : baudrate{baudrate_p},
                  char_size{char_size_p},
                  stop_bit{stop_bit_p},
                  parity{parity_p},
                  data_order{data_order_p}
            {}

            virtual void enable() const = 0;
            virtual void disable() const = 0;
            virtual void send_char(const char c) = 0;
            [[nodiscard]] virtual std::optional<std::uint8_t> receive_char() = 0;

            void enable(const std::uint32_t baudrate_p)
            {
                baudrate = baudrate_p;
                enable();
            }

            void send(std::string_view sv)
            {
                for (const auto c : sv)
                {
                    send_char(c);
                }
            }

            template <std::integral T>
            requires (sizeof(T) == 1)
            void send(std::span<T> data)
            {
                for (const auto c : data)
                {
                    send_char(static_cast<std::uint8_t>(c));
                }
            }

            void send(const std::uint8_t* const data, const std::uint32_t len)
            {
                for (uint32_t i = 0; i < len; ++i)
                {
                    send_char(data[i]);
                }
            }

            [[nodiscard]] std::uint32_t get_baudrate() const { return baudrate; };

        protected:
            std::uint32_t baudrate{9'600};
            const Uart_character_size char_size{Uart_character_size::CHAR_SIZE_8};
            const Uart_stop_bit stop_bit{Uart_stop_bit::STOP_BIT_1};
            const Uart_parity parity{Uart_parity::NONE};
            const Uart_data_order data_order{Uart_data_order::LSB_FIRST};
    };

    template<typename T>
    concept Usart_driver = std::derived_from<T, Usart_interface>;
}
