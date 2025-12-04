#pragma once

#include <cstdint>
#include <array>
#include <cctype>
#include <charconv>
#include <unordered_map>
#include <functional>
#include <string_view> // Include string_view
#include <sam.h>

#include "utils/logging/Logger.hpp"
#include "utils/fifo.hpp"
#include "chrono/Timeout_timer.hpp"
#include "chrono/Wait.hpp"
#include "chrono/Periodic_timer.hpp"
#include "drivers/can/can_interface.hpp"

#include "aron/output/dds.hpp"
#include "aron/output/current_source.hpp"
#include "aron/communication/command_sequencer.hpp"
#include "aron/communication/command_handler.hpp"

// ID 0 - 250: sources ID
// ID 251: General problem
// ID 252: Reserved problem
// ID 253: Pending ID confirmation
// ID 254: Heartbeat message
// ID 255: Broadcast message

namespace aron
{
    using namespace UniqueMessageIDs;
    class Communication
    {
    public:
        static void init(nd::drivers::CAN_interface &can_p, const std::uint8_t id_p);
        static void uart_rx_irq_handler();
        static void rx_handler();
        static void send_can_message(const std::string_view msg);
        static void send_can_message(const std::string_view msg, const uint32_t &id_from_msg);
        
        static void process_message(const std::string_view msg, const bool redirect_to_can);


        static void set_source_id(uint8_t id_p);
        static void erase_eeprom_page();
        static void setting_id_after_programming();
        static void set_source_id_to_eeprom(uint8_t id_p);
        static uint8_t get_source_id();


    private:
        // ** Messages handling **
        static inline nd::drivers::CAN_interface *can{nullptr};
        static inline uint8_t *eeprom_addr_p{reinterpret_cast<uint8_t *>(SEEPROM_ADDR)};                     // Assigning pointer to the SmartEEPROM
        static inline uint8_t *eeprom_id_set{reinterpret_cast<uint8_t *>(SEEPROM_ADDR + sizeof(uint8_t *))}; // Assigning pointer to the SmartEEPROM
        static inline std::uint8_t id{};
        static inline std::uint8_t &id_eeprom{*eeprom_addr_p}; // Declaring vairable on SmartEEPROM
        static inline std::uint8_t uart_msg_ptr{};
        static inline nd::utils::Fifo<std::uint8_t, 512> uart_rx_fifo{};
        static inline std::array<char, 16> uart_msg_buff{};
        static inline nd::chrono::Timeout_timer uart_rx_timeout{nd::chrono::durations::seconds{1}};

        static void uart_handler();
        static void can_handler();
        static void find_first_letter(const std::string_view msg, uint8_t *first_letter_position);
        static int16_t eval_id(const std::string_view msg);

    };
}
