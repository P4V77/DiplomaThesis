#include "timer_manager.hpp"

namespace aron
{
    chrono::Periodic_timer TimerManager::status_led_timer{chrono::durations::milliseconds{500}};
    chrono::Periodic_timer TimerManager::status_temp_timer{chrono::durations::milliseconds{10000}};
    chrono::Periodic_timer TimerManager::source_timeout{chrono::durations::milliseconds{12000}};
    chrono::Periodic_timer TimerManager::can_heart_beat{chrono::durations::milliseconds{3000}};
    // chrono::Periodic_timer TimerManager::can_heart_beat{chrono::durations::milliseconds{1000}};
    chrono::Periodic_timer TimerManager::can_info{chrono::durations::milliseconds{1}};
    chrono::Timeout_timer TimerManager::button_hold_timer{chrono::durations::milliseconds{2000}}; //

    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void TimerManager::run()
    {

        if (status_led_timer.is_time())
        {
            handleStatusLedTimer(); // Status LED toggle
        }
        if (status_temp_timer.is_time())
        {
            handleStatusTempTimer(); // Temp check
        }
        if (can_heart_beat.is_time())
        {
            handleCanHeartBeat();
        }
        if (source_timeout.is_time())
        {
            handleSourceTimeout();
        }
        if (can_info.is_time())
        {
            handleCanInfo();
        }
        if (Interface::button_1.is_set())
        {
            handleButtonPress(); // Check button press
        }
    }

    void TimerManager::handleStatusLedTimer()
    {
        aron::Interface::led_green.toggle();
        (aron::Current_source::is_enabled()) ? aron::Interface::led_yellow.high() : aron::Interface::led_yellow.low();
    }

    void TimerManager::handleSourceTimeout()
    {
        aron::BusActivity::cleanup_inactive_sources();
    }

    void TimerManager::handleCanHeartBeat()
    {

        aron::BusActivity::send_heart_beat();

        // Disable ADC1 interupt on RESRDY
        // ADC1->INTENCLR.bit.RESRDY = 1;
        // ADC1->SWTRIG.bit.FLUSH = 1;
        // ADC1->INPUTCTRL.reg = (ADC_INPUTCTRL_MUXPOS_AIN6 | ADC_INPUTCTRL_MUXNEG_GND);

        // while (ADC1->SYNCBUSY.bit.SWTRIG)
        //     ;

        // while (!ADC1->INTFLAG.bit.RESRDY)
        //     ;

        // float voltage_adc = nd::utils::digital_to_analog_voltage(12, aron::Current_source::get_adc_vref(), ADC1->RESULT.reg);

        // // Substract max voltage from table and meassured voltage
        // log().debug("Voltage {}", voltage_adc);
        // ADC1->INTENSET.bit.RESRDY = 1;

        // log().debug("pos act {} ", aron::Position_control::get_actual_position());
        // log().debug("id {} ", aron::Communication::get_source_id());
        // std::array<char, 64> buffer; // Static buffer
        // auto [iterator1, count1] = fmt::format_to_n(buffer.begin(), buffer.size(),
        //                                             "255H1");
        // const std::string_view msg{buffer.begin(), buffer.begin() + count1};
        // Communication::process_message(msg, true);

        // auto [iterator2, count2] = fmt::format_to_n(buffer.begin(), buffer.size(),
        //                                             "255P1");
        // const std::string_view msg2{buffer.begin(), buffer.begin() + count2};
        // Communication::process_message(msg2, true);
    }

    void TimerManager::handleCanInfo()
    {
        aron::Position_control::log_position_can();
    }

    void TimerManager::handleButtonPress()
    {
        aron::Interface::button_press();
    }

    void TimerManager::handleStatusTempTimer()
    {
        ADC1->SWTRIG.bit.FLUSH = 1;
        ADC1->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS_AIN0 | ADC_INPUTCTRL_MUXNEG_GND;
        EVSYS->SWEVT.bit.CHANNEL14;
    }

} // namespace aron