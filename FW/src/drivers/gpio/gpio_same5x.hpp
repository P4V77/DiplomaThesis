#pragma once

#include <cstdint>
#include <sam.h>

#include "gpio_interface.hpp"

namespace nd::drivers
{
    namespace gpio_pins
    {
        constexpr Gpio_pin pa00{0, 0};
        constexpr Gpio_pin pa01{0, 1};
        constexpr Gpio_pin pa02{0, 2};
        constexpr Gpio_pin pa03{0, 3};
        constexpr Gpio_pin pa04{0, 4};
        constexpr Gpio_pin pa05{0, 5};
        constexpr Gpio_pin pa06{0, 6};
        constexpr Gpio_pin pa07{0, 7};
        constexpr Gpio_pin pa08{0, 8};
        constexpr Gpio_pin pa09{0, 9};
        constexpr Gpio_pin pa10{0, 10};
        constexpr Gpio_pin pa11{0, 11};
        constexpr Gpio_pin pa12{0, 12};
        constexpr Gpio_pin pa13{0, 13};
        constexpr Gpio_pin pa14{0, 14};
        constexpr Gpio_pin pa15{0, 15};
        constexpr Gpio_pin pa16{0, 16};
        constexpr Gpio_pin pa17{0, 17};
        constexpr Gpio_pin pa18{0, 18};
        constexpr Gpio_pin pa19{0, 19};
        constexpr Gpio_pin pa20{0, 20};
        constexpr Gpio_pin pa21{0, 21};
        constexpr Gpio_pin pa22{0, 22};
        constexpr Gpio_pin pa23{0, 23};
        constexpr Gpio_pin pa24{0, 24};
        constexpr Gpio_pin pa25{0, 25};
        constexpr Gpio_pin pa27{0, 27};
        constexpr Gpio_pin pa30{0, 30};
        constexpr Gpio_pin pa31{0, 31};

        constexpr Gpio_pin pb00{1, 0};
        constexpr Gpio_pin pb01{1, 1};
        constexpr Gpio_pin pb02{1, 2};
        constexpr Gpio_pin pb03{1, 3};
        constexpr Gpio_pin pb04{1, 4};
        constexpr Gpio_pin pb05{1, 5};
        constexpr Gpio_pin pb06{1, 6};
        constexpr Gpio_pin pb07{1, 7};
        constexpr Gpio_pin pb08{1, 8};
        constexpr Gpio_pin pb09{1, 9};
        constexpr Gpio_pin pb10{1, 10};
        constexpr Gpio_pin pb11{1, 11};
        constexpr Gpio_pin pb12{1, 12};
        constexpr Gpio_pin pb13{1, 13};
        constexpr Gpio_pin pb14{1, 14};
        constexpr Gpio_pin pb15{1, 15};
        constexpr Gpio_pin pb16{1, 16};
        constexpr Gpio_pin pb17{1, 17};
        constexpr Gpio_pin pb22{1, 22};
        constexpr Gpio_pin pb23{1, 23};
        constexpr Gpio_pin pb30{1, 30};
        constexpr Gpio_pin pb31{1, 31};
    }

    enum class Gpio_alt_functions : std::uint8_t
    {
        A = 0,
        B = 1,
        C = 2,
        D = 3,
        E = 4,
        F = 5,
        G = 6,
        H = 7,
        I = 8,
        J = 9,
        K = 10,
        L = 11,
        M = 12,
        N = 13,
    };

    class Gpio_same5x : public Gpio_interface
    {
    public:
        explicit Gpio_same5x(const Gpio_pin &gpio_pin)
            : port{PORT->Group[gpio_pin.port_group]},
              pin_pos{gpio_pin.pin_num},
              pin_mask{1u << pin_pos}
        {
        }

        Gpio_same5x(const Gpio_pin &gpio_pin, const bool inverted_p)
            : port{PORT->Group[gpio_pin.port_group]},
              pin_pos{gpio_pin.pin_num},
              pin_mask{1u << pin_pos},
              inverted{inverted_p}
        {
        }

        void set_as_input() const final
        {
            port.DIRCLR.reg = pin_mask;
        }

        void set_as_output() const final
        {
            port.DIRSET.reg = pin_mask;
        }

        void enable_input_buffer() const final
        {
            port.PINCFG[pin_pos].bit.INEN = 1;
        }

        void disable_input_buffer() const final
        {
            port.PINCFG[pin_pos].bit.INEN = 0;
        }

        void set_pull_up() const final
        {
            port.PINCFG[pin_pos].bit.PULLEN = 1;
            high();
        }

        void set_pull_down() const final
        {
            port.PINCFG[pin_pos].bit.PULLEN = 1;
            low();
        }

        void disable_pull() const final
        {
            port.PINCFG[pin_pos].bit.PULLEN = 0;
        }

        void set_driver_strength(const Gpio_driver_strengths strength) const final
        {
            port.PINCFG[pin_pos].bit.DRVSTR = (strength == Gpio_driver_strengths::STRONGER);
        }

        void low() const final
        {
            port.OUTCLR.reg = pin_mask;
        }

        void high() const final
        {
            port.OUTSET.reg = pin_mask;
        }

        void toggle() const final
        {
            port.OUTTGL.reg = pin_mask;
        }

        [[nodiscard]] bool is_set() const final
        {
            return inverted == !(port.IN.reg & pin_mask);
        }

        void enable_alternate_function(const Gpio_alt_functions alternate_function) const
        {
            // Select alternate function
            if (pin_pos % 2 == 0)
            {
                // Even pins (0, 2, 4, ...)
                port.PMUX[pin_pos / 2].bit.PMUXE = static_cast<std::uint8_t>(alternate_function) & 0x0fu;
            }
            else
            {
                // Odd pins (1, 3, 5, ...)
                port.PMUX[(pin_pos - 1) / 2].bit.PMUXO = static_cast<std::uint8_t>(alternate_function) & 0x0fu;
            }

            // Enable multiplexing
            port.PINCFG[pin_pos].bit.PMUXEN = 1;
        }

        void disable_alternate_function() const
        {
            port.PINCFG[pin_pos].bit.PMUXEN = 0;
        }

        [[nodiscard]] std::uint8_t get_pin_num() const final
        {
            return pin_pos;
        }

    private:
        PortGroup &port;
        const std::uint8_t pin_pos;
        const std::uint32_t pin_mask;
        const bool inverted{false};
    };
}
