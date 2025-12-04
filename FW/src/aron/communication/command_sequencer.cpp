#include "command_sequencer.hpp"
#include "command_handler.hpp" // Include CommandHandler for command_exec
#include <cstring>             // For std::strncpy
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

    bool CommandSequencer::add_sequence_command(const SequenceCommands &sequence)
    {
        if (sequence_command_count >= MAX_SEQUENCE_COMMANDS)
        {
            log().error("*E Sequence command limit reached");
            return false;
        }

        stored_sequences[sequence_command_count] = sequence;
        log().debug("Sequence command count {}", sequence_command_count);
        sequence_command_count++;
        return true;
    }

    void CommandSequencer::start_sequence()
    {
        log().debug("Calling start_sequence");

        if (sequence_command_count == 0)
        {
            log().error("*E No commands in sequence");
            return;
        }

        if (!tc3_initialized)
        {
            init_tc3_counter();
            tc3_initialized = true;
        }

        sequence_running = true;
        wait_start_time = nd::drivers::Systick::get();
        current_sequence_command = 0;

        call_next_command(); // Start the sequence correctly
    }

    void CommandSequencer::clear_sequencer()
    {
        sequence_command_count = 0;
        current_sequence_command = 0;
        sequence_running = false;
        wait_time_ms = 0;
        wait_start_time = 0;
        stored_sequences = {};
        tc3_initialized = false;

        // Disabeling TC3 Counter
        TC3->COUNT16.CTRLA.bit.ENABLE = 0;
        while (TC3->COUNT16.SYNCBUSY.bit.ENABLE)
            ;
    }

    void CommandSequencer::sequence_handler()
    {
        if (!sequence_running)
        {
            return;
        }
        auto &cmd = stored_sequences[current_sequence_command].sequence_command;

        switch (cmd)
        {
        case SequenceCommandType::SEQ_WHOLE_PERIOD:
            seq_whole_period_counter();
            break;
        case SequenceCommandType::SEQ_HALF_PERIOD:
            seq_half_period_counter();
            break;
        case SequenceCommandType::SEQ_TIME_MS:
            seq_time_counter();
            break;
        default:
            log().error("*E COM: Command not supported");
            break;
        }
    }

    void CommandSequencer::seq_whole_period_counter()
    {
        if (!aron::DDS::get_period_completed())
        {
            return;
        }
        duration_handler(1u);
        aron::DDS::set_executed_period(false); // Complete period acknowledge
    }

    void CommandSequencer::seq_half_period_counter()
    {
        if (aron::DDS::get_period_completed())
        {
            aron::DDS::set_executed_half_period(false); // Complete period acknowledge
            aron::DDS::set_executed_period(false);      // Complete period acknowledge
            aron::Current_source::enable_force();
            duration_handler(1u);
        }
        if (aron::DDS::get_half_period_completed())
        {
            aron::Current_source::disable_force();
        }
    }

    void CommandSequencer::seq_time_counter()
    {
        uint32_t current_time = nd::drivers::Systick::get();
        uint32_t elapsed_ms = current_time - wait_start_time;

        if (elapsed_ms >= 1)
        {
            duration_handler(elapsed_ms);
            wait_start_time = current_time;
        }
    }

    void CommandSequencer::duration_handler(const uint32_t &duration_substractor)
    {
        auto &current_command = stored_sequences[current_sequence_command];

        if (current_command.sequence_duration <= (int32_t)duration_substractor)
        {
            current_command.sequence_duration = 0;
        }
        else
        {
            current_command.sequence_duration -= (int32_t)duration_substractor;
        }
        if (!current_command.sequence_duration)
        {
            log().info("Sequence execution completed");
            current_sequence_command++;
            call_next_command();
        }
    }

    // In CommandSequencer::call_next_command:
    void CommandSequencer::call_next_command()
    {
        if (current_sequence_command >= sequence_command_count)
        {
            aron::Current_source::disable_force();
            sequence_running = false;
            log().info("Sequence completed");
            clear_sequencer();
            return;
        }
        auto &cmd = stored_sequences[current_sequence_command];
        execute_sequence_command(cmd);

        wait_start_time = nd::drivers::Systick::get();
    }

    void CommandSequencer::init_tc3_counter()
    {
        // Init TC3 for CommandSequencer
        nd::drivers::same5x::Mclk_ctrl::enable(TC3);
        nd::drivers::same5x::Gclk_pchctrl::enable(TC3_GCLK_ID, 3); // 120 MHz
        TC3->COUNT16.CC[0].reg = 48000;                            // 2.5 kHz
        TC3->COUNT16.INTENSET.bit.MC0 = 1;
        TC3->COUNT16.CTRLA.bit.ENABLE = 1;
        while (TC3->COUNT16.SYNCBUSY.bit.ENABLE)
            ;
    }

    void CommandSequencer::execute_sequence_command(const SequenceCommands &cmd)
    {
        aron::CommandHandler::command_exec(CURRENT_COMMAND, cmd.current ? cmd.value_current : 0);
        aron::CommandHandler::command_exec(AC_OFFSET_COMMAND, cmd.ac_offset ? cmd.value_ac_offset : 0);
        aron::CommandHandler::command_exec(SQUARE_DUTY_COMMAND, cmd.square_duty ? cmd.value_square_duty : 50);
        aron::CommandHandler::command_exec(SQUARE_RAMP_COMMAND, cmd.square_ramp ? cmd.value_square_ramp : 0);
        aron::CommandHandler::command_exec(PHASE_SHIFT_COMMAND, cmd.phase_shift ? cmd.value_phaseshift : 0);
        aron::CommandHandler::command_exec(POSITION_AMPLITUDE_COMMAND, cmd.position_amplitude ? cmd.value_position_amplitude : 0);
        aron::CommandHandler::command_exec(POSITION_OFFSET_COMMAND, cmd.position_offset ? cmd.value_position_offset : 3500);
    
        int32_t frequency = static_cast<int>(cmd.value_waveform_frequency);
    
        switch (cmd.waveform)
        {
        case WaveformType::HARMONIC:
            aron::CommandHandler::command_exec(WAVEFORM_HARMONIC_COMMAND, frequency);
            break;
        case WaveformType::TRIANGLE:
            aron::CommandHandler::command_exec(WAVEFORM_TRIANGLE_COMMAND, frequency);
            break;
        case WaveformType::SQUARE:
            aron::CommandHandler::command_exec(WAVEFORM_SQUARE_COMMAND, frequency);
            break;
        default:
            // FOR DC_OUTPUT NO WAVEFORM IS NEEDED
            break;
        }
    
        int32_t output_val = static_cast<int>(cmd.value_output);
        switch (cmd.output_select)
        {
        case OutputType::DC_OUTPUT:
            aron::CommandHandler::command_exec(OUTPUT_DC_COMMAND, output_val);
            break;
        case OutputType::AC_OUTPUT:
            aron::CommandHandler::command_exec(OUTPUT_AC_COMMAND, output_val);
            break;
        case OutputType::POSITION_OUTPUT:
            aron::CommandHandler::command_exec(OUTPUT_PULSE_COMMAND, output_val);
            break;
        default:
            break;
        }
    }
}