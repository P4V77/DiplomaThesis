#include "can_same5x.hpp"

#include <algorithm>
#include <span>

#include "utils/logging/Logger.hpp"

namespace
{
    struct alignas(4) CanMramRxbe
    {
        CAN_RXBE_0_Type RXBE_0{};
        CAN_RXBE_1_Type RXBE_1{};
        CAN_RXBE_DATA_Type RXBE_DATA[16]{};
    };

    struct alignas(4) CanMramRxf0e
    {
        CAN_RXF0E_0_Type RXF0E_0{};
        CAN_RXF0E_1_Type RXF0E_1{};
        CAN_RXF0E_DATA_Type RXF0E_DATA[16]{};
    };

    struct alignas(4) CanMramRxf1e
    {
        CAN_RXF1E_0_Type RXF1E_0{};
        CAN_RXF1E_1_Type RXF1E_1{};
        CAN_RXF1E_DATA_Type RXF1E_DATA[16]{};
    };

    struct alignas(4) CanMramSidfe
    {
        CAN_SIDFE_0_Type SIDFE_0{};
    };

    struct alignas(4) CanMramTxbe
    {
        CAN_TXBE_0_Type TXBE_0{};
        CAN_TXBE_1_Type TXBE_1{};
        CAN_TXBE_DATA_Type TXBE_DATA[16]{};
    };

    struct alignas(4) CanMramTxefe
    {
        CAN_TXEFE_0_Type TXEFE_0{};
        CAN_TXEFE_1_Type TXEFE_1{};
    };

    struct alignas(4) CanMramXifde
    {
        CAN_XIDFE_0_Type XIDFE_0{};
        CAN_XIDFE_1_Type XIDFE_1{};
    };

    using std_filter_arr_t = std::array<CanMramSidfe, 1>;
    using ext_filter_arr_t = std::array<CanMramXifde, 0>;
    using rx_fifo_arr_t = std::array<CanMramRxbe, 16>;
    using tx_fifo_arr_t = std::array<CanMramTxbe, 16>;
    using tx_event_fifo_arr_t = std::array<CanMramTxefe, 0>;

    __attribute__((section(".canvars"))) std_filter_arr_t can_rx_standard_filters{};
    __attribute__((section(".canvars"))) ext_filter_arr_t can_rx_extended_filters{};
    __attribute__((section(".canvars"))) rx_fifo_arr_t can_rx_fifo{};
    __attribute__((section(".canvars"))) tx_fifo_arr_t can_tx_fifo{};
    __attribute__((section(".canvars"))) tx_event_fifo_arr_t can_tx_event_fifo{};
}

