#pragma once

#include <cstdint>
#include <string_view>
#include <array>
#include <functional>
#include <stdbool.h> // For bool

namespace aron
{
    // Enum for sequence commands
    enum class SequenceCommandType
    {
        SEQ_WHOLE_PERIOD, // Whole period counter
        SEQ_HALF_PERIOD,  // Half period counter
        SEQ_TIME_MS       // Time counter
    };

    enum class WaveformType
    {
        HARMONIC,
        TRIANGLE,
        SQUARE,
        DC
    };

    enum class OutputType
    {
        DC_OUTPUT,
        AC_OUTPUT,
        POSITION_OUTPUT
    };

    // Structure for sequence commands
    struct SequenceCommands
    {
        SequenceCommandType sequence_command{};
        int32_t sequence_duration = 0;

        // BOOL
        bool current{false}; // Changed char to bool
        int32_t value_current = 0;

        bool ac_offset{false}; // Changed char to bool
        int32_t value_ac_offset = 0;

        bool square_duty{false}; // Changed char to bool
        int32_t value_square_duty = 0;

        bool square_ramp{false}; // Changed char to bool
        int32_t value_square_ramp = 0;

        bool phase_shift{false}; // Changed char to bool
        int32_t value_phaseshift = 0;

        bool position_amplitude{false}; // Changed char to bool
        int32_t value_position_amplitude = 0;

        bool position_offset{false}; // Changed char to bool
        int32_t value_position_offset = 0;

        WaveformType waveform{};
        int32_t value_waveform_frequency = 0;

        OutputType output_select{};
        int32_t value_output = 0;
    };

    // For command execution mapping
    struct CommandEntry
    {
        std::string_view command;
        std::function<void(const std::string_view &, const std::int32_t &)> func;
    };

    struct CommandEntryNoVal
    {
        std::string_view command;
        void (*func)();
    };

    // For parsing into sequencer
    struct WaveformMapping
    {
        std::string_view command;
        WaveformType waveform;
    };
    // For parsing into sequencer

    struct OutputMapping
    {
        std::string_view command;
        OutputType output;
    };
    // For parsing other commands into sequencer
    struct OtherCommandMapping
    {
        std::string_view command;
        bool SequenceCommands::*bool_field;
        int32_t SequenceCommands::*value_field;
    };

    // For checking integer values in SequenceCommands
    struct ValidationRuleInt32
    {
        std::string_view fieldName;
        std::int32_t SequenceCommands::*valuePtr;
        std::function<bool(std::int32_t)> validationFunc;
    };

} // namespace aron
