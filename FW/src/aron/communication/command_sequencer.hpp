#pragma once

#include <cstdint>
#include <array>
#include <string_view>

#include "utils/logging/Logger.hpp"

#include "aron/structs.hpp"
#include "aron/constant_expressions.hpp"

#include "aron/communication/command_handler.hpp"
#include "aron/communication/communication.hpp"
#include "aron/output/dds.hpp"

namespace aron
{
    class CommandSequencer
    {
    public:
        static bool add_sequence_command(const SequenceCommands &sequence);
        static void start_sequence();
        static void clear_sequencer();
        static void sequence_handler();
        static bool is_running() { return sequence_running; }

    private:
        static constexpr size_t MAX_SEQUENCE_COMMANDS = 100;

        static inline std::array<SequenceCommands, MAX_SEQUENCE_COMMANDS> stored_sequences{};
        static inline size_t sequence_command_count{0};
        static inline size_t current_sequence_command{0};
        static inline bool sequence_running{false};
        static inline uint32_t wait_time_ms{0};
        static inline uint32_t wait_start_time{0};
        static inline bool tc3_initialized{false};

        static void execute_sequence_command(const SequenceCommands &cmd); // Changed from execute_next_command
        static void seq_whole_period_counter();
        static void seq_half_period_counter();
        static void seq_time_counter();
        static void duration_handler(const uint32_t &duration_substractor);
        static void call_next_command();
        static void init_tc3_counter();
    };
}