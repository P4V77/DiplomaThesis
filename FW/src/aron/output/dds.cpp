#include "dds.hpp"

namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void DDS::tc0_handler()
    {
        if (phase_accumulator > static_cast<float>((ac_lut.size() / 2) - 1))
        {
            executed_half_period = true; // Flag signaling half period completition
        }
        if (phase_accumulator > (ac_lut.size() - 1))
        {

            phase_accumulator = std::fmod(phase_accumulator, ac_lut.size()); // Resets phase accum to 0
            executed_period = true;                                          // Flag signalling full period completition
        }

        phase_accumulator += phase_increment; // Incrementing phase acumulator

        // Calculating next DDS value
        if (aron::Current_source::get_ac_status())
        {
            aron::DDS::dds_handler_current();
        }
        else if (aron::Position_control::get_position_control_status())
        {
            aron::DDS::dds_handler_position();
        }
        // log().debug("Current req {}", current_request);
    }

    // Initializes AC lut as LUT with SINE waveform
    void DDS::init_harm(const float frequency)
    {
        if (!tc0_set)
        {
            init_tc0_couter();
        }

        phase_increment = 4.0f * static_cast<float>(frequency) * (static_cast<float>(ac_lut.size()) / dds_update_freq);
        log().debug("phase_increment): {}", phase_increment);
        // If LUTs not defined for sinus waveform
        if (waveform != 'H')
        {
            // First and last value must be zero
            for (uint16_t i = 0; i < static_cast<uint16_t>(ac_lut.size()); i++)
            {
                ac_lut[i] = sinf((i * 2 * static_cast<float>(M_PI)) / static_cast<float>(ac_lut.size()));
            }
            waveform = 'H';
        }
        output_freq = frequency;
    }

    // Initializes AC lut as LUT with TRIANGLE waveform
    void DDS::init_triangel(const float frequency)
    {
        if (!tc0_set)
        {
            init_tc0_couter();
        }

        phase_increment = 4.0f * static_cast<float>(frequency) * (static_cast<float>(ac_lut.size()) / dds_update_freq);

        // If LUTs not defined for sinus waveform

        if (waveform != 'T')
        {
            // First and last value must be zero
            ac_lut[0] = 0.0f;
            float increment{4.0f / ac_lut.size()}; // Increment to reach 1.0 from -1.0
            size_t quarter_size = ac_lut.size() / 4;
            // Rising from 0 to 1
            for (size_t i = 1; i < quarter_size; ++i)
            {
                ac_lut[i] = ac_lut[i - 1] + increment;
            }
            // Falling from 1 to -1
            for (size_t i = quarter_size; i < (3 * quarter_size); ++i)
            {
                ac_lut[i] = ac_lut[i - 1] - increment;
            }
            // Rissing from -1 to 0
            for (size_t i = (3 * quarter_size); i < (ac_lut.size()); ++i)
            {
                ac_lut[i] = ac_lut[i - 1] + increment;
            }
            waveform = 'T';
        }
        output_freq = frequency;
    }

    void DDS::init_square(const float frequency)
    {
        if (!tc0_set)
        {
            init_tc0_couter();
        }
        if (new_duty_cycle == false)
        {
            phase_increment = 4.0f * static_cast<float>(frequency) * (static_cast<float>(ac_lut.size()) / dds_update_freq);
        }

        if ((waveform != 'S') || new_duty_cycle || new_ramp_req)
        {
            uint16_t period = ac_lut.size();
            uint16_t high_period = static_cast<uint16_t>(((float)period * (float)duty_cycle) / 100.0f);
            uint16_t rise_samples = static_cast<uint16_t>(((float)rise_time_percentage / 100.0f) * (float)period);

            for (uint16_t i = 0; i < period; ++i)
            {
                if (i <= high_period) // High period
                {
                    if (i < rise_samples)
                    {
                        // Rising edge
                        float increment = 2.0f / static_cast<float>(rise_samples);
                        ac_lut[i] = -1.0f + increment * static_cast<float>(i);
                    }
                    else
                    {
                        // Flat after rising edge
                        ac_lut[i] = 1.0f;
                    }
                }
                else if ((i > high_period) && (i <= (high_period + rise_samples)))
                {
                    // Falling Edge
                    float increment = 2.0f / static_cast<float>(rise_samples);
                    ac_lut[i] = 1.0f - increment * static_cast<float>(i - high_period);
                }
                else // Low period
                {
                    // Flat after falling edge
                    ac_lut[i] = -1.0f;
                }
            }

            waveform = 'S';
            new_duty_cycle = false;
        }
        output_freq = frequency;
    }

    void DDS::dds_handler_current()
    {
        // // Generate output value using ac_lut from init
        // // In ac_lut can be various waveforms
        size_t ac_lut_index = std::clamp(
            static_cast<size_t>(std::round(phase_accumulator)), // Proper rounding
            static_cast<size_t>(0),
            ac_lut.size() - 1);
        dds_output_value = (ac_lut[ac_lut_index] * aron::Current_source::get_current_amplitude()) + ac_offset;
        aron::Current_source::set_current(dds_output_value);
        // log().debug("Output current {}", dds_output_value);
    }

    void DDS::dds_handler_position()
    {
        // Generate output value using ac_lut from init
        // In ac_lut can be various waveforms
        // Using LUT for generating position
        size_t ac_lut_index = std::clamp(
            static_cast<size_t>(std::round(phase_accumulator)), // Proper rounding
            static_cast<size_t>(0),
            ac_lut.size() - 1);
        dds_output_value = ac_lut[ac_lut_index] * amplitude_position + offset_position;
        aron::Position_control::set_position(dds_output_value);

        // log().debug("position req (mm): {}", dds_output_value);
        // log().debug("position req {} mm     act {} mm", aron::Current_source::position_request, aron::Current_source::position_actual);
    }

    bool DDS::is_ac_lut_set()
    {
        bool ac_lut_set = false;
        if (waveform != 0)
        {
            ac_lut_set = true;
        }
        return ac_lut_set;
    }
    char DDS::get_ac_waveform()
    {
        return waveform;
    }
    float DDS::get_amplitude_position()
    {
        return amplitude_position;
    }
    float DDS::get_offset_position()
    {
        return offset_position;
    }
    float DDS::get_frequency()
    {
        return output_freq;
    }
    bool DDS::get_period_completed()
    {
        return executed_period;
    }
    bool DDS::get_half_period_completed()
    {
        return executed_half_period;
    }
    void DDS::set_ac_offset(float ac_offset_req)
    {
        ac_offset = ac_offset_req;
    }
    void DDS::set_position_offset(float offset_position_req)
    {
        offset_position = offset_position_req;
    }
    void DDS::set_position_amplitude(float amplitude_position_req)
    {
        amplitude_position = amplitude_position_req;
    }
    void DDS::set_phase_shift(float phase_shift)
    {
        // Ensure phase_shift is in the range [0, 360)
        phase_shift = std::fmod(phase_shift + 360.0f, 360.0f); // Wraps between 0-360
        phase_accumulator = (phase_shift / 360.0f) * ac_lut.size();
    }
    void DDS::set_ac_waveform(char ac_waveform_req)
    {
        waveform = ac_waveform_req;
    }
    void DDS::set_ac_duty_cycle(uint8_t ac_duty_cycle_req)
    {
        duty_cycle = ac_duty_cycle_req;
    }
    void DDS::set_ac_ramp_time_percentage(uint8_t ramp_time_percentage_i)
    {
        rise_time_percentage = ramp_time_percentage_i;
    }
    void DDS::set_new_duty_cycle_set(bool new_duty_cycle_req)
    {
        new_duty_cycle = new_duty_cycle_req;
    }
    void DDS::set_new_ramp_time_percentage(bool new_ramp_req_i)
    {
        new_ramp_req = new_ramp_req_i;
    }
    void DDS::set_executed_period(bool next_value_set)
    {
        executed_period.store(next_value_set);
    }
    void DDS::set_executed_half_period(bool next_value_set)
    {
        executed_half_period.store(next_value_set);
    }
    void DDS::reset_phase_accumulator()
    {
        phase_accumulator = 0.0f;
    }
    void DDS::init_tc0_couter()
    {
        // Initializing counter for periodic updating of phase accum
        // 12 kHz
        // TC0 for DDS and Command sequencer
        // Start synced to its overflow
        nd::drivers::same5x::Mclk_ctrl::enable(TC0);
        nd::drivers::same5x::Gclk_pchctrl::enable(TC0_GCLK_ID, 3); // 120 MHz
        TC0->COUNT16.CC[0].reg = 10000;                            // 12 kHZ
        // TC0->COUNT16.CC[0].reg = 6000; // 20 kHZ
        while (TC0->COUNT16.SYNCBUSY.bit.COUNT)
            ;
        TC0->COUNT16.INTENSET.bit.MC0 = 1;
        TC0->COUNT16.CTRLA.reg = TC_CTRLA_PRESCALER_DIV1;
        aron::DDS::dds_update_freq = static_cast<float>(SystemCoreClock) / static_cast<float>(TC0->COUNT16.CC[0].reg);

        TC0->COUNT16.CTRLA.bit.ENABLE = 1;
        while (TC0->COUNT16.SYNCBUSY.bit.ENABLE)
            ;

        tc0_set = true;
    }
}