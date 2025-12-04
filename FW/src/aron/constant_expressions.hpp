#pragma once
namespace aron
{
    namespace CommandConstants
    {
        // Validation Constants
        static constexpr int32_t OUTPUT_DISABLED = 0;                // -
        static constexpr int32_t OUTPUT_ENABLED = 1;                 // -
        static constexpr int32_t DUTY_CYCLE_MIN = 0;                 // %
        static constexpr int32_t DUTY_CYCLE_MAX = 100;               // %
        static constexpr int32_t RAMP_MIN = 0;                       // %
        static constexpr int32_t RAMP_MAX = 50;                      // %
        static constexpr int32_t PHASE_MIN = 0;                      // Degrees
        static constexpr int32_t PHASE_MAX = 360;                    // Degrees
        static constexpr int32_t OFFSET_MIN = -8000;                 // mA
        static constexpr int32_t OFFSET_MAX = 8000;                  // mA
        static constexpr int32_t CURRENT_MIN = -10000;               // mA
        static constexpr int32_t CURRENT_MAX = 10000;                // mA
        static constexpr int32_t FREQUENCY_MIN = 1;                  // Hz
        static constexpr int32_t FREQUENCY_MAX = 3000;               // Hz
        static constexpr int32_t SEQ_PERIOD_MIN = 1;                 // Mininmal number of periods
        static constexpr int32_t SEQ_MS_MIN = 1;                     // Mininmal number of periods
        static constexpr float POSITION_AMPLITUDE_OFFSET_MAX = 8.0f; // mm
        static constexpr float POSITION_AMPLITUDE_OFFSET_MIN = 1.0f; // mm

        // Declare constexpr command strings
        static constexpr std::string_view CURRENT_COMMAND = "I";
        static constexpr std::string_view AC_OFFSET_COMMAND = "O";
        static constexpr std::string_view SQUARE_DUTY_COMMAND = "C";
        static constexpr std::string_view SQUARE_RAMP_COMMAND = "RT";
        static constexpr std::string_view PHASE_SHIFT_COMMAND = "F";
        static constexpr std::string_view POSITION_AMPLITUDE_COMMAND = "R";
        static constexpr std::string_view POSITION_OFFSET_COMMAND = "V";
        static constexpr std::string_view WAVEFORM_HARMONIC_COMMAND = "H"; // or WAVEFORM_COMMAND
        static constexpr std::string_view WAVEFORM_TRIANGLE_COMMAND = "T"; // or WAVEFORM_COMMAND
        static constexpr std::string_view WAVEFORM_SQUARE_COMMAND = "S";   // or WAVEFORM_COMMAND
        static constexpr std::string_view OUTPUT_DC_COMMAND = "D";         // or OUTPUT_SELECT_COMMAND
        static constexpr std::string_view OUTPUT_AC_COMMAND = "A";         // or OUTPUT_SELECT_COMMAND
        static constexpr std::string_view OUTPUT_PULSE_COMMAND = "P";      // or OUTPUT_SELECT_COMMAND
    }
    namespace UniqueMessageIDs
    {
        // Unique message IDs
        static constexpr uint8_t Broadcast{255};
        static constexpr uint8_t HeartBeat{254};
        static constexpr uint8_t PendingIDConfirmation{253};
        static constexpr uint8_t GeneralInfo{252};
        static constexpr uint8_t GeneralProblem{251};
        static constexpr uint8_t valid_msg_ids[] = {HeartBeat, PendingIDConfirmation, GeneralInfo, GeneralProblem};
        static constexpr size_t valid_msg_ids_size = sizeof(valid_msg_ids) / sizeof(valid_msg_ids[0]);
    };
}