#pragma once

#include <cstdint>
#include <sam.h>

#include "utils/logging/Logger.hpp"



namespace nd::drivers::same5x
{
    template <std::uint8_t id>
    requires (id < GCLK_GEN_NUM)
    class Gclk_genctrl
    {
        public:
            static void enable(const std::uint32_t div, const std::uint32_t source)
            {
                GCLK->GENCTRL[id].reg = GCLK_GENCTRL_DIV(div) |
                        GCLK_GENCTRL_GENEN |
                        (source & GCLK_GENCTRL_SRC_Msk);
                while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(id));
            }

            static void disable()
            {
                GCLK->GENCTRL[id].reg = 0;
                while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(id));
            }

            static void print_status()
            {
                auto& log = utils::Logger::getLogger();

                for (std::uint32_t i=0; i<GCLK_GEN_NUM; ++i)
                {
                    log.debug("GCLK.GEN[{:02}] =0b{:016b}", i, static_cast<std::uint16_t>(GCLK->GENCTRL[i].reg));
                }
            }
    };

    class Gclk_pchctrl
    {
        public:
            static void enable(const std::uint8_t id, const std::uint32_t source_gen)
            {
                GCLK->PCHCTRL[id].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(source_gen);
                while (!GCLK->PCHCTRL[id].bit.CHEN);
            }

            static void disable(const std::uint8_t id)
            {
                GCLK->PCHCTRL[id].reg = 0;
                while (GCLK->PCHCTRL[id].bit.CHEN);
            }

            static std::uint8_t get_id(Sercom* sercom)
            {
                if (sercom == SERCOM0)
                    return SERCOM0_GCLK_ID_CORE;
                else if (sercom == SERCOM1)
                    return SERCOM1_GCLK_ID_CORE;
                else if (sercom == SERCOM2)
                    return SERCOM2_GCLK_ID_CORE;
                else if (sercom == SERCOM3)
                    return SERCOM3_GCLK_ID_CORE;
                else if (sercom == SERCOM4)
                    return SERCOM4_GCLK_ID_CORE;
                else if (sercom == SERCOM5)
                    return SERCOM5_GCLK_ID_CORE;
                else
                    return 0;
            };

            static std::uint8_t get_id(Tc* tc)
            {
                if (tc == TC0 || tc == TC1)
                    return TC0_GCLK_ID;
                else if (tc == TC2 || tc == TC3)
                    return TC2_GCLK_ID;
                else if (tc == TC4 || tc == TC5)
                    return TC4_GCLK_ID;
                else
                    return 0;
            };

            static void print_status()
            {
                auto& log = utils::Logger::getLogger();

                for (std::uint32_t i=0; i<GCLK_NUM; ++i)
                {
                    log.debug("GCLK.USER[{:02}]=0b{:016b}", i, static_cast<std::uint16_t>(GCLK->PCHCTRL[i].reg));
                }
            }
    };
}
