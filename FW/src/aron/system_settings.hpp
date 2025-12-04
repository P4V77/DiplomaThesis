#pragma once

#include <sam.h>
#include "drivers/sam5x/gclk.hpp"
#include "drivers/sam5x/mclk.hpp"
#include "utils/adc_dac_utils.hpp"
#include "utils/logging/Logger.hpp"
#include "utils/logging/LoggerUart.hpp"

#include "aron/output/current_source.hpp"

namespace aron
{
    static inline void setup_clock_generators()
    {
        // 20MEG crystal
        OSCCTRL->XOSCCTRL[1]
            .reg = OSCCTRL_XOSCCTRL_STARTUP(0xA) | OSCCTRL_XOSCCTRL_ENALC | OSCCTRL_XOSCCTRL_IMULT(0x5) | OSCCTRL_XOSCCTRL_IPTAT(0x3) | OSCCTRL_XOSCCTRL_XTALEN | OSCCTRL_XOSCCTRL_ENABLE;
        while (!OSCCTRL->STATUS.bit.XOSCRDY1)
            ;

        // GCLK[1] 1MHz
        nd::drivers::same5x::Gclk_genctrl<1>::enable(20, GCLK_GENCTRL_SRC_XOSC1);

        // DPLL 120MHz
        nd::drivers::same5x::Gclk_pchctrl::enable(OSCCTRL_GCLK_ID_FDPLL0, 1);

        OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK |
                                         OSCCTRL_DPLLCTRLB_DCOFILTER(0x0) |
                                         OSCCTRL_DPLLCTRLB_DCOEN;
        OSCCTRL->Dpll[0].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDR(120);
        while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.DPLLRATIO)
            ;

        OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 1;
        while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.ENABLE)
            ;
        // while (!(OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY))
        //     ;

        // DPLL1 200MHz
        nd::drivers::same5x::Gclk_pchctrl::enable(OSCCTRL_GCLK_ID_FDPLL1, 1);

        OSCCTRL->Dpll[1].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK |
                                         OSCCTRL_DPLLCTRLB_DCOFILTER(0x0) |
                                         OSCCTRL_DPLLCTRLB_DCOEN;
        OSCCTRL->Dpll[1].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDR(200);
        while (OSCCTRL->Dpll[1].DPLLSYNCBUSY.bit.DPLLRATIO)
            ;

        OSCCTRL->Dpll[1].DPLLCTRLA.bit.ENABLE = 1;
        while (OSCCTRL->Dpll[1].DPLLSYNCBUSY.bit.ENABLE)
            ;
        // while (!OSCCTRL->Dpll[1].DPLLSTATUS.bit.CLKRDY)
        //     ;

        // GCLK[0] 120MHz
        nd::drivers::same5x::Gclk_genctrl<0>::enable(1, GCLK_GENCTRL_SRC_DPLL0);
        SystemCoreClock = 120'000'000;

        // GCLK[2] 20MHz
        nd::drivers::same5x::Gclk_genctrl<2>::enable(1, GCLK_GENCTRL_SRC_XOSC1);

        // GCLK[3] 30MHz
        nd::drivers::same5x::Gclk_genctrl<3>::enable(4, GCLK_GENCTRL_SRC_DPLL0);

        // GCLK[4] 200 MHz
        nd::drivers::same5x::Gclk_genctrl<4>::enable(1, GCLK_GENCTRL_SRC_DPLL1);

        // Disable FDPLL
        OSCCTRL->DFLLCTRLA.reg = 0;
        while (OSCCTRL->DFLLSYNC.bit.ENABLE)
            ;

        // VCORE buck converter
        SUPC->VREG.bit.SEL = 1; // Broken with PLLs
        while (!SUPC->STATUS.bit.VREGRDY)
            ;

        // ADC1 config for Temp Sensing
        nd::drivers::same5x::Mclk_ctrl::enable(ADC1);
        nd::drivers::same5x::Gclk_pchctrl::enable(ADC1_GCLK_ID, 3); // 30 MHz
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pb08}.enable_alternate_function(nd::drivers::Gpio_alt_functions::B);
        ADC1->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV2; // 15 MHz
        // REFERENCE VREFA
        nd::drivers::Gpio_same5x{nd::drivers::gpio_pins::pa04}.enable_alternate_function(nd::drivers::Gpio_alt_functions::B);
        ADC1->REFCTRL.reg = ADC_REFCTRL_REFSEL_AREFB; // ADC_REFCTRL_REFSEL_INTREF;
        while (ADC1->SYNCBUSY.bit.REFCTRL)
            ;
        // REFERENCE VREFA
        ADC1->SAMPCTRL.reg = ADC_SAMPCTRL_OFFCOMP;
        while (ADC1->SYNCBUSY.bit.SAMPCTRL)
            ;
        ADC1->CALIB.reg = ADC_CALIB_BIASREFBUF(*(std::uint32_t *)ADC1_FUSES_BIASREFBUF_ADDR >> ADC1_FUSES_BIASREFBUF_Pos) |
                          ADC_CALIB_BIASR2R(*(std::uint32_t *)ADC1_FUSES_BIASR2R_ADDR >> ADC1_FUSES_BIASR2R_Pos) |
                          ADC_CALIB_BIASCOMP(*(std::uint32_t *)ADC1_FUSES_BIASCOMP_ADDR >> ADC1_FUSES_BIASCOMP_Pos);
        ADC1->INTENSET.reg = ADC_INTENSET_RESRDY;
        ADC1->CTRLB.reg = ADC_CTRLB_RESSEL_16BIT;
        while (ADC1->SYNCBUSY.bit.CTRLB)
            ;
        // Average 64 samples
        // ADC1->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(0x6) | ADC_AVGCTRL_ADJRES(0x4);
         // Average 256 samples
        ADC1->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(0xA) | ADC_AVGCTRL_ADJRES(0x4);
        while (ADC1->SYNCBUSY.bit.AVGCTRL)
            ;

        // WTD
        nd::drivers::same5x::Mclk_ctrl::enable(WDT); // 1khz internal OSC ULPW
        WDT->EWCTRL.reg = WDT_CONFIG_PER_CYC256;
        WDT->CONFIG.reg = WDT_CONFIG_PER_CYC1024;
        WDT->INTENSET.bit.EW = 1;

        // EVSYS
        nd::drivers::same5x::Mclk_ctrl::enable(EVSYS);
        EVSYS->PRICTRL.bit.RREN = 1; // Round robin priority
    }

    static inline void watch_dog_reset()
    {
        // This function resets the Watchdog Timer to prevent system reset
        if (!WDT->SYNCBUSY.bit.CLEAR)
        {
            // WDT RESET VALUE (Writing any other value than 0xA5 resets system)
            WDT->CLEAR.reg = 0xA5;
        }
    }
}
