#include "Wait.hpp"



namespace nd::chrono
{
    void wait(const steady_clock::duration duration_p)
    {
        Timeout_timer timer{duration_p};
        timer.start();

        while (!timer.is_time());
    };
}
