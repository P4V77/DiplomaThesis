#pragma once

#include <cstdlib> // For rand()
#include <cstdint>
#include <array>
#include <chrono>
#include <random> // Include for random number generation
#include <string_view>

#include <fmt/core.h>  // Core formatting functions
#include <fmt/format.h> // fmt::format (if needed)

#include "utils/logging/Logger.hpp"
#include "chrono/Periodic_timer.hpp"

#include "aron/communication/communication.hpp"
#include "aron/output/current_source.hpp" // For Current_source::disable()

#include "command_handler.hpp"


// ID 0 - 250: sources ID
// ID 251: General problem
// ID 252: Reserved problem
// ID 253: Pending ID confirmation
// ID 254: Heartbeat message
// ID 255: Broadcast message

namespace aron
{
    class BusActivity
    {
    public:
        static void process_heartbeat(const std::int32_t &source_id);
        static void cleanup_inactive_sources();
        static bool is_source_active(uint8_t source_id);
        static uint8_t find_next_available_id();
        static void notify_id_change();
        static void set_new_id_from_command(const std::int32_t &value); // Added function
        static void send_heart_beat();

    private:
        static constexpr size_t MAX_SOURCES = 250;
        static constexpr uint32_t TIMEOUT_MS = 5000;
        // Initialize static members
        static inline std::array<bool, MAX_SOURCES> active_sources{};
        static inline std::array<uint32_t, MAX_SOURCES> last_heartbeat_time{};
        static inline uint8_t conflict_id = 0;
    };
}