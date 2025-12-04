#include "command_handler.hpp"
#include <cstring> // For std::strncpy
using namespace aron::CommandConstants;
namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void CommandHandler::message_handler(const std::string_view &cmd, const std::int32_t &value)
    {
        if (!validate_command(cmd, value))
        {
            return;
        }

        if (filling_sequence)
        {
            // Filling sequence
            fill_sequence_structure(cmd, value);
        }
        else
        {
            // Non sequence command or sequence start/clear
            command_exec(cmd, value);
        }
    }

    bool CommandHandler::validate_command(const std::string_view &cmd, const std::int32_t &value)
    {
        /* Validation array for checking correct values of commands */
        static const std::array<ValidationRuleInt32, 17> validation_rules_int32 = {
            /* Checking if set output value is 0 or 1 */
            ValidationRuleInt32{"D", &SequenceCommands::value_output, [](std::int32_t v)
                                { return v == OUTPUT_DISABLED || v == OUTPUT_ENABLED; }},
            ValidationRuleInt32{"A", &SequenceCommands::value_output, [](std::int32_t v)
                                { return v == OUTPUT_DISABLED || v == OUTPUT_ENABLED; }},
            ValidationRuleInt32{"P", &SequenceCommands::value_output, [](std::int32_t v)
                                { return v == OUTPUT_DISABLED || v == OUTPUT_ENABLED; }},
            /* Checking value of duty cycle for square in % */
            ValidationRuleInt32{"C", &SequenceCommands::value_square_duty, [](std::int32_t v)
                                { return v >= DUTY_CYCLE_MIN && v <= DUTY_CYCLE_MAX; }},
            /* Checking value of ramp slope in % */
            ValidationRuleInt32{"RT", &SequenceCommands::value_square_ramp, [](std::int32_t v)
                                { return v >= RAMP_MIN && v <= RAMP_MAX; }},
            /* Checking value of phase */
            ValidationRuleInt32{"F", &SequenceCommands::value_phaseshift, [](std::int32_t v)
                                { return v >= PHASE_MIN && v <= PHASE_MAX; }},
            /* Checking value of offset */

            ValidationRuleInt32{"O", &SequenceCommands::value_ac_offset, [](std::int32_t v)
                                { return v >= OFFSET_MIN && v <= OFFSET_MAX; }},
            /* Checking value of current */
            ValidationRuleInt32{"I", &SequenceCommands::value_current, [](std::int32_t v)
                                { return v >= CURRENT_MIN && v <= CURRENT_MAX; }},
            /* Checking value of frequency for waveforms */
            ValidationRuleInt32{"H", &SequenceCommands::value_waveform_frequency, [](std::int32_t v)
                                { return v >= FREQUENCY_MIN && v <= FREQUENCY_MAX; }},
            ValidationRuleInt32{"T", &SequenceCommands::value_waveform_frequency, [](std::int32_t v)
                                { return v >= FREQUENCY_MIN && v <= FREQUENCY_MAX; }},
            ValidationRuleInt32{"S", &SequenceCommands::value_waveform_frequency, [](std::int32_t v)
                                { return v >= FREQUENCY_MIN && v <= FREQUENCY_MAX; }},
            /* Checking for Can logging */
            ValidationRuleInt32{"CL", &SequenceCommands::value_waveform_frequency, [](std::int32_t v)
                                { return v >= OUTPUT_DISABLED && v <= OUTPUT_ENABLED; }},
            /* Checking value of position amplitude */
            ValidationRuleInt32{"R", &SequenceCommands::value_position_amplitude, [](std::int32_t v)
                                {
                                    float amplitude = static_cast<float>(v) / 1000.0f;
                                    float offset = DDS::get_offset_position();
                                    return (amplitude + offset) <= POSITION_AMPLITUDE_OFFSET_MAX &&
                                           (offset - amplitude) >= POSITION_AMPLITUDE_OFFSET_MIN;
                                }},
            /* Checking value of position offset */
            ValidationRuleInt32{"V", &SequenceCommands::value_position_offset, [](std::int32_t v)
                                {
                                    float offset = static_cast<float>(v) / 1000.0f;
                                    float amplitude = DDS::get_amplitude_position();
                                    return (offset + amplitude) <= POSITION_AMPLITUDE_OFFSET_MAX &&
                                           (offset - amplitude) >= POSITION_AMPLITUDE_OFFSET_MIN;
                                }},
            ValidationRuleInt32{"SP", &SequenceCommands::sequence_duration, [](std::int32_t v)
                                { return v >= SEQ_PERIOD_MIN; }},
            ValidationRuleInt32{"SH", &SequenceCommands::sequence_duration, [](std::int32_t v)
                                { return v >= SEQ_PERIOD_MIN; }},
            ValidationRuleInt32{"SW", &SequenceCommands::sequence_duration, [](std::int32_t v)
                                { return v >= SEQ_MS_MIN; }}};

        for (const auto &rule : validation_rules_int32)
        {
            if (cmd == rule.fieldName)
            {
                if (!rule.validationFunc(value))
                {
                    log().error("*E Invalid value for {}: {}", cmd, value);
                    return false;
                }
                return true;
            }
        }
        return true; // If command is not found, assume valid
    }

    void CommandHandler::command_exec(const std::string_view &cmd, const std::int32_t &value)
    {
        static const std::array
            command_entry = {CommandEntry{"I", set_current_amplitude_cmd},
                              CommandEntry{"H", init_harm_cmd},
                              CommandEntry{"T", init_triangle_cmd},
                              CommandEntry{"S", init_square_cmd},
                              CommandEntry{"O", set_ac_offset_cmd},
                              CommandEntry{"C", set_ac_duty_cycle_cmd},
                              CommandEntry{"RT", set_ac_ramp_time},
                              CommandEntry{"F", set_phase_shift_cmd},
                              CommandEntry{"D", enable_dc_cmd},
                              CommandEntry{"A", enable_ac_cmd},
                              CommandEntry{"P", enable_position_cmd},
                              CommandEntry{"R", set_position_amplitude_cmd},
                              CommandEntry{"V", set_position_offset_cmd},
                              CommandEntry{"PC", position_calibration_cmd},
                              CommandEntry{"ID", set_new_id_from_command},
                              CommandEntry{"CL", can_logging},
                              CommandEntry{"HB", process_hb_message},
                              CommandEntry{"SP", new_sequence_detected_command},
                              CommandEntry{"SH", new_sequence_detected_command},
                              CommandEntry{"SW", new_sequence_detected_command},
                              CommandEntry{"SC", clear_all_sequences_cmd},
                              CommandEntry{"SF", stop_filling_current_sequence},
                              CommandEntry{"ST", start_stored_sequences_cmd}};

        for (const auto &entry : command_entry)
        {
            if (cmd == entry.command)
            {
                entry.func(cmd, value);
                return;
            }
        }
        log().error("*E COM: Command '{}' not supported", cmd);
    }

    void CommandHandler::new_sequence_detected_command(const std::string_view &cmd, const std::int32_t &value)
    {
        filling_sequence = true;

        if (cmd == "SP")
        {
            cmd_sequence.sequence_command = SequenceCommandType::SEQ_WHOLE_PERIOD;
        }
        else if (cmd == "SH")
        {
            cmd_sequence.sequence_command = SequenceCommandType::SEQ_HALF_PERIOD;
        }
        else if (cmd == "SW")
        {
            cmd_sequence.sequence_command = SequenceCommandType::SEQ_TIME_MS;
        }
        else
        {
            log().error("*E Invalid sequence command: {}", cmd);
            filling_sequence = false; // Prevent further sequence filling if command is invalid
            return;
        }
        cmd_sequence.sequence_duration = value;
        log().debug("Starting to fill sequence: {} with duration {}", cmd, value);
    }

    void CommandHandler::fill_sequence_structure(const std::string_view &cmd, const std::int32_t &value)
    {
        constexpr WaveformMapping waveform_mappings[] = {
            {"D", WaveformType::DC},
            {"H", WaveformType::HARMONIC},
            {"T", WaveformType::TRIANGLE},
            {"S", WaveformType::SQUARE}};

        constexpr std::array<OtherCommandMapping, 7> other_command_mappings = {
            {{"I", &SequenceCommands::current, &SequenceCommands::value_current},
             {"O", &SequenceCommands::ac_offset, &SequenceCommands::value_ac_offset},
             {"C", &SequenceCommands::square_duty, &SequenceCommands::value_square_duty},
             {"RT", &SequenceCommands::square_ramp, &SequenceCommands::value_square_ramp},
             {"F", &SequenceCommands::phase_shift, &SequenceCommands::value_phaseshift},
             {"R", &SequenceCommands::position_amplitude, &SequenceCommands::value_position_amplitude},
             {"V", &SequenceCommands::position_offset, &SequenceCommands::value_position_offset}}};

        constexpr OutputMapping output_mappings[] = {
            {"D", OutputType::DC_OUTPUT},
            {"A", OutputType::AC_OUTPUT},
            {"P", OutputType::POSITION_OUTPUT}};

        for (const auto &entry : waveform_mappings)
        {
            if (cmd == entry.command)
            {
                cmd_sequence.waveform = entry.waveform;
                cmd_sequence.value_waveform_frequency = value;
                return;
            }
        }

        for (const auto &[command_str, bool_field, value_field] : other_command_mappings)
        {
            if (cmd == command_str)
            {
                cmd_sequence.*bool_field = true;   // Set boolean flag to true
                cmd_sequence.*value_field = value; // Store the value
                return;
            }
        }
        for (const auto &entry : output_mappings)
        {
            if (cmd == entry.command)
            {
                cmd_sequence.output_select = entry.output;
                cmd_sequence.value_output = value;
                add_to_sequence(); // Adding to sequence if output selection detected
                return;
            }
        }

        log().error("{} is not a supported sequence command", cmd);
    }

    void CommandHandler::set_current_amplitude_cmd(const std::string_view &, const std::int32_t &value)
    {
        Current_source::set_current_amplitude(static_cast<float>(value) / 1000.0f);
    }

    void CommandHandler::init_harm_cmd(const std::string_view &, const std::int32_t &value)
    {
        DDS::init_harm(static_cast<uint16_t>(value));
        log().debug("Harmonic frequency set to {}", value);
    }

    void CommandHandler::init_triangle_cmd(const std::string_view &, const std::int32_t &value)
    {
        DDS::init_triangel(static_cast<uint16_t>(value));
        log().debug("Triangle frequency set to {}", value);
    }

    void CommandHandler::init_square_cmd(const std::string_view &, const std::int32_t &value)
    {
        DDS::init_square(static_cast<uint16_t>(value));
        log().debug("Square frequency set to {}", value);
    }

    void CommandHandler::set_ac_offset_cmd(const std::string_view &, const std::int32_t &value)
    {
        DDS::set_ac_offset(static_cast<float>(value) / 1000.0f);
        log().debug("DC offset set to {} mA", value);
    }

    void CommandHandler::set_ac_duty_cycle_cmd(const std::string_view &, const std::int32_t &value)
    {
        DDS::set_ac_duty_cycle(static_cast<uint8_t>(value));
        DDS::set_new_duty_cycle_set(true);
        DDS::init_square(DDS::get_frequency());
        log().debug("Duty cycle set to {}", value);
    }

    void CommandHandler::set_ac_ramp_time(const std::string_view &, const std::int32_t &value)
    {
        DDS::set_ac_ramp_time_percentage(static_cast<uint8_t>(value));
        DDS::set_new_ramp_time_percentage(true);
        DDS::init_square(DDS::get_frequency());
        log().debug("Ramp time set to {}%", value);
    }

    void CommandHandler::set_phase_shift_cmd(const std::string_view &, const std::int32_t &value)
    {
        DDS::set_phase_shift(static_cast<float>(value));
        log().debug("Phase shift set to {}", value);
    }

    void CommandHandler::set_position_amplitude_cmd(const std::string_view &, const std::int32_t &value)
    {
        float amplitude = static_cast<float>(value) / 1000.0f;
        DDS::set_position_amplitude(amplitude);
        log().debug("Amplitude set to {} mm", amplitude);
    }

    void CommandHandler::set_position_offset_cmd(const std::string_view &, const std::int32_t &value)
    {
        float offset = static_cast<float>(value) / 1000.0f;
        DDS::set_position_offset(offset);
        log().debug("Offset set to {} mm", offset);
    }

    void CommandHandler::enable_dc_cmd(const std::string_view &, const std::int32_t &value)
    {
        if (value == 0)
        {
            Current_source::disable();
            return;
        }
        Position_control::set_position_control(false);
        Current_source::set_ac_output(false);
        Current_source::set_current(Current_source::get_current_amplitude());
        Current_source::enable();
    }

    void CommandHandler::enable_ac_cmd(const std::string_view &, const std::int32_t &value)
    {
        if (value == 0)
        {
            Current_source::disable();
            return;
        }
        if (!aron::DDS::is_ac_lut_set())
        {
            log().error("*E Can't set AC output without specified waveform and frequency");
            return;
        }

        Position_control::set_position_control(false);
        Current_source::set_ac_output(true);
        Current_source::enable();
    }

    void CommandHandler::enable_position_cmd(const std::string_view &, const std::int32_t &value)
    {
        if (value == 0)
        {
            Current_source::disable();
            DDS::reset_phase_accumulator();
            return;
        }
        Current_source::set_ac_output(false);
        Position_control::set_position_control(true);
        Current_source::enable();
    }
    void CommandHandler::can_logging(const std::string_view &, const std::int32_t &value)
    {
        Position_control::set_position_logging(value);
    }
    void CommandHandler::set_new_id_from_command(const std::string_view &, const std::int32_t &value)
    {
        BusActivity::set_new_id_from_command(value);
    }

    void CommandHandler::process_hb_message(const std::string_view &, const std::int32_t &value)
    {
        BusActivity::process_heartbeat(value);
    }

    void CommandHandler::start_stored_sequences_cmd(const std::string_view &, const std::int32_t &)
    {
        CommandSequencer::start_sequence();
    }

    void CommandHandler::clear_all_sequences_cmd(const std::string_view &, const std::int32_t &)
    {
        CommandSequencer::clear_sequencer();
        Current_source::disable();
    }

    void CommandHandler::stop_filling_current_sequence(const std::string_view &, const std::int32_t &)
    {
        cmd_sequence = SequenceCommands{};
    }
    void CommandHandler::position_calibration_cmd(const std::string_view &, const std::int32_t &)
    {
        log().debug("Position calibration started");
        Position_control::position_calibration();
    }

    void CommandHandler::add_to_sequence()
    {
        if (CommandSequencer::add_sequence_command(cmd_sequence))
        {
            log().debug("Sequence successfully parsed into Sequence handler");
            filling_sequence = false;
            cmd_sequence = SequenceCommands{}; // Reset structure to default
        }
    }
}
