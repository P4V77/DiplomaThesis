#pragma once

#include <cstdint>
#include <limits>
#include "sam.h"

#include "drivers/can/can_interface.hpp"
#include "drivers/gpio/gpio_same5x.hpp"
#include "fmt/format.h"



namespace nd::drivers
{
    class CAN_ATSAM_C_E : public CAN_interface
    {
        public:
            explicit CAN_ATSAM_C_E(Can* const can_p, const drivers::Gpio_pin rx_pin_p, const drivers::Gpio_pin tx_pin_p)
                : can(*can_p), rx_pin{rx_pin_p}, tx_pin{tx_pin_p}
            {}

            [[nodiscard]] CAN_states get_bus_state() const
            {
                using enum CAN_states;
                if (can.PSR.bit.BO)
                    return BUSS_OFF;

                if (can.PSR.bit.EP == 0u)
                    return ERROR_ACTIVE;
                else
                    return ERROR_PASSIVE;
            }

            [[nodiscard]] CAN_activity get_bus_activity() const
            {
                switch (can.PSR.bit.ACT)
                {
                    using enum CAN_activity;

                    case CAN_PSR_ACT_SYNC_Val:
                        return SYNC;

                    case CAN_PSR_ACT_IDLE_Val:
                        return IDLE;

                    case CAN_PSR_ACT_RX_Val:
                        return RX;

                    case CAN_PSR_ACT_TX_Val:
                        return TX;
                }
            }

            void init() final;
            void send(const CAN_message& msg) final;
            void receive(CAN_message& msg) final;
            [[nodiscard]] bool is_new_message_received() final;
            void go_to_sleep() final;
            void wakeup_from_sleep() final;

        private:
            Can& can;
            const drivers::Gpio_same5x rx_pin;
            const drivers::Gpio_same5x tx_pin;

            void init_pins()
            {
                if (&can == CAN0)
                {
                    rx_pin.enable_alternate_function(Gpio_alt_functions::I);
                    tx_pin.enable_alternate_function(Gpio_alt_functions::I);
                }
                else if (&can == CAN1)
                {
                    rx_pin.enable_alternate_function(Gpio_alt_functions::H);
                    tx_pin.enable_alternate_function(Gpio_alt_functions::H);
                }
            }

            [[nodiscard]] std::uint8_t get_GCLK_ID() const
            {
                if (&can == CAN0)
                {
                    return static_cast<std::uint8_t>(CAN0_GCLK_ID);
                }
                else if (&can == CAN1)
                {
                    return static_cast<std::uint8_t>(CAN1_GCLK_ID);
                }
                else
                {
                    // If instance of CAN bus peripheral is not matched, return number of GCLK users.
                    // This will lead to HW exception/error - not a great solution, but solution.
                    return static_cast<std::uint8_t>(GCLK_NUM);
                }
            }

            [[nodiscard]] IRQn get_NVIC_IRQn() const
            {
                if (&can == CAN0)
                {
                    return CAN0_IRQn;
                }
                else if (&can == CAN1)
                {
                    return CAN1_IRQn;
                }
                else
                {
                    // If instance of CAN bus peripheral is not matched, return number of interrupt vectors.
                    // This will lead to HW exception/error - not a great solution, but solution.
                    return PERIPH_COUNT_IRQn;
                }
            }

            void init_clock(const std::uint8_t gclk_generator = 0) const
            {
                if (gclk_generator > GCLK_GEN_NUM_MSB)
                    return;

                if (&can == CAN0)
                {
                    MCLK->AHBMASK.bit.CAN0_ = 1;
                }
                else if (&can == CAN1)
                {
                    MCLK->AHBMASK.bit.CAN1_ = 1;
                }

                const auto gclk_id = get_GCLK_ID();

                GCLK->PCHCTRL[gclk_id].reg = GCLK_PCHCTRL_GEN(gclk_generator) | GCLK_PCHCTRL_CHEN;
                while (!GCLK->PCHCTRL[gclk_id].bit.CHEN);
            }

            void enable_NVIC_irq() const
            {
                NVIC_EnableIRQ(get_NVIC_IRQn());
            }

            void disable_NVIC_irq() const
            {
                NVIC_DisableIRQ(get_NVIC_IRQn());
            }

            void start_init() const
            {
                can.CCCR.bit.INIT = 1u;
                while (can.CCCR.bit.INIT != 1u);

                can.CCCR.bit.CCE = 1;
            }

            void stop_init() const
            {
                can.CCCR.bit.INIT = 0u;
                while (can.CCCR.bit.INIT != 0u);
            }

            void enable_CAN_FD() const
            {
                can.CCCR.bit.FDOE = 1;
            }

            void disable_CAN_FD() const
            {
                can.CCCR.bit.FDOE = 0;
            }

            void send_test_msg() const;
            void receive_test() const;
    };
}

template<>
struct fmt::formatter<nd::drivers::CAN_states>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(nd::drivers::CAN_states const& state, FormatContext& ctx)
    {
        switch (state)
        {
            using enum nd::drivers::CAN_states;

            case ERROR_ACTIVE:
                return fmt::format_to(ctx.out(), "ERROR ACTIVE");
            case ERROR_PASSIVE:
                return fmt::format_to(ctx.out(), "ERROR PASSIVE");
            case BUSS_OFF:
                return fmt::format_to(ctx.out(), "BUS OFF");
            default:
                return fmt::format_to(ctx.out(), "PARSE ERROR");
        }
    }
};

template<>
struct fmt::formatter<nd::drivers::CAN_activity>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(nd::drivers::CAN_activity const& state, FormatContext& ctx)
    {
        switch (state)
        {
            using enum nd::drivers::CAN_activity;

            case SYNC:
                return fmt::format_to(ctx.out(), "SYNC");
            case IDLE:
                return fmt::format_to(ctx.out(), "IDLE");
            case RX:
                return fmt::format_to(ctx.out(), "RX");
            case TX:
                return fmt::format_to(ctx.out(), "TX");
            default:
                return fmt::format_to(ctx.out(), "PARSE ERROR");
        }
    }
};

//template<>
//struct fmt::formatter<CanMramRxbe>
//{
//    template<typename ParseContext>
//    constexpr auto parse(ParseContext& ctx)
//    {
//        return ctx.begin();
//    }
//
//    template<typename FormatContext>
//    auto format(CanMramRxbe& msg, FormatContext& ctx)
//    {
//        fmt::format_to(ctx.out(), "CAN message\n  ID={}\n  FD={}\n  DLC={}\n\n", msg.RXBE_0.bit.ID,
//                       msg.RXBE_1.bit.FDF,
//                       msg.RXBE_1.bit.DLC);
//    }
//};
