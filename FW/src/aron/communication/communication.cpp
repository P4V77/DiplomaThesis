#include "communication.hpp"

using namespace aron::UniqueMessageIDs;
namespace aron
{
    namespace
    {
        auto log = []() -> nd::utils::Logger &
        {
            return nd::utils::Logger::getLogger();
        };
    }

    void Communication::init(nd::drivers::CAN_interface &can_p, const std::uint8_t id_p)
    {
        can = &can_p;

        // After programming check if eeprom_id_set, so it wont do every restart
        if ((*eeprom_id_set != 0x00u))
        {
            erase_eeprom_page();
            set_source_id_to_eeprom(id_p); // Sets ID after programming to ID defined in main()
        }
        else
        {
            id = id_eeprom; // Set id to stored ID value
        }

        // SERCOM4 RX
        SERCOM4->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;

        log().debug("EEMPROM ID={}", id_eeprom);
        log().info("COM: RAM ID={}", id);
    }

    void Communication::uart_rx_irq_handler()
    {
        uart_rx_fifo.write(static_cast<std::uint8_t>(SERCOM4->USART.DATA.reg));
    }

    void Communication::rx_handler()
    {
        uart_handler();
        can_handler();
    }

    uint8_t Communication::get_source_id()
    {
        return id;
    }

    void Communication::set_source_id(uint8_t id_p)
    {
        id = id_p;
        set_source_id_to_eeprom(id_p);
    }

    void Communication::set_source_id_to_eeprom(uint8_t id_p)
    {
        while (NVMCTRL->SEESTAT.bit.BUSY)
            ;
        id_eeprom = id_p; // WRITE ID = 1 to EEPROM
    }

    void Communication::erase_eeprom_page()
    {
        while (NVMCTRL->SEESTAT.bit.BUSY)
            ;
        // Erase EEPROM
        nd::utils::Logger::getLogger().debug("Erasing EEPOROM page...");
        NVMCTRL->ADDR.reg = (uint32_t)(eeprom_addr_p);
        NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_EP | NVMCTRL_CTRLB_CMDEX_KEY;
        while (!(NVMCTRL->STATUS.bit.READY))
            ;
        NVMCTRL->INTFLAG.bit.DONE = 1;
        nd::utils::Logger::getLogger().debug("EEPOROM erased");

        *eeprom_id_set = 0x00u; // Writing 0x00 makes eeprom flag to not set id to 1 after every restart
    }

    void Communication::uart_handler()
    {
        while (!uart_rx_fifo.is_empty())
        {
            uart_rx_timeout.start();
            const auto rx_char{uart_rx_fifo.read()};

            if (rx_char == '\n' || rx_char == '\r')
            {
                if (uart_msg_ptr)
                {
                    const std::string_view msg{uart_msg_buff.begin(), uart_msg_ptr};
                    log().debug("COM: UART got: {} ({}B)", msg, msg.length());
                    process_message(msg, true);
                    uart_msg_ptr = 0;
                }
                uart_rx_timeout.reset();
            }
            else
            {
                uart_msg_buff[uart_msg_ptr] = rx_char;
                ++uart_msg_ptr;

                if (uart_msg_ptr >= uart_msg_buff.size())
                {
                    log().error("*E COM: UART msg too long ({}B)", uart_msg_ptr);
                    uart_msg_ptr = 0;
                    uart_rx_timeout.reset();
                }
            }
        }

        if (uart_rx_timeout.is_time())
        {
            log().error("*E COM: UART msg timeouted (1s)");
            uart_msg_ptr = 0;
        }
    }

    void Communication::can_handler()
    {
        while (can->is_new_message_received())
        {
            // ** NEW CAN MESSAGE FORMAT RECIEVE!!!!! **
            nd::drivers::CAN_message can_msg{};
            can->receive(can_msg);
            const auto len_data{strnlen(reinterpret_cast<const char *>(can_msg.data.data()), can_msg.size)};

            char id_str[4]; // Assuming max 3 digits for ID + null terminator
            snprintf(id_str, sizeof(id_str), "%u", static_cast<uint>(can_msg.id));

            constexpr size_t MAX_CAN_DATA_SIZE = 64; // Or 8 for standard CAN
            char data_str[MAX_CAN_DATA_SIZE + 1];
            strncpy(data_str, reinterpret_cast<const char *>(can_msg.data.data()), len_data);
            data_str[len_data] = '\0'; // Ensure null termination

            char combined_msg[128]; // Adjust size as needed
            snprintf(combined_msg, sizeof(combined_msg), "%s%s", id_str, data_str);

            std::string_view final_msg{combined_msg, strlen(combined_msg)};

            log().debug("COM: CAN got: {} ({}B)", final_msg, len_data);
            process_message(final_msg, false);
        }
    }

