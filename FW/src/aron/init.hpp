#pragma once

#include "utils/logging/LoggerUart.hpp"
#include "utils/logging/Logger.hpp"

#include "chrono/Wait.hpp"
#include "chrono/Periodic_timer.hpp"

#include "aron/interface/interface.hpp"
#include "aron/sw_timers/timer_manager.hpp"
#include "aron/system_settings.hpp"
#include "aron/temp/temp_meassure.hpp"
#include "aron/output/current_source.hpp"
#include "aron/output/dds.hpp"
#include "aron/output/position_control.hpp"
#include "aron/communication/communication.hpp"
#include "aron/communication/bus_activity.hpp"
#include "aron/irq_handlers.hpp"
#include "aron/flash_operations.hpp"
#include "aron/interupt_settings.hpp"

using namespace nd;

namespace aron
{
    void init_sequence()
    {
        aron::setup_clock_generators();
        drivers::Systick::start(SystemCoreClock, 1'000);

        nd::utils::Logger::getLogger().info("Aron - modular current power supply");
        nd::utils::Logger::getLogger().info("Init start");

        aron::Interface::init_all();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        aron::init_interupts();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        aron::nvm_ram_ecc_and_eeprom_setting();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        aron::TempMeassure::init();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        aron::Current_source::init();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        aron::Position_control::init();
        nd::chrono::wait(nd::chrono::durations::milliseconds{1});
        aron::Current_source::disable();
        nd::utils::Logger::getLogger().info("Init end");

        // WDT(WATCHDOG) ENABLE
        WDT->CTRLA.reg = WDT_CTRLA_ENABLE;
        while (WDT->SYNCBUSY.bit.ENABLE)
            ;
    }
}