namespace nd::drivers
{
    void CAN_ATSAM_C_E::init()
    {
        init_pins();
        init_clock(2);
        start_init();
        enable_CAN_FD();

        // Transmit pause, CAN FD
        can.CCCR.reg |= CAN_CCCR_TXP;

        // CAN FD bit timing
        can.DBTP.reg = 0;

        //
        // http://www.bittiming.can-wiki.info/
        //
        // CANopen
        //      Sample point at 87.5%
        //      SJW=1
        //
        // 500kb/s
        //      prescaler = 2
        //      tq = 1/(20/1)
        //      tsync = 1
        //      tprop + tseg1 = 34 (max. 127)
        //      tseg2 = 5 (max. 127)

        // CAN bit timing
        can.NBTP.reg = CAN_NBTP_NBRP(1 - 1) | // Nominal Baud Rate Prescaler (0x0 - 0x1FF) -> bit time quantum
                       CAN_NBTP_NTSEG1(34 - 1) |
                       // Nominal Time segment before sample point (0x - 0x7F) - tprop + tseg1
                       CAN_NBTP_NTSEG2(5 - 1) | // Time segment after sample point (0x0 - 0x7F) - tseg2
                       CAN_NBTP_NSJW(1 - 1);    // Nominal (Re)Synchronization Jump Width (0x0 - 0x7F)

        // Global Filter Configuration
        can.GFC.reg = 0;

        // Standard ID Filter Configuration
        can.SIDFC.reg = CAN_SIDFC_LSS(can_rx_standard_filters.size()) |
                        CAN_SIDFC_FLSSA(reinterpret_cast<std::uint32_t>(&can_rx_standard_filters));

        // Extended ID Filter Configuration
        // No filters for now
        can.XIDFC.reg = CAN_XIDFC_LSE(can_rx_extended_filters.size()) |
                        CAN_XIDFC_FLESA(reinterpret_cast<std::uint32_t>(&can_rx_extended_filters));

        // Filter[0] - store to FIFO0, accept all
        can_rx_standard_filters[0].SIDFE_0.reg = CAN_SIDFE_0_SFT_CLASSIC |
                                                 CAN_SIDFE_0_SFEC_STF0M |
                                                 CAN_SIDFE_0_SFID1(0) | // filter
                                                 CAN_SIDFE_0_SFID2(0);  // mask

        // Tx Buffer Configuration (FIFO)
        can.TXBC.reg = CAN_TXBC_TFQS(can_tx_fifo.size()) |
                       CAN_TXBC_NDTB(0) | // 0 TX buffer
                       CAN_TXBC_TBSA(reinterpret_cast<std::uint32_t>(&can_tx_fifo));

        // Tx Buffer Element Size Configuration
        // 64 data bytes in TX buffers
        can.TXESC.reg = CAN_TXESC_TBDS_DATA64;

        // Tx Event FIFO Configuration
        can.TXEFS.reg = CAN_TXEFC_EFS(can_tx_event_fifo.size()) |
                        CAN_TXEFC_EFSA(reinterpret_cast<std::uint32_t>(&can_tx_event_fifo));

        // Rx FIFO Configuration
        // 64 data bytes in RX buffers and FIFOs
        can.RXESC.reg = CAN_RXESC_RBDS_DATA64 | CAN_RXESC_F1DS_DATA64 | CAN_RXESC_F0DS_DATA64;
        can.RXF0C.reg = CAN_RXF0C_F0S(can_rx_fifo.size()) |
                        CAN_RXF0C_F0SA(reinterpret_cast<std::uint32_t>(&can_rx_fifo));

        // Interrupts - all on line 0
        can.ILS.reg = 0;

        // Enable interrupt line 0
        // can.ILE.reg = CAN_ILE_EINT0;

        // Enable RX FIFO0 new message interrupt
        // can.IE.reg = CAN_IE_RF0NE;

        // enable_NVIC_irq();
        stop_init();
    }

    void CAN_ATSAM_C_E::go_to_sleep()
    {
        // Go to IDLE state
        can.CCCR.bit.CSR = 1;
        while (!can.CCCR.bit.CSA || !can.CCCR.bit.INIT)
            ;
    }

    void CAN_ATSAM_C_E::wakeup_from_sleep()
    {
        // Go to NORMAL mode
        can.CCCR.bit.CSR = 0;
        while (can.CCCR.bit.CSA)
            ;

        stop_init();
    }

    void CAN_ATSAM_C_E::send_test_msg() const
    {
        can_tx_fifo[0].TXBE_0.bit.ID = 2 << 18u;
        can_tx_fifo[0].TXBE_1.bit.DLC = 8;
        can_tx_fifo[0].TXBE_DATA[0].bit.DB0 = 0xDE;
        can_tx_fifo[0].TXBE_DATA[0].bit.DB1 = 0xAD;
        can_tx_fifo[0].TXBE_DATA[0].bit.DB2 = 0xBE;
        can_tx_fifo[0].TXBE_DATA[0].bit.DB3 = 0xEF;
        can_tx_fifo[0].TXBE_DATA[1].bit.DB0 = 0xBA;
        can_tx_fifo[0].TXBE_DATA[1].bit.DB1 = 0xDD;
        can_tx_fifo[0].TXBE_DATA[1].bit.DB2 = 0xCA;
        can_tx_fifo[0].TXBE_DATA[1].bit.DB3 = 0xFE;

        // Request to send buffer 0
        can.TXBAR.reg = CAN_TXBAR_AR0;

        // Wait for transmission
        // while (can.TXBRP.bit.TRP0);
    }

