#include "systick.hpp"



std::atomic_uint32_t nd::drivers::Systick::count{};
std::uint32_t nd::drivers::Systick::tick_ns{};

void SysTick_Handler()
{
    nd::drivers::Systick::irq_handler();
}
