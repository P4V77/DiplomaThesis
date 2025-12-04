#pragma once

#include "chrono/chrono.hpp"



namespace nd::chrono
{
    class Stopwatch
    {
        public:
            Stopwatch()
            {
                start();
            }

            void start()
            {
                t1 = steady_clock::now();
            }

            void stop()
            {
                t2 = steady_clock::now();
                duration = t2 - t1;
            }

            std::chrono::milliseconds get()
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            }

        private:
            steady_clock::time_point t1{};
            steady_clock::time_point t2{};
            steady_clock::duration duration{};
    };
}