    void Communication::send_can_message(const std::string_view msg)
    {
        // ** New CAN Message Format **
        uint32_t id_can = id;
        int32_t id_from_msg = eval_id(msg); // Find ID

        if (id_from_msg == -1) // Check if ID is invalid
        {
            log().error("*E COM: Skipping CAN message due to invalid ID: {}", msg);
            return; // Return early if ID is invalid
        }

        // Checking message for broadcast, heartbeat and so on
        for (size_t i = 0; i < valid_msg_ids_size; ++i)
        {
            if (valid_msg_ids[i] == id_from_msg)
            {
                // If ID in message is one of those, then set id as id from message
                id_can = (uint32_t)id_from_msg;
                break;
            }
        }

        uint8_t first_letter_position; // Find first letter
        find_first_letter(msg, &first_letter_position);

        // Create a new string_view without the ID
        std::string_view msg_without_id{msg.data() + first_letter_position, msg.length() - first_letter_position};

        // log().debug("COM: sending msg to CAN ({}{})", id_from_msg, msg_without_id);

        nd::drivers::CAN_message can_msg{};
        can_msg.id = id_can;
        can_msg.is_fd = true;
        can_msg.size = static_cast<uint8_t>(msg_without_id.length());
        std::copy(msg_without_id.begin(), msg_without_id.end(), can_msg.data.begin());
        can->send(can_msg);
    }

    void Communication::send_can_message(const std::string_view msg, const uint32_t &id_from_msg)
    {
        uint8_t first_letter_position; // Find first letter
        find_first_letter(msg, &first_letter_position);
        // Create a new string_view without the ID
        std::string_view msg_without_id{msg.data() + first_letter_position, msg.length() - first_letter_position};

        nd::drivers::CAN_message can_msg{};
        can_msg.id = id_from_msg;
        can_msg.is_fd = true;
        can_msg.size = static_cast<uint8_t>(msg_without_id.length());
        std::copy(msg_without_id.begin(), msg_without_id.end(), can_msg.data.begin());
        can->send(can_msg);
        log().debug("COM: sending msg to CAN ({})", msg);
    }

    int16_t Communication::eval_id(const std::string_view msg)
    {
        int16_t id_from_msg;
        auto result = std::from_chars(msg.data(), msg.data() + msg.size(), id_from_msg);

        if (result.ec != std::errc())
        {
            log().error("*E COM: msg ID is not a valid number");
            return -1;
        }
        return id_from_msg;
    }

    void Communication::find_first_letter(const std::string_view msg, uint8_t *first_letter_position)
    {
        auto it = std::find_if(msg.begin(), msg.end(), [](char c)
                               { return std::isalpha(c); });
        if (it == msg.end())
        {
            // log().error("*E COM: No alphabetic character found in message");
            return;
        }
        *first_letter_position = static_cast<uint8_t>(std::distance(msg.begin(), it));
    }

    void Communication::process_message(const std::string_view msg, const bool redirect_to_can)
    {
        if (msg.length() < 3)
        {
            log().error("*E COM: msg too short ({} < 3)", msg.length());
            return;
        }

        int16_t id_from_msg = eval_id(msg);

        // If eval_id fails, it returns -1.  Check for this.
        if (id_from_msg == -1)
        {
            return; // Exit if the ID is invalid
        }

        bool valid_for_processing = false;

        // Checking if the message ID matches our device ID
        if (id_from_msg == id)
        {
            valid_for_processing = true;
        }
        else
        {
            // Checking for valid global messages (heartbeat, broadcast, etc.)
            for (size_t i = 0; i < valid_msg_ids_size; ++i)
            {
                if (id_from_msg == valid_msg_ids[i])
                {
                    valid_for_processing = true;
                    break;
                }
            }
        }

        // If it's from UART or Broadcast message, redirect it to CAN
        if (redirect_to_can && !valid_for_processing)
        {
            log().debug("Redirecting message to CAN: {}", msg);
            send_can_message(msg, static_cast<uint32_t>(id_from_msg));
            return; // Stop processing  
        }

        // If the message ID is **not** meant for this device
        if (!valid_for_processing && id_from_msg != Broadcast)
        {
            // log().error("*E COM: msg ID {} not valid for this device", id_from_msg);
            return; // Stop processing
        }

        // Process message since ID passed validation
        uint8_t first_letter_position;
        find_first_letter(msg, &first_letter_position);

        // Fixed-size command buffer
        char command_buffer[3] = {0}; // Ensure null termination
        command_buffer[0] = msg[first_letter_position];

        if (msg.length() > (first_letter_position + 1u) && std::isalpha(msg[first_letter_position + 1u]))
        {
            command_buffer[1] = msg[first_letter_position + 1];
        }

        std::string_view command_str(command_buffer);
        size_t value_start_position = first_letter_position + command_str.length();

        std::int32_t value{0};
        auto [ptr, ec] = std::from_chars(msg.begin() + value_start_position, msg.end(), value);
        if (ec == std::errc::invalid_argument)
        {
            return;
        }

        aron::CommandHandler::message_handler(command_str, value);
    }
}
