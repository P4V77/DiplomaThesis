#pragma once

#include <atomic>
#include <cstdint>
#include <cmath>

#include "drivers/sam5x/mclk.hpp"
#include "drivers/sam5x/gclk.hpp"
#include "drivers/systick/systick.hpp"
#include "drivers/gpio/gpio_same5x.hpp"
#include "utils/adc_dac_utils.hpp"
#include "utils/logging/Logger.hpp"
#include "utils/pid/pid.hpp"
#include "chrono/Wait.hpp"

#include "aron/output/dds.hpp"
#include "aron/output/position_control.hpp"
#include "aron/temp/temp_meassure.hpp"
#include "aron/h_interp.hpp"


namespace aron
{
    class Current_source
    {
    public:
        static bool is_enabled();
        static void init();
        static void init_PID();
        static void adc_irq_handler();
        static void bridge_driving(const float *error_current, float *duty);
        static void handler();
        static void disable();
        static void enable();
        static void enable_force();
        static void disable_force();

        static float get_current_actual();
        static float get_current_request();
        static float get_current_amplitude();
        static float get_adc_vref();
        static bool get_ac_status();
        static uint32_t get_switching_frequency();

        static void set_current(const float current);
        static void set_current_amplitude(const float current);
        static void set_dc_output(bool dc_output_request);
        static void set_ac_output(bool ac_output_request);

    private:
        static_assert(std::atomic<float>::is_always_lock_free, "std::atomic<float> is not lock free");

        static inline std::atomic<bool> enabled{false};
        static inline std::atomic<bool> disable_request{false};
        static inline std::atomic<bool> in_interupt{false};
        static inline std::atomic<bool> ac_output{false};

        static inline std::atomic<float> current_actual{0.0f};
        static inline std::atomic<float> current_amplitude{0.0f};
        static inline std::atomic<float> current_request{0.0f};

        static inline uint32_t switching_frequency{};
        /*PID Constants*/
        static constexpr float Kp{750.0f};
        static constexpr float Ki{3000.0f};
        static constexpr float Kd{0.002f};
        
        static constexpr float out_limit_min{25.0f};
        static constexpr float out_limit_max{1975.0f};
        static constexpr float f_c = 100.0f;
        static constexpr float error_th_min = 0.3f;
        static constexpr float error_th_max = 1.0f;
        /* Current senseB sensitivity */
        static constexpr float U_to_I = 0.12f;
        static inline uint16_t duty_B{0};
        static inline uint16_t duty_A{0};
        static inline float voltage_adc{0.0f};
        static inline float dc_offset{1.25f};
        static inline float adc_vref{2.5f};
        static inline float act_current_filtered{0.0f};
        static inline float duty_filtered{0.0f};

        static void get_new_current();
        static void regulator();
        static void zero_current_offset_calibration();
    };
}
