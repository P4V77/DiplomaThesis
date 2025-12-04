#pragma once

#include <cstdint>
#include <sam.h>



namespace nd::drivers::same5x
{
    class Mclk_ctrl
    {
        public:
            template<typename T>
            static void enable(const T* const reg)
            {
                enable_disable(reg, true);
            }

            template<typename T>
            static void disable(const T* const reg)
            {
                enable_disable(reg, false);
            }
        
        private:
            template<typename T>
            static void enable_disable(const T* const reg, const bool state)
            {
                const void* const reg_void{static_cast<const void* const>(reg)};
                    // AHB
                    if (reg_void == static_cast<void*>(CAN1)) MCLK->AHBMASK.bit.CAN1_ = state;
                    else if (reg_void == static_cast<void*>(CAN0)) MCLK->AHBMASK.bit.CAN0_ = state;
                    else if (reg_void == static_cast<void*>(SDHC0)) MCLK->AHBMASK.bit.SDHC0_ = state;

                    // APBA
                    else if(reg_void == TC1) MCLK->APBAMASK.bit.TC1_ = state;
                    else if(reg_void == TC0) MCLK->APBAMASK.bit.TC0_ = state;
                    else if(reg_void == SERCOM1) MCLK->APBAMASK.bit.SERCOM1_ = state;
                    else if(reg_void == SERCOM0) MCLK->APBAMASK.bit.SERCOM0_ = state;
                    else if(reg_void == FREQM) MCLK->APBAMASK.bit.FREQM_ = state;
                    else if(reg_void == EIC) MCLK->APBAMASK.bit.EIC_ = state;
                    else if(reg_void == RTC) MCLK->APBAMASK.bit.RTC_ = state;
                    else if(reg_void == WDT) MCLK->APBAMASK.bit.WDT_ = state;
                    else if(reg_void == GCLK) MCLK->APBAMASK.bit.GCLK_ = state;
                    else if(reg_void == SUPC) MCLK->APBAMASK.bit.SUPC_ = state;
                    else if(reg_void == OSC32KCTRL) MCLK->APBAMASK.bit.OSC32KCTRL_ = state;
                    else if(reg_void == OSCCTRL) MCLK->APBAMASK.bit.OSCCTRL_ = state;
                    else if(reg_void == RSTC) MCLK->APBAMASK.bit.RSTC_ = state;
                    else if(reg_void == MCLK) MCLK->APBAMASK.bit.MCLK_ = state;
                    else if(reg_void == PM) MCLK->APBAMASK.bit.PM_ = state;

                    // APBB
                    else if (reg_void == RAMECC) MCLK->APBBMASK.bit.RAMECC_ = state;
                    else if (reg_void == TC3) MCLK->APBBMASK.bit.TC3_ = state;
                    else if (reg_void == TC2) MCLK->APBBMASK.bit.TC2_ = state;
                    else if (reg_void == TCC1) MCLK->APBBMASK.bit.TCC1_ = state;
                    else if (reg_void == TCC0) MCLK->APBBMASK.bit.TCC0_ = state;
                    else if (reg_void == SERCOM3) MCLK->APBBMASK.bit.SERCOM3_ = state;
                    else if (reg_void == SERCOM2) MCLK->APBBMASK.bit.SERCOM2_ = state;
                    else if (reg_void == EVSYS) MCLK->APBBMASK.bit.EVSYS_ = state;
                    else if (reg_void == PORT) MCLK->APBBMASK.bit.PORT_ = state;
                    else if (reg_void == USB) MCLK->APBBMASK.bit.USB_ = state;
                    else if (reg_void == CCL) MCLK->APBCMASK.bit.CCL_ = state;
                    else if (reg_void == QSPI) MCLK->APBCMASK.bit.QSPI_ = state;
                    else if (reg_void == ICM) MCLK->APBCMASK.bit.ICM_ = state;
                    else if (reg_void == TRNG) MCLK->APBCMASK.bit.TRNG_ = state;
                    else if (reg_void == AES) MCLK->APBCMASK.bit.AES_ = state;
                    else if (reg_void == AC) MCLK->APBCMASK.bit.AC_ = state;
                    else if (reg_void == PDEC) MCLK->APBCMASK.bit.PDEC_ = state;

                    // APBC
                    else if (reg_void == TC5) MCLK->APBCMASK.bit.TC5_ = state;
                    else if (reg_void == TC4) MCLK->APBCMASK.bit.TC4_ = state;
                    else if (reg_void == TCC3) MCLK->APBCMASK.bit.TCC3_ = state;
                    else if (reg_void == TCC2) MCLK->APBCMASK.bit.TCC2_ = state;

                    // APBD
                    else if (reg_void == PCC) MCLK->APBDMASK.bit.PCC_ = state;
                    else if (reg_void == I2S) MCLK->APBDMASK.bit.I2S_ = state;
                    else if (reg_void == DAC) MCLK->APBDMASK.bit.DAC_ = state;
                    else if (reg_void == ADC1) MCLK->APBDMASK.bit.ADC1_ = state;
                    else if (reg_void == ADC0) MCLK->APBDMASK.bit.ADC0_ = state;
                    else if (reg_void == TCC4) MCLK->APBDMASK.bit.TCC4_ = state;
                    else if (reg_void == SERCOM5) MCLK->APBDMASK.bit.SERCOM5_ = state;
                    else if (reg_void == SERCOM4) MCLK->APBDMASK.bit.SERCOM4_ = state;
            }

//            RwReg* get_mask_register(const std::uint32_t mask)
//            {
//                switch (mask)
//                {
//                    case MCLK_AHBMASK_NVMCTRL_CACHE:
//                    case MCLK_AHBMASK_NVMCTRL_SMEEPROM:
//                    case MCLK_AHBMASK_QSPI_2X:
//                    case MCLK_AHBMASK_PUKCC:
//                    case MCLK_AHBMASK_ICM:
//                    case MCLK_AHBMASK_CAN1:
//                    case MCLK_AHBMASK_CAN0:
//                    case MCLK_AHBMASK_SDHC0:
//                    case MCLK_AHBMASK_QSPI:
//                    case MCLK_AHBMASK_PAC:
//                    case MCLK_AHBMASK_USB:
//                    case MCLK_AHBMASK_DMAC:
//                    case MCLK_AHBMASK_CMCC:
//                    case MCLK_AHBMASK_NVMCTRL:
//                    case MCLK_AHBMASK_DSU:
//                    case MCLK_AHBMASK_HPB3:
//                    case MCLK_AHBMASK_HPB2:
//                    case MCLK_AHBMASK_HPB1:
//                    case MCLK_AHBMASK_HPB0:
//                        return &REG_MCLK_AHBMASK;
//
//                    case MCLK_APBAMASK_TC1:
//                    case MCLK_APBAMASK_TC0:
//                    case MCLK_APBAMASK_SERCOM1:
//                    case MCLK_APBAMASK_SERCOM0:
//                    case MCLK_APBAMASK_FREQM:
//                    case MCLK_APBAMASK_EIC:
//                    case MCLK_APBAMASK_RTC:
//                    case MCLK_APBAMASK_:

//                }
//            }
    };
}
