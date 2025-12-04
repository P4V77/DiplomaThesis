#include "usart_blocking_same5x.hpp"

#include <cmath>

#include "drivers/sam5x/mclk.hpp"
#include "drivers/sam5x/gclk.hpp"



namespace nd::drivers
{
    Usart_blocking_same5x::Usart_blocking_same5x(Sercom* const sercom_p,
                               const std::uint8_t clock_generator_num_p,
                               const Gpio_pin rx_pin_p,
                               const Gpio_pin tx_pin_p,
                               const Gpio_alt_functions pins_alt_function_p,
                               const std::uint8_t rxpo_p,
                               const std::uint8_t txpo_p,
                               const std::uint32_t baudrate_p)
        : Usart_interface(baudrate_p),
          sercom{sercom_p},
          usart{&sercom->USART},
          clock_generator_num{clock_generator_num_p},
          rx_pin{rx_pin_p},
          tx_pin{tx_pin_p},
          pins_alt_function{pins_alt_function_p},
          rxpo{rxpo_p},
          txpo{txpo_p}
    {}

    Usart_blocking_same5x::Usart_blocking_same5x(Sercom* const sercom_p,
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
                               const Uart_data_order data_order_p)
        : Usart_interface(baudrate_p, char_size_p, stop_bit_p, parity_p, data_order_p),
          sercom{sercom_p},
          usart{&sercom->USART},
          clock_generator_num{clock_generator_num_p},
          rx_pin{rx_pin_p},
          tx_pin{tx_pin_p},
          pins_alt_function{pins_alt_function_p},
          rxpo{rxpo_p},
          txpo{txpo_p}
    {}

    void Usart_blocking_same5x::enable() const
    {
        rx_pin.enable_alternate_function(pins_alt_function);
        tx_pin.enable_alternate_function(pins_alt_function);

        drivers::same5x::Mclk_ctrl::enable(sercom);
        drivers::same5x::Gclk_pchctrl::enable(drivers::same5x::Gclk_pchctrl::get_id(sercom), clock_generator_num);


        const auto use_fractional_generator{static_cast<bool>(SystemCoreClock % baudrate)};

        // Asynchronous, rxpo for RX, txpo for TX, internal clock, 16x oversampling
        sercom->USART.CTRLA.reg = ((data_order == Uart_data_order::LSB_FIRST) ? 1 : 0) << SERCOM_USART_CTRLA_DORD_Pos |
                                  SERCOM_USART_CTRLA_FORM((parity != Uart_parity::NONE) ? 1 : 0) |
                                  SERCOM_USART_CTRLA_RXPO(rxpo) |
                                  SERCOM_USART_CTRLA_TXPO(txpo) |
                                  SERCOM_USART_CTRLA_SAMPR((use_fractional_generator) ? 1 : 0) |
                                  SERCOM_USART_CTRLA_MODE(1);

        const auto char_size_val{[&]() -> std::uint8_t
            {
                switch (char_size) {
                    case Uart_character_size::CHAR_SIZE_5:
                        return 5;
                    case Uart_character_size::CHAR_SIZE_6:
                        return 6;
                    case Uart_character_size::CHAR_SIZE_7:
                        return 7;
                    case Uart_character_size::CHAR_SIZE_9:
                        return 1;
                    case Uart_character_size::CHAR_SIZE_8:
                    default:
                        return 0;
                }
            }()
        };

        // Enable RX, TX
        sercom->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN |
                                  ((parity == Uart_parity::ODD) ? 1 : 0) << SERCOM_USART_CTRLB_PMODE_Pos |
                                  ((stop_bit == Uart_stop_bit::STOP_BIT_2) ? 1 : 0) << SERCOM_USART_CTRLB_SBMODE_Pos |
                                  SERCOM_USART_CTRLB_CHSIZE(char_size_val);
        while (sercom->USART.SYNCBUSY.bit.CTRLB);

        if (use_fractional_generator)
        {
            float mul_ratio = float(SystemCoreClock)/(float(baudrate) * 16.0f);
            float fp = (mul_ratio - static_cast<float>(static_cast<std::uint8_t>(mul_ratio))) * 8.0f;

            sercom->USART.BAUD.reg = SERCOM_USART_BAUD_FRAC_BAUD(static_cast<std::uint16_t>(mul_ratio)) |
                                     SERCOM_USART_BAUD_FRAC_FP(static_cast<std::uint8_t>(std::lround(fp)));
        }
        else
        {
            const auto baud = static_cast<uint16_t>(65536.0f * (1.0f - 16.0f * (static_cast<float>(baudrate)/static_cast<float>(SystemCoreClock))));
            sercom->USART.BAUD.reg = baud;
        }

        sercom->USART.CTRLA.bit.ENABLE = 1;
        while (sercom->USART.SYNCBUSY.bit.ENABLE);
    }

    void Usart_blocking_same5x::disable() const
    {
        sercom->USART.CTRLA.bit.ENABLE = 0;
        while (sercom->USART.SYNCBUSY.bit.ENABLE);

        drivers::same5x::Gclk_pchctrl::disable(drivers::same5x::Gclk_pchctrl::get_id(sercom));
        drivers::same5x::Mclk_ctrl::disable(sercom);

        rx_pin.disable_alternate_function();
        tx_pin.disable_alternate_function();
    }

    void Usart_blocking_same5x::send_char(const char c)
    {
        while (!sercom->USART.INTFLAG.bit.DRE);

        sercom->USART.DATA.reg = c;
    }

    std::optional<std::uint8_t> Usart_blocking_same5x::receive_char()
    {
        while (!sercom->USART.INTFLAG.bit.RXC)
        {}

        return sercom->USART.DATA.reg;
    }
}
