#pragma once

#include "chrono/Periodic_timer.hpp"
#include "chrono/Timeout_timer.hpp"
#include "aron/interface/interface.hpp"
#include "aron/output/current_source.hpp"
#include "aron/output/dds.hpp"
#include "aron/output/position_control.hpp"
#include "aron/communication/communication.hpp"
#include "aron/communication/bus_activity.hpp"

using namespace nd;
namespace aron
{

    class TimerManager
    {
    public:
        static void run();

    private:
        static chrono::Periodic_timer status_led_timer;
        static chrono::Periodic_timer status_temp_timer;
        static chrono::Periodic_timer source_timeout;
        static chrono::Periodic_timer can_heart_beat;
        static chrono::Periodic_timer can_info;
        static chrono::Timeout_timer button_hold_timer;

        static void handleStatusLedTimer();
        static void handleStatusTempTimer();
        static void handleSourceTimeout();
        static void handleCanHeartBeat();
        static void handleCanInfo();
        static void handleButtonPress();
    };
} // namespace aron