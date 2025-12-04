#pragma once

#include <array>
#include <string_view>

#include "fmt/format.h"
#include "chrono/chrono.hpp"

namespace nd::utils
{
    class Logger
    {
    public:
        static Logger &getLogger()
        {
            if (logger == nullptr)
            {
                static Logger logger_static;
                logger = &logger_static;
            }
            return *logger;
        }

        static void setLogger(Logger &project_logger)
        {
            logger = &project_logger;
        }

        void set_levels(const bool level_error_p, const bool level_warn_p, const bool level_info_p,
                        const bool level_debug_p, const bool level_can_p, const bool level_canopen_p)
        {
            level_error = level_error_p;
            level_warn = level_warn_p;
            level_info = level_info_p;
            level_debug = level_debug_p;
            level_can = level_can_p;
            level_canopen = level_canopen_p;
        }

        template <typename... T>
        void info(fmt::format_string<T...> format, T &&...args)
        {
            if (level_info)
                log_message("info", format, fmt::make_format_args(args...));
        }

        template <typename... T>
        void warn(fmt::format_string<T...> format, T &&...args)
        {
            if (level_warn)
                log_message("warn", format, fmt::make_format_args(args...));
        }

        template <typename... T>
        void error(fmt::format_string<T...> format, T &&...args)
        {
            if (level_error)
                log_message("error", format, fmt::make_format_args(args...));
        }

        template <typename... T>
        void debug(fmt::format_string<T...> format, T &&...args)
        {
            if (level_debug)
                log_message("debug", format, fmt::make_format_args(args...));
        }

        template <typename... T>
        void can(fmt::format_string<T...> format, T &&...args)
        {
            if (level_can)
                log_message("CAN", format, fmt::make_format_args(args...));
        }

        template <typename... T>
        void canopen(fmt::format_string<T...> format, T &&...args)
        {
            if (level_canopen)
                log_message("CANOP", format, fmt::make_format_args(args...));
        }

    protected:
        using buff_t = std::array<std::uint8_t, 1024>;
        buff_t buff{};

    private:
        inline static Logger *logger{nullptr};

        bool level_error{true};
        bool level_warn{true};
        bool level_info{true};
        bool level_debug{true};
        bool level_can{true};
        bool level_canopen{true};

        void log_message(const std::string_view type, fmt::string_view format, fmt::format_args args)
        {
            const auto t = chrono::steady_clock::now().time_since_epoch().count();
            mutex_lock();
            auto [iterator1, count1] = fmt::format_to_n(buff.begin(), buff.size(), "[{:9d}.{:03d}][{:^5}] ", t / 1'000, t % 1000, type);
            auto [iterator2, count2] = fmt::vformat_to_n(buff.begin() + count1, buff.size() - count1, format, args);
            auto [iterator3, count3] = fmt::format_to_n(buff.begin() + count1 + count2, buff.size() - count1 - count2, "\n");

            log(buff, count1 + count2 + count3);
            mutex_unlock();
        }

        virtual void log(buff_t &, const std::uint_fast32_t)
        {
            // By default - do nothing
        }

        virtual void mutex_lock()
        {
            // By default - do nothing
        }

        virtual void mutex_unlock()
        {
            // By default - do nothing
        }
    };
}
