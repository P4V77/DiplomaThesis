#pragma once

#include <cstdint>
#include <array>
#include <string_view>
#include <sam.h>
#include <cctype>
#include <charconv>

#include "drivers/gpio/gpio_same5x.hpp"
#include "drivers/systick/systick.hpp"
#include "drivers/can/transcievers/tcan334.h"
#include "drivers/can/can_same5x.hpp"
#include "drivers/sam5x/gclk.hpp"
#include "drivers/sam5x/mclk.hpp"

#include "utils/logging/Logger.hpp"
#include "utils/logging/LoggerUart.hpp"
#include "utils/logging/LoggerCan.hpp"

#include "aron/communication/communication.hpp"

namespace aron
{

    using namespace nd;
    class Interface
    {
    public:
        /* LEDs */
        static inline drivers::Gpio_same5x led_green{drivers::gpio_pins::pa09};
        static inline drivers::Gpio_same5x led_yellow{drivers::gpio_pins::pa07};
        static inline drivers::Gpio_same5x led_red{drivers::gpio_pins::pa08};
        /* BUTTON */
        static inline drivers::Gpio_same5x button_1{drivers::gpio_pins::pa15, true};
        /* CAN */
        static inline drivers::TCAN334<drivers::Gpio_same5x> can_xcvr{drivers::gpio_pins::pa17, drivers::gpio_pins::pb11};
        static inline drivers::CAN_ATSAM_C_E can{CAN1, drivers::gpio_pins::pb13, drivers::gpio_pins::pb12};
        /* LOGGER */
        static inline nd::utils::LoggerUart logger{SERCOM4, drivers::gpio_pins::pa13, drivers::gpio_pins::pa12, 921'600, drivers::Gpio_alt_functions::D, 1, 0, 0};
        static inline nd::utils::LoggerCan can_logger; // Define the static member can_logger

        static void init_led();
        static void init_button();
        static void init_communication();
        static void init_can_logger();
        static void init_all();
        static void button_press();
    };
}