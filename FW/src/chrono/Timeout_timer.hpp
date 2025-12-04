#pragma once

#include <cstdint>

#include "chrono.hpp"



namespace nd::chrono
{
    class Timeout_timer
    {
        public:
            Timeout_timer() = default;
            explicit Timeout_timer(const steady_clock::duration timeout_p)
                : timeout{timeout_p}
            {}

            void start()
            {
                start_time = steady_clock::now();
                active = true;
            }

            [[nodiscard]] auto elapsed() const
            {
                const auto current_time = steady_clock::now();
                const auto elapsed_time = current_time - start_time;
                return elapsed_time;
            }

            [[nodiscard]] bool is_time()
            {
                if (!active)
                    return false;

                if (elapsed() >= timeout)
                {
                    reset();
                    return true;
                }

                return false;
            }

            void reset()
            {
                active = false;
            }

            void set_timeout(const steady_clock::duration timeout_p)
            {
                timeout = timeout_p;
            }

            [[nodiscard]] auto get_timeout() const
            {
                return timeout;
            }

            [[nodiscard]] bool is_active() const
            {
                return active;
            }

            [[nodiscard]] bool is_timeout_set() const
            {
                using namespace std::chrono_literals;
                return !(timeout == 0s);
            }

        private:
            steady_clock::time_point start_time{};
            steady_clock::duration timeout{0};
            bool active{false};
    };
}
