#pragma once

#include <cstdint>

#include "chrono/chrono.hpp"



namespace nd::chrono
{
    class Periodic_timer
    {
        public:
            explicit Periodic_timer(const steady_clock::duration period_p)
                : period{period_p}
            {
                enable();
            }

            Periodic_timer(const steady_clock::duration period_p, const bool enabled_p)
                : period{period_p}, enabled{enabled_p}
            {
                reset();
            }

            [[nodiscard]] bool is_time()
            {
                if (!enabled)
                    return false;

                const auto current_time = steady_clock::now();
                const auto elapsed_time = current_time - start_time;

                if (elapsed_time >= period)
                {
                    reset();
                    return true;
                }

                return false;
            }

            void enable()
            {
                enabled = true;
                reset();
            }

            void enable_now()
            {
                enabled = true;
                start_time = steady_clock::now() - period;
            }

            void disable()
            {
                enabled = false;
            }

            void reset()
            {
                start_time = steady_clock::now();
            }

            void set_period(const steady_clock::duration period_p)
            {
                period = period_p;
            }

            [[nodiscard]] auto get_period() const
            {
                return period;
            }

            [[nodiscard]] bool is_enabled() const
            {
                return enabled;
            }

        private:
            steady_clock::time_point start_time{};
            steady_clock::duration period{0};
            bool enabled{false};
    };
}
