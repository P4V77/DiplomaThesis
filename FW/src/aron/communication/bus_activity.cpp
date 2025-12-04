#include "bus_activity.hpp"

namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }
    void BusActivity::process_heartbeat(const std::int32_t &source_id)
    {
        if (source_id > 250 || source_id == 0)
        {
            log().error("*E HB source ID invalid: {}", source_id);
            return;
        }

        // ID conflict detection
        if ((source_id == Communication::get_source_id()) && active_sources[(uint8_t)source_id])
        {
            // Disable output to avoid conflict
            Current_source::disable();
            log().error("*E ID conflict detected: {}. Disabling output", Communication::get_source_id());

            // Retry conflict resolution up to a limit
            int conflict_retries = 0;
            while (conflict_retries < 5)
            {
                uint8_t new_id = find_next_available_id();
                if (new_id != 0)
                {
                    conflict_id = Communication::get_source_id();
                    Communication::set_source_id(new_id);
                    notify_id_change();
                    break; // Exit loop once conflict is resolved
                }
                conflict_retries++;
                int rand_delay_ms = rand() % 100 + 1; // Random delay between retries
                nd::chrono::wait(nd::chrono::durations::milliseconds{rand_delay_ms});
            }

            if (conflict_retries == 5)
            {
                log().error("*E Max conflict resolution attempts reached.");
                return; // Abort after max retries
            }
        }

        // Update source status and heartbeat time
        active_sources[(uint8_t)source_id] = true;
        last_heartbeat_time[(uint8_t)source_id] = nd::drivers::Systick::get();
    }

    void BusActivity::cleanup_inactive_sources()
    {
        const uint32_t current_time = nd::drivers::Systick::get();

        for (size_t i = 0; i < active_sources.size(); ++i)
        {
            if (active_sources[i] && (current_time - last_heartbeat_time[i] > TIMEOUT_MS))
            {
                active_sources[i] = false;
                log().info("Source {} timed out", i);
            }
        }
    }

    bool BusActivity::is_source_active(uint8_t source_id)
    {
        if (source_id >= active_sources.size())
        {
            return false;
        }
        return active_sources[source_id];
    }

    uint8_t BusActivity::find_next_available_id()
    {
        // Try to find the next available ID
        for (uint8_t id = 1; id <= MAX_SOURCES; ++id)
        {
            if (!active_sources[id])
            {
                return id; // Return the first available ID
            }
        }
        // If no available ID is found, log the error
        log().error("*E No available IDs found.");
        return 0; // No available IDs
    }

    void BusActivity::notify_id_change()
    {
        std::array<char, 64> buffer; // Static buffer
        auto [iterator1, count1] = fmt::format_to_n(buffer.begin(), buffer.size(),
                                                    "ID {} set to ID {}",
                                                    conflict_id, aron::Communication::get_source_id());
        const std::string_view msg{buffer.begin(), buffer.begin() + count1};
        log().info("{}", msg);
        Communication::send_can_message(msg);
    }

    void BusActivity::set_new_id_from_command(const std::int32_t &value)
    {
        if (value > 250 || value == 0)
        {
            log().error("*E Source ID invalid: {}", value);
            return;
        }

        // Check if requested ID is already in use
        if (is_source_active(static_cast<uint8_t>(value)))
        {
            log().error("*E Requested ID {} already in use", value);
            return;
        }
        Communication::set_source_id(static_cast<uint8_t>(value));
    }
    void BusActivity::send_heart_beat()
    {
        std::array<char, 64> buffer; // Static buffer
        auto [iterator1, count1] = fmt::format_to_n(buffer.begin(), buffer.size(),
                                                    "254HB{}",
                                                    aron::Communication::get_source_id());
        const std::string_view msg{buffer.begin(), buffer.begin() + count1};
        Communication::send_can_message(msg);
    }
}
