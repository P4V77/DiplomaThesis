#pragma once

#include <atomic>
#include <cstdint>
#include <cmath>

// #include "drivers/sam5x/mclk.hpp"
#include "drivers/sam5x/gclk.hpp"
#include "drivers/systick/systick.hpp"
#include "drivers/gpio/gpio_same5x.hpp"
#include "utils/adc_dac_utils.hpp"
#include "utils/logging/Logger.hpp"
#include "chrono/Wait.hpp"

#include "aron/interface/interface.hpp"
#include "aron/h_interp.hpp"

#include "current_source.hpp"
namespace aron
{
    using namespace position_LUT;
    class Position_control
    {
    public:
        static void init();
        static void position_calibration();
        static void adc1_position_handler();
        static void position_regulator();

        static float get_flux_density();
        static float get_voltage();
        static float get_actual_position();
        static bool get_position_control_status();
        static uint16_t get_LUT_index();

        static void set_position(const float position);
        static void set_position_control(bool request);
        static void set_position_logging(bool request);

        static void debug_logger();
        static void log_position_can();

    private:
        static constexpr float hall_sensitivity = 0.11f;

        static inline std::atomic<float> position_actual{0.0f};
        static inline std::atomic<float> position_request{0.0f};
        static inline std::atomic<float> adc1_result{0.0f};

        static inline std::atomic<bool> position_control{false};

        static constexpr float Kp{3.0f};
        static constexpr float Ki{10.0f};
        static constexpr float Kd{0.002f};

        static constexpr float Kt{0.3f};
        static constexpr float Imin = -6.0f;
        static constexpr float Imax = 6.0f;
        static constexpr float sampling_freq{static_cast<float>(15000000 / 12 / 256)};
        static constexpr float f_c = 30.0f;
        static inline bool position_log_enabled{false};
        static inline float LUT_offset{0.0f};

        static void evaluate();
        static void floatToCharArray(float value, char *buffer, size_t buffer_size);
    };
}
