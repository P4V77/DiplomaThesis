#include <sam.h>
#include <cctype>
#include <charconv>

#include "utils/logging/Logger.hpp"
#include "chrono/Wait.hpp"
#include "chrono/Periodic_timer.hpp"
#include "utils/pid/pid.hpp"

namespace regulator
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    // Constructor definition
    PID::PID(
        float Kp_i,
        float Ki_i,
        float Kd_i,
        float out_limit_min_i,
        float out_limit_max_i,
        float sampling_freq,
        float f_c_i)
        : Kp(Kp_i),
          Ki(Ki_i),
          Kd(Kd_i),
          out_limit_min(out_limit_min_i),
          out_limit_max(out_limit_max_i),
          f_c(f_c_i),
          T_sampling(1.0f / sampling_freq),
          beta(2 * static_cast<float>(M_PI) * f_c * T_sampling / (2 * static_cast<float>(M_PI) * f_c * T_sampling + 1.0f)),
          integrator(0.0f),
          derivative(0.0f),
          derivative_filtered(0.0f),
          error_prev(0.0f),
          error_sum(0.0f)
    {
    }

    float PID::out_pid(const float *error)
    {

        // Proportional term
        const float proportional = *error * Kp;

        // Integral term (Anti-windup applied)
        error_sum += *error; // Accumulate error over time
        float new_integrator = integrator + 0.5f * Ki * T_sampling * error_sum;

        // Clamp integrator before applying
        const float integrator_limit = out_limit_max * 0.1f;
        integrator = std::clamp(new_integrator, -integrator_limit, integrator_limit);

        // Derivative term
        derivative = Kd * (*error - error_prev) / T_sampling;
        derivative_filtered = (1.0f - beta) * derivative_filtered + beta * derivative;

        // Save current error
        error_prev = *error;

        // Compute output and apply output limits
        float out = proportional + integrator + derivative_filtered;
        out = std::clamp(out, out_limit_min, out_limit_max);

        return out;
    }

}