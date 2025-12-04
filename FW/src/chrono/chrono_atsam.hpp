#pragma once

#include <cstdint>
#include <chrono>

#include "drivers/systick/systick.hpp"



namespace nd::chrono
{
    struct steady_clock
    {
        using rep        = std::uint32_t;
        using period     = std::milli;
        using duration   = std::chrono::duration<rep, period>;
        using time_point = std::chrono::time_point<steady_clock>;
        static constexpr bool is_steady{true};

        static time_point now() noexcept
        {
            return time_point{duration{drivers::Systick::get()}};
        }
    };
}
