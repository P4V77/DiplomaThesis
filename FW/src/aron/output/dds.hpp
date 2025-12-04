#pragma once

#include <cstdint>
#include <array>
#include <string_view>
#include <cctype>
#include <charconv>
#include <sam.h>

#include "utils/logging/Logger.hpp"
#include "chrono/Periodic_timer.hpp"
#include "chrono/Wait.hpp"

#include "current_source.hpp"

namespace aron
{
    class DDS
    {
    public:
        static void init_counter();
        static void tc0_handler();
        static void init_harm(const float frequency);
        static void init_triangel(const float frequency);
        static void init_square(const float frequency);
        static void set_current_ampl(const float current);
        static void dds_handler_current();
        static void dds_handler_position();
        static bool is_ac_lut_set();

        static bool get_new_duty_cycle_set();
        static char get_ac_waveform();
        static float get_amplitude_position();
        static float get_offset_position();
        static float get_frequency();
        static bool get_period_completed();
        static bool get_half_period_completed();
        static bool get_period_ended();

        static void set_ac_offset(float ac_offset_req);
        static void set_position_offset(float position_offset_req);
        static void set_position_amplitude(float position_amplitude_req);
        static void set_phase_shift(float phase_shift);
        static void set_ac_waveform(char ac_waveform_req);
        static void set_ac_duty_cycle(uint8_t ac_duty_cycle_req);
        static void set_ac_ramp_time_percentage(uint8_t ramp_time_percentage_i);
        static void set_new_duty_cycle_set(bool new_duty_cycle_req);
        static void set_new_ramp_time_percentage(bool new_ramp_req_i);
        static void set_executed_period(bool next_value_set);
        static void set_executed_half_period(bool next_value_set);
        static void reset_phase_accumulator();

    private:
        static inline float current_amplitude{0.0f};
        static inline float output_freq{0.0f};
        static inline float phase_increment{0.0f};
        static inline float dds_output_value{0.0f};
        static inline float ac_offset{0.0f};
        static inline char waveform{0};
        static inline bool new_duty_cycle{false};
        static inline bool new_ramp_req{false};
        static inline bool tc0_set{false};
        static inline uint8_t rise_time_percentage{0};
        static inline float dds_update_freq{};

        static inline std::array<float, 2400> ac_lut{};
        static inline std::atomic<float> phase_accumulator{0.0f};
        static inline std::atomic<float> offset_position{4.5f};
        static inline std::atomic<float> amplitude_position{3.5f};
        static inline std::atomic<uint8_t> duty_cycle{50};
        static inline std::atomic<bool> executed_period{false};
        static inline std::atomic<bool> executed_half_period{false};

        static void init_tc0_couter();
    };
}
