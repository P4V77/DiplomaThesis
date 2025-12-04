#pragma once

#include <cstdlib> // For rand()
#include <cstdint>
#include <array>
#include <chrono>
#include <random> // Include for random number generation
#include <string_view>

#include "chrono/Periodic_timer.hpp"
#include "utils/logging/Logger.hpp"

#include "aron/output/current_source.hpp" // For Current_source::disable()
#include "aron/communication/command_handler.hpp"
#include "aron/communication/communication.hpp"

namespace aron
{
    class TempMeassure
    {
    public:
        static void init();
        static void adc1_1_handler();
        static inline bool overheated{false};

    private:
        static void adc1_temperature_read();
    };
}