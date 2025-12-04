#pragma once

#include <cstdint>
#include <type_traits>



namespace nd::drivers
{
    struct Gpio_pin
    {
        const std::uint8_t port_group{};
        const std::uint8_t pin_num{};
    };

    enum class Gpio_driver_strengths : std::uint8_t
    {
        NORMAL,
        STRONGER,
    };

    class Gpio_interface
    {
        public:
            virtual void set_as_input() const = 0;
            virtual void set_as_output() const = 0;
            virtual void enable_input_buffer() const = 0;
            virtual void disable_input_buffer() const = 0;
            virtual void set_pull_up() const = 0;
            virtual void set_pull_down() const = 0;
            virtual void disable_pull() const = 0;
            virtual void set_driver_strength(const Gpio_driver_strengths strength) const = 0;
            virtual void low() const = 0;
            virtual void high() const = 0;
            virtual void toggle() const = 0;
            [[nodiscard]] virtual bool is_set() const = 0;
            [[nodiscard]] virtual std::uint8_t get_pin_num() const = 0;
    };

    class Gpio_same5x;
    class Gpio_nrf9160;

    template<typename T>
    concept Gpio_driver = std::is_same_v<T, drivers::Gpio_same5x> || std::is_same_v<T, drivers::Gpio_nrf9160>;
}
