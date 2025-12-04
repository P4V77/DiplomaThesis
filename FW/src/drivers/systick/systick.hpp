#pragma once

#include <cstdint>
#include <atomic>
#include <sam.h>



namespace nd::drivers
{
    class Systick
    {
        public:
            static void start(const std::uint32_t clock, const std::uint32_t target)
            {
                tick_ns = 1'000'000'000 / clock;
                SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
                SysTick->VAL = 0;
                SysTick->LOAD = (clock / target) - 1;
                SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
                NVIC_EnableIRQ(SysTick_IRQn);
            }

            static void stop()
            {
                NVIC_DisableIRQ(SysTick_IRQn);
                SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
                tick_ns = 0;
            }

            static void irq_handler()
            {
                count++;
            }

            static std::uint32_t get()
            {
                return count;
            }

            static void delay_ns(const std::uint32_t delay)
            {
                const std::uint32_t start{SysTick->VAL};
                const std::uint32_t max_delay{SysTick->LOAD * tick_ns};

                if (tick_ns == 0 || delay < tick_ns || delay >= max_delay)
                {
                    return;
                }

                const std::uint32_t ticks{delay / tick_ns};
                if (ticks < SysTick->VAL)
                {
                    const std::uint32_t end{start - ticks};
                    while (SysTick->VAL > end);
                }
                else
                {
                    const std::uint32_t end{SysTick->LOAD - (ticks - start)};
                    while (true)
                    {
                        const auto val = SysTick->VAL;
                        if (val < end && val > start)
                            break;
                    }
                }
            }

            static void delay_us(const std::uint32_t delay)
            {
                delay_ns(delay * 1'000);
            }

        private:
            static_assert(std::atomic_uint32_t::is_always_lock_free, "Atomic of std::uint32_t is not lock free");
            static std::atomic_uint32_t count;
            static std::uint32_t tick_ns;
    };
}
