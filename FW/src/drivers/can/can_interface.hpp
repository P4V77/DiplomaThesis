#pragma once

#include <cstdint>
#include <array>
#include <span>
#include <atomic>
#include <fmt/format.h>
#include <fmt/ranges.h>



namespace nd::drivers
{
    enum class CAN_activity
    {
        SYNC,
        IDLE,
        RX,
        TX,
    };

    enum class CAN_states
    {
        ERROR_ACTIVE,
        ERROR_PASSIVE,
        BUSS_OFF,
    };

    struct CAN_message
    {
        std::uint32_t id{};
        std::uint8_t size{};
        std::array<std::uint8_t, 64> data{};
        bool is_extended_id{false};
        bool is_fd{false};
    };

    class CAN_interface
    {
        public:
            virtual void init() = 0;
            virtual void send(const CAN_message& msg) = 0;
            virtual void receive(CAN_message& msg) = 0;
            [[nodiscard]] virtual bool is_new_message_received() = 0;
            virtual void go_to_sleep() = 0;
            virtual void wakeup_from_sleep() = 0;
    };

    constexpr std::uint8_t size_to_DLC(const std::uint8_t size)
    {
        if (size <= 8)
            return size;
        else if (size <= 12)
            return 9;
        else if (size <= 16)
            return 10;
        else if (size <= 20)
            return 11;
        else if (size <= 24)
            return 12;
        else if (size <= 32)
            return 13;
        else if (size <= 48)
            return 14;
        else if (size <= 64)
            return 15;
        else
            return 0;
    }

    constexpr std::uint8_t DLC_to_size(const std::uint8_t can_size)
    {
        if (can_size <= 8)
            return can_size;

        switch (can_size)
        {
            case 9:
                return 12;
            case 10:
                return 16;
            case 11:
                return 20;
            case 12:
                return 24;
            case 13:
                return 32;
            case 14:
                return 48;
            case 15:
                return 64;
            default:
                return 0;
        }
    }
}

template<>
struct fmt::formatter<nd::drivers::CAN_message>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(nd::drivers::CAN_message const& msg, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "CAN message: ID={:03x} EXT={} FD={} Size={} Data={::02x}",
            msg.id, msg.is_extended_id, msg.is_fd, msg.size, std::span{msg.data.begin(), msg.size});
    }
};
