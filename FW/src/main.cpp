#include "aron/init.hpp"

using namespace nd;

int main()
{
    aron::init_sequence();                              // Initialization Of MCU And Its Peripherials
    aron::Communication::init(aron::Interface::can, 1); // <-- Adjust Defualtly Programmed CAN ID Here
    // aron::Interface::init_can_logger();              // <-- Logs On UART When Commented, Uncomment For Redirection Of Logs To CAN

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true)
    {
        aron::Communication::rx_handler(); // Proccessing Messages From CAN And UART
        aron::TimerManager::run();         // General Timed Events
        aron::watch_dog_reset();           // WatchDog Feeding
    }
#pragma clang diagnostic pop
}