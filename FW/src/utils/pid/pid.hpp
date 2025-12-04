#pragma once

#include <cstdint>
#include <array>
#include <string_view>

#include "aron/output/current_source.hpp"

namespace regulator
{
    class PID
    {
    public:
        PID(float Kp, float Ki, float Kd, float out_limit_min, float out_limit_max, float sampling_freq, float f_c);
        float out_pid(const float *error);

    private:
        float Kp;
        float Ki;
        float Kd;
        float out_limit_min;
        float out_limit_max;
        float f_c;
        float T_sampling;
        float beta;
        float integrator;
        float derivative;
        float derivative_filtered;
        float error_prev;
        float error_sum;
    };
}
