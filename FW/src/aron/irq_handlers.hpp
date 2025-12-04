#pragma once

using namespace nd;

void WDT_Handler()
{
    // Disables Output
    EVSYS->SWEVT.bit.CHANNEL1 = 1;

    WDT->INTFLAG.bit.EW = 1;
}

void ADC1_1_Handler()
{
    // aron::Interface::led_yellow.high();
    aron::TempMeassure::adc1_1_handler();
    // aron::Interface::led_yellow.low();
}

void ADC0_1_Handler()
{
    // aron::Interface::led_red.high();
    aron::Current_source::adc_irq_handler();
    // aron::Interface::led_red.low();
}

void SERCOM4_2_Handler()
{
    aron::Communication::uart_rx_irq_handler();
}

void TC0_Handler()
{
    // aron::Interface::led_red.high();
    aron::DDS::tc0_handler();
    // Counter reset
    TC0->COUNT16.COUNT.reg = 0x0;
    while (TC0->COUNT16.SYNCBUSY.bit.COUNT)
        ;
    // // Flag reset
    TC0->COUNT16.INTFLAG.bit.MC0 = 1;
    // aron::Interface::led_red.low();
}

void TC2_Handler()
{
    // log().debug("ARON Button restart");
    nd::utils::Logger::getLogger().debug("ARON Button restart");

    // Safely disable output
    aron::Current_source::disable();
    // Wait for acumulated energy to be dissipated
    chrono::wait(chrono::durations::milliseconds{500});
    // Restart the system
    NVIC_SystemReset();
}

void TC3_Handler()
{
    // aron::Interface::led_red.high();
    aron::CommandSequencer::sequence_handler(); // Calling command sequencer, nothing to do with AC output, but 10khz period is just about right for command sequence
    // Counter reset
    TC3->COUNT16.COUNT.reg = 0x0;
    while (TC3->COUNT16.SYNCBUSY.bit.COUNT)
        ;
    // // Flag reset
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;
    // aron::Interface::led_red.low();
}

void RAMECC_Handler()
{
    uint32_t addr_error = RAMECC->ERRADDR.reg;

    // Reset on double bit error in one byte
    if ((RAMECC->INTFLAG.bit.DUALE) == 1)
    {
        nd::utils::Logger::getLogger().error("Can't reccover, RESTARTING SYSTEM, error at {}", addr_error);
        // Safely disable output
        aron::Current_source::disable();
        // Wait for acumulated energy to be dissipated
        chrono::wait(chrono::durations::milliseconds{2000});
        // Restart the system
        NVIC_SystemReset();
        return;
    }

    // Clearing flag on single bit error in one byte
    RAMECC->INTFLAG.reg = RAMECC_INTFLAG_SINGLEE | RAMECC_INTFLAG_DUALE;
    // ECC Corrects one bit errors automatically
}
