#pragma once

#include <array>
#include <string_view>
#include <cstdint>
#include <functional>

#include "utils/logging/Logger.hpp"

#include "aron/structs.hpp"
#include "aron/constant_expressions.hpp"
#include "aron/output/current_source.hpp"
#include "aron/output/dds.hpp"
#include "aron/output/position_control.hpp"

#include "command_sequencer.hpp"
#include "communication.hpp"
#include "bus_activity.hpp"

namespace aron
{
    class CommandHandler
    {
    public:
        static void message_handler(const std::string_view &cmd, const std::int32_t &value);
        static void command_exec(const std::string_view &cmd, const std::int32_t &value);
        static void cleanup_inactive_sources();

    private:
        // Waveform CommandsshowDevDebugOutputshowDevDebugOutput
        static void set_current_amplitude_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void init_harm_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void init_triangle_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void init_square_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void set_ac_offset_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void set_ac_duty_cycle_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void set_ac_ramp_time(const std::string_view &cmd, const std::int32_t &value);
        static void set_phase_shift_cmd(const std::string_view &cmd, const std::int32_t &value);

        // Output Commands
        static void enable_dc_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void enable_ac_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void enable_position_cmd(const std::string_view &cmd, const std::int32_t &value);

        // Position Commands
        static void set_position_amplitude_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void set_position_offset_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void can_logging(const std::string_view &, const std::int32_t &value);

        // Communication Commands
        static void set_new_id_from_command(const std::string_view &cmd, const std::int32_t &value);
        static void process_hb_message(const std::string_view &cmd, const std::int32_t &value);

        // Sequence Commands
        static void fill_sequence_structure(const std::string_view &cmd, const std::int32_t &value);
        static void new_sequence_detected_command(const std::string_view &cmd, const std::int32_t &value);
        static void start_stored_sequences_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void add_to_sequence();
        static void clear_all_sequences_cmd(const std::string_view &cmd, const std::int32_t &value);
        static void stop_filling_current_sequence(const std::string_view &cmd, const std::int32_t &value);
        static void position_calibration_cmd(const std::string_view &, const std::int32_t &);
        // Helpers
        static bool validate_command(const std::string_view &cmd, const std::int32_t &value);
        static inline bool filling_sequence{false};

        // Sequence filling struct
        static inline SequenceCommands cmd_sequence{};
    };
}