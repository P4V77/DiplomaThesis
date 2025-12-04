#pragma once

#include "Logger.hpp"

#include "drivers/can/can_same5x.hpp" // Changed include
#include "drivers/can/can_interface.hpp"

#include "aron/communication/communication.hpp"

namespace nd::utils
{
    class LoggerCan : public Logger
    {
    public:
        LoggerCan() = default; // Default constructor

        // The init() method that initializes the logger
        void init(drivers::CAN_ATSAM_C_E &can_interface_p, uint32_t can_id_p)
        {
            can = &can_interface_p;
            id_can = can_id_p;
            can_logs_enabled = false; // Make sure it's initially disabled

            info("CAN Logger initialized");
        }

        void enable()
        {
            can_logs_enabled = true;
            info("CAN Logger enabled");
        }

        void disable()
        {
            can_logs_enabled = false;
            info("CAN Logger disabled");
        }

    public:
        drivers::CAN_ATSAM_C_E *can;
        uint32_t id_can;
        bool can_logs_enabled = false;

        void log(buff_t &buffer, const std::uint_fast32_t length) override
        {
            if (!can_logs_enabled)
            {
                // If CAN logging is disabled, skip logging.
                return;
            }

            // Temporary buffer for formatted output
            char msg_buffer[64]; // Ensure size is enough for expected log messages
            // Format the message using snprintf
            int formatted_length = std::snprintf(msg_buffer, sizeof(msg_buffer), "%.*s", static_cast<int>(length), buffer.data());
            if (formatted_length < 0 || formatted_length > 63)
            {
                error("Formatting error in snprintf.");
                return;
            }
            // Create a string_view from the formatted message
            std::string_view formatted_msg{msg_buffer, static_cast<size_t>(formatted_length)};

            // Prepare CAN message
            nd::drivers::CAN_message can_msg{};
            can_msg.id = id_can;
            can_msg.is_fd = true;
            can_msg.size = static_cast<uint8_t>(formatted_length);

            // Copy formatted message into CAN message data buffer
            std::copy(formatted_msg.begin(), formatted_msg.end(), can_msg.data.begin());

            // Send CAN message
            can->send(can_msg);
        }
    };
}