    void CAN_ATSAM_C_E::receive_test() const
    {
        auto &msg = can_rx_fifo;
        const std::uint8_t data_index = can.RXF0S.bit.F0GI;
        nd::utils::Logger::getLogger().debug("CAN message\n  ID={}\n  FD={}\n  DLC={}\n\n", msg[data_index].RXBE_0.bit.ID >> 18,
                                             static_cast<bool>(msg[data_index].RXBE_1.bit.FDF),
                                             static_cast<std::uint8_t>(msg[data_index].RXBE_1.bit.DLC));

        // Increment FIFO
        can.RXF0A.reg = CAN_RXF0A_F0AI(data_index);
    }

    bool CAN_ATSAM_C_E::is_new_message_received()
    {
        return can.RXF0S.bit.F0PI != can.RXF0S.bit.F0GI;
    }

    void CAN_ATSAM_C_E::send(const CAN_message &msg)
    {
        if (can.TXFQS.bit.TFQF)
        {
            utils::Logger::getLogger().error("No free space in TX FIFO, cannot send message!");
            
            can.TXBCR.reg = 0xFFFFFFFF; // Should reset message requests
            utils::Logger::getLogger().error("TX FIFO reset");
            return;
        }

        const std::uint8_t data_index = can.TXFQS.bit.TFQPI;
        auto &driver_msg = can_tx_fifo[data_index];

        driver_msg.TXBE_0.bit.ESI = false;
        driver_msg.TXBE_0.bit.RTR = false;
        driver_msg.TXBE_1.bit.MM = 0;
        driver_msg.TXBE_1.bit.EFC = false;

        if (msg.is_extended_id)
        {
            driver_msg.TXBE_0.bit.XTD = 1;
            driver_msg.TXBE_0.bit.ID = msg.id & 0x1F'FF'FF'FF;
        }
        else
        {
            driver_msg.TXBE_0.bit.XTD = 0;
            driver_msg.TXBE_0.bit.ID = (msg.id << 18) & 0x1F'FF'FF'FF;
        }

        if (msg.is_fd)
        {
            driver_msg.TXBE_1.bit.FDF = 1;
            driver_msg.TXBE_1.bit.BRS = 0;
        }
        else
        {
            driver_msg.TXBE_1.bit.FDF = 0;
        }

        driver_msg.TXBE_1.bit.DLC = size_to_DLC(msg.size) & 0xF;
        const auto data_len_dlc{DLC_to_size(driver_msg.TXBE_1.bit.DLC)};
        const auto driver_data = std::as_writable_bytes(std::span{driver_msg.TXBE_DATA});
        const auto msg_data = std::as_bytes(std::span{msg.data.begin(), data_len_dlc});
        std::copy(msg_data.begin(), msg_data.end(), driver_data.begin());

        // Request transmission (and increment FIFO)
        can.TXBAR.reg = (1u << data_index);
    }

    void CAN_ATSAM_C_E::receive(CAN_message &msg)
    {
        if (!is_new_message_received())
            return;

        const std::uint8_t data_index = can.RXF0S.bit.F0GI;
        auto &driver_msg = can_rx_fifo[data_index];

        if (driver_msg.RXBE_0.bit.XTD)
        {
            msg.is_extended_id = true;
            msg.id = driver_msg.RXBE_0.bit.ID;
        }
        else
        {
            msg.is_extended_id = false;
            msg.id = driver_msg.RXBE_0.bit.ID >> 18;
        }

        if (driver_msg.RXBE_1.bit.FDF)
        {
            msg.is_fd = true;
        }
        else
        {
            msg.is_fd = false;
        }

        msg.size = DLC_to_size(driver_msg.RXBE_1.bit.DLC);
        const std::uint32_t driver_data_len = (msg.size / sizeof(driver_msg.RXBE_DATA[0])) + ((msg.size % sizeof(driver_msg.RXBE_DATA[0])) ? 1 : 0);
        const auto driver_data = std::as_bytes(std::span{driver_msg.RXBE_DATA, driver_data_len});
        const auto msg_data = std::as_writable_bytes(std::span{msg.data.begin(), msg.size});
        std::copy(driver_data.begin(), driver_data.end(), msg_data.begin());

        // Increment FIFO
        can.RXF0A.reg = CAN_RXF0A_F0AI(data_index);
    }
}
