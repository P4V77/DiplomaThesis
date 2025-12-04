#pragma once

#include <cstdint>
#include <sam.h>



namespace nd::drivers::same5x
{
    enum Xosc_startup : std::uint8_t
    {
        DELAY_31_US,
        DELAY_61_US,
        DELAY_122_US,
        DELAY_244_US,
        DELAY_488_US,
        DELAY_977_US,
        DELAY_2_MS,
        DELAY_4_MS,
        DELAY_8_MS,
        DELAY_16_MS,
        DELAY_31_MS,
        DELAY_63_MS,
        DELAY_125_MS,
        DELAY_250_MS,
        DELAY_500_MS,
        DELAY_1_S,
    };

    template<std::uint8_t id>
    requires (id < OSCCTRL_XOSCS_NUM)
    class Xoscctrl
    {
        public:
            static void enable(const std::uint32_t frequency, const Xosc_startup startup_delay)
            {
                std::uint32_t imult{};
                std::uint32_t iptat{};

                if (frequency <= 8'000'000)
                {
                    imult = 3;
                    iptat = 2;
                }
                else if (frequency <= 16'000'000)
                {
                    imult = 4;
                    iptat = 3;
                }
                else if (frequency <= 24'000'000)
                {
                    imult = 5;
                    iptat = 3;
                }
                else if (frequency <= 48'000'000)
                {
                    imult = 6;
                    iptat = 3;
                }

                OSCCTRL->XOSCCTRL[id].reg = OSCCTRL_XOSCCTRL_STARTUP(startup_delay) |
                        OSCCTRL_XOSCCTRL_ENALC |
                        OSCCTRL_XOSCCTRL_IMULT(imult) |
                        OSCCTRL_XOSCCTRL_IPTAT(iptat) |
                        OSCCTRL_XOSCCTRL_XTALEN |
                        OSCCTRL_XOSCCTRL_ENABLE;

                while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_XOSCRDY(1<<id)));
            }

            static void disable()
            {
                OSCCTRL->XOSCCTRL[id].reg = 0;
            }
    };
}
