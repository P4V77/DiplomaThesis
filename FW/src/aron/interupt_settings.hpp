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
    void init_interupts()
    {
        // RAMECC INTERUPT SET
        RAMECC->INTENSET.reg = RAMECC_INTENSET_SINGLEE |
                               RAMECC_INTENSET_DUALE;

        // NVIC Priotity
        NVIC_SetPriority(WDT_IRQn, 0);        // Watchdog
        NVIC_SetPriority(RAMECC_IRQn, 1);     // RAM ECC
        NVIC_SetPriority(TC0_IRQn, 2);        // DDS
        NVIC_SetPriority(ADC1_1_IRQn, 3);     // CUrrent source
        NVIC_SetPriority(ADC0_1_IRQn, 4);     // Position Control
        NVIC_SetPriority(SERCOM4_2_IRQn, 20); // Communication
        NVIC_SetPriority(TC2_IRQn, 254);      // Button reset

        // NVIC Enable
        NVIC_EnableIRQ(ADC1_1_IRQn);
        NVIC_EnableIRQ(ADC0_1_IRQn);
        NVIC_EnableIRQ(TC0_IRQn);
        NVIC_EnableIRQ(TC2_IRQn);
        NVIC_EnableIRQ(TC3_IRQn);
        NVIC_EnableIRQ(WDT_IRQn);
        NVIC_EnableIRQ(RAMECC_IRQn);
        NVIC_EnableIRQ(SERCOM4_2_IRQn);
    }
}