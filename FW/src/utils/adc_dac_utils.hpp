#pragma once

#include <cstdint>



namespace nd::utils
{
    template <std::uint32_t resolution, float vref>
    inline constexpr float get_lsb()
    {
        constexpr std::uint32_t bits{1u << resolution};
        return vref / float{bits};
    }

    template <std::uint32_t resolution, float vref>
    inline constexpr float digital_to_analog_voltage(const std::uint32_t digital_value)
    {
        return static_cast<float>(digital_value) * get_lsb<resolution, vref>();
    }

    template <std::uint32_t resolution, float vref>
    inline constexpr std::uint32_t analog_voltage_to_digital(const float voltage)
    {
        return static_cast<std::uint32_t>(voltage / get_lsb<resolution, vref>());
    }

    inline constexpr float get_lsb(std::uint32_t resolution, float vref)
    {
        const std::uint32_t bits{1u << resolution};
        return vref / static_cast<float>(bits);
    }

    inline constexpr float digital_to_analog_voltage(std::uint32_t resolution, float vref, const std::uint32_t digital_value)
    {
        return static_cast<float>(digital_value) * get_lsb(resolution, vref);
    }

    inline constexpr std::uint32_t analog_voltage_to_digital(std::uint32_t resolution, float vref, const float voltage)
    {
        return static_cast<std::uint32_t>(voltage / get_lsb(resolution, vref));
    }

    inline constexpr float voltage_divider_scaling(const std::uint32_t R1, const std::uint32_t R2)
    {
        const auto R1f{static_cast<float>(R1)};
        const auto R2f{static_cast<float>(R2)};
        return R2f / (R1f + R2f);
    }
}
