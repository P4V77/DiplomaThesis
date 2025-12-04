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
    static constexpr uint32_t WORDS_IN_PAGE = FLASH_PAGE_SIZE / sizeof(uint32_t);
    static constexpr uint32_t WORDS_PER_BUFFER = 128 / 32; // 128-bit buffer = 4 words

    void nvm_ram_ecc_and_eeprom_setting()
    {

        // nd::utils::Logger::getLogger().debug("RAMECC STATUS {}", RAMECC->STATUS.bit.ECCDIS);
        nd::utils::Logger::getLogger().debug("Starting RAMECC Configuration...");

        /* USER ROW ptrs */
        uint32_t *user_row_ptr = (uint32_t *)USER_ROW;
        uint32_t user_row_data[8]; // Read first 5 words (160 bits)

        // Step 1: Read and save USER ROW data
        for (int i = 0; i < 5; i++)
        {
            user_row_data[i] = *(user_row_ptr + i);
        }

        for (int i = 5; i < 8; i++)
        {
            user_row_data[i] = 0xFFFFFFFF;
        }

        /* SAFE USER ROW CONFIG */
        // user_row_data[0] = 0xFE9A9239;
        // user_row_data[1] = 0xAEECFF80;
        // user_row_data[2] = 0xFFFFFFFF;
        // user_row_data[3] = 0xFFFFFFFF;
        // user_row_data[4] = 0x00804010;
        // user_row_data[5] = 0xFFFFFFFF;
        // user_row_data[6] = 0xFFFFFFFF;
        // user_row_data[7] = 0xFFFFFFFF;
        /* SAFE USER ROW CONFIG */

        if (user_row_data[1] != 0xAEECFF01)
        {
            nd::utils::Logger::getLogger().debug("Reading current USER ROW data:");
            __disable_irq();

            nd::utils::Logger::getLogger().debug("Currently saved USER ROW data:");
            for (int i = 0; i < 8; i++)
            {
                nd::utils::Logger::getLogger().debug("user_row_data[{}] = 0x{:08X}", i, user_row_data[i]);
            }
            nd::utils::Logger::getLogger().debug("Only first 5 words from whole USER page are config and must be stored and set after erase");

            // Step 2: Modify only the relevant ECC bit (Bit 39 in word 1)
            nd::utils::Logger::getLogger().debug("Modifying USER ROW data to ensure ECC is enabled (clearing bits 32-39 and setting bit 39 of word 1).");
            uint32_t original_word1 = user_row_data[1];
            user_row_data[1] = 0xAEECFF01; // Set SBLK to 1, Set PSZ to 0,  ECCDIS is 0 (enable ECC)

            nd::utils::Logger::getLogger().debug("USER ROW word 1 before modification: 0x{:08X}", original_word1);
            nd::utils::Logger::getLogger().debug("USER ROW word 1 after modification:  0x{:08X}", user_row_data[1]);

            // Ensure the NVM is ready before writing
            while (!(NVMCTRL->STATUS.bit.READY))
                ;
            nd::utils::Logger::getLogger().debug("NVM controller ready.");

            // Step 3: Erase the User Page
            nd::utils::Logger::getLogger().debug("Erasing User Page...");
            NVMCTRL->ADDR.reg = (uint32_t)(user_row_ptr);
            NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_EP | NVMCTRL_CTRLB_CMDEX_KEY;
            while (!(NVMCTRL->STATUS.bit.READY))
                ;
            NVMCTRL->INTFLAG.bit.DONE = 1; // Clear flag after command execution
            nd::utils::Logger::getLogger().debug("User Page erased.");

            // Step 4: Clear Page Buffer
            nd::utils::Logger::getLogger().debug("Clearing Page Buffer...");
            NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_PBC | NVMCTRL_CTRLB_CMDEX_KEY;
            while (!(NVMCTRL->STATUS.bit.READY))
                ;
            NVMCTRL->INTFLAG.bit.DONE = 1; // Clear flag after command execution
            nd::utils::Logger::getLogger().debug("Page Buffer cleared.");

            // Step 5: Write first 4 words using WQW
            nd::utils::Logger::getLogger().debug("Writing first 4 words back to User Row:");
            for (int i = 0; i < 4; i++)
            {
                *(user_row_ptr + i) = user_row_data[i];
                nd::utils::Logger::getLogger().debug("Loaded USER ROW word {} = 0x{:08X} into buffer.", i, user_row_data[i]);
            }
            nd::utils::Logger::getLogger().debug("Modified USER ROW data (first 4 words) loaded into buffer:");
            for (int i = 0; i < 4; i++)
            {
                nd::utils::Logger::getLogger().debug("user_row_data[{}] = 0x{:08X}", i, user_row_data[i]);
            }

            nd::utils::Logger::getLogger().debug("Performing Quad-Word Write (WQW) for first 4 words...");
            NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_WQW | NVMCTRL_CTRLB_CMDEX_KEY;
            while (!(NVMCTRL->STATUS.bit.READY))
                ;
            NVMCTRL->INTFLAG.bit.DONE = 1;
            nd::utils::Logger::getLogger().debug("First 4 words written successfully.");

            // Step 6: Write 5th to 8th word separately (must write a full 4-word block)
            nd::utils::Logger::getLogger().debug("Writing 5th to 8th word back to User Row...");
            for (int i = 4; i < 8; i++)
            {
                *(user_row_ptr + i) = user_row_data[i];
                nd::utils::Logger::getLogger().debug("Loaded extra USER ROW word {} = 0x{:08X} into buffer.", i, user_row_data[i]);
            }
            nd::utils::Logger::getLogger().debug("Modified USER ROW data (last 4 words) loaded into buffer:");
            for (int i = 4; i < 8; i++)
            {
                nd::utils::Logger::getLogger().debug("user_row_data[{}] = 0x{:08X}", i, user_row_data[i]);
            }

            nd::utils::Logger::getLogger().debug("Performing Quad-Word Write (WQW) for 5th word block...");
            NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_WQW | NVMCTRL_CTRLB_CMDEX_KEY;
            while (!(NVMCTRL->STATUS.bit.READY))
                ;
            NVMCTRL->INTFLAG.bit.DONE = 1;
            nd::utils::Logger::getLogger().debug("Last 4 words written successfully.");

            // Step 7: Ensure changes take effect after reset
            nd::utils::Logger::getLogger().debug("RAMECC Configuration Completed. Resetting...");
            __enable_irq();
            NVIC_SystemReset();
        }
    }

    void flash_page_read(uint32_t *flash_page_addr, std::array<uint32_t, WORDS_IN_PAGE> &buffer)
    {
        nd::utils::Logger::getLogger().debug("Reading Flash Page...");
        for (uint32_t i = 0; i < WORDS_IN_PAGE; i++)
        {
            buffer[i] = *(flash_page_addr + i);
            nd::utils::Logger::getLogger().debug("Read Flash word {} = 0x{:08X}", i, buffer[i]);
        }
        nd::utils::Logger::getLogger().debug("Flash Page Read Complete.");
    }

    void flash_page_modify(std::array<uint32_t, WORDS_IN_PAGE> &buffer, uint32_t page_position_offset, uint32_t word)
    {
        if (page_position_offset < WORDS_IN_PAGE)
        {
            buffer[page_position_offset] = word;
            nd::utils::Logger::getLogger().debug("Modified Flash word at position {}: 0x{:08X}", page_position_offset, word);
        }
        else
        {
            nd::utils::Logger::getLogger().error("Invalid position {}. Must be between 0 and {}", page_position_offset, WORDS_IN_PAGE - 1);
        }
    }

    void flash_page_erase(uint32_t *flash_page_addr)
    {
        nd::utils::Logger::getLogger().debug("Erasing Flash page...");
        NVMCTRL->ADDR.reg = (uint32_t)(flash_page_addr);
        NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_EP | NVMCTRL_CTRLB_CMDEX_KEY;
        while (!(NVMCTRL->STATUS.bit.READY))
            ;
        NVMCTRL->INTFLAG.bit.DONE = 1;

        nd::utils::Logger::getLogger().debug("Flash page erased.");
    }

    void flash_page_write(uint32_t *flash_page_addr, uint32_t page_position_offset, uint32_t *word)
    {
        __disable_irq();
        nd::utils::Logger::getLogger().debug("Starting Flash Page Write...");

        // Step 1: Read the current Flash page data into an array
        std::array<uint32_t, WORDS_IN_PAGE> flash_page_stored_data;

        flash_page_read(flash_page_addr, flash_page_stored_data); // Read function

        // Step 2: Modify the specific word in the Flash page at the given offset
        std::array<uint32_t, WORDS_IN_PAGE> flash_page_data_new;
        flash_page_modify(flash_page_data_new, page_position_offset, *word);

        // Step 3: Wait for NVMCTRL to be ready
        while (!(NVMCTRL->STATUS.bit.READY))
            ;
        nd::utils::Logger::getLogger().debug("NVMCTRL is ready for the next operation.");

        // Step 4: Erase the Flash page before writing
        flash_page_erase(flash_page_addr);

        // Step 5: Write in 128-bit (16-byte) chunks
        nd::utils::Logger::getLogger().debug("Writing Flash page in 128-bit chunks...");

        for (uint32_t i = 0; i < WORDS_IN_PAGE; i += WORDS_PER_BUFFER)
        {
            // Load 128-bit chunk into buffer
            for (uint32_t j = 0; j < WORDS_PER_BUFFER; j++)
            {
                flash_page_addr[i + j] = flash_page_data_new[i + j];
            }

            while (!(NVMCTRL->STATUS.bit.READY))
                ;

            // Issue Write Page (WP) command after filling the buffer
            nd::utils::Logger::getLogger().debug("Writing 128-bit chunk at offset {}...", i);
            NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_WP | NVMCTRL_CTRLB_CMDEX_KEY;
            while (!(NVMCTRL->STATUS.bit.READY))
                ;
            NVMCTRL->INTFLAG.bit.DONE = 1;
        }

        nd::utils::Logger::getLogger().debug("Flash page programming complete.");
        __enable_irq();
    }
}
