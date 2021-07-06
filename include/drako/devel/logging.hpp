#pragma once
#ifndef DRAKO_DEBUG_LOGGING_HPP
#define DRAKO_DEBUG_LOGGING_HPP

/// @brief   Lightweight runtime logging framework.
/// @author  Grassi Edoardo

#include "drako/core/compiler.hpp"
#include "drako/core/platform.hpp"

// TODO: remove header dependency or move implementation
#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
/* vvv include after vvv */
#include <debugapi.h>
#endif

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string_view>

namespace drako
{
    enum class log_msg_priority
    {
        info,
        warning,
        error,
        failure
    };

    [[nodiscard]] DRAKO_FORCE_INLINE constexpr const char* to_string(log_msg_priority prio) noexcept
    {
        switch (prio)
        {
            case log_msg_priority::info:
                return "INFO";
            case log_msg_priority::warning:
                return "WARNING";
            case log_msg_priority::error:
                return "ERROR";
            case log_msg_priority::failure:
                return "FAILURE";
            default:
                return "UNKNOWN_MSG_PRIORITY";
        }
    }

    enum class log_msg_channel : std::uint8_t;

    class debug_logger
    {
    public:
        constexpr explicit debug_logger(const char* pFileName) noexcept;

        constexpr static void log_message(log_msg_channel chn, const char* msg) noexcept;

    private:
        static debug_logger _instance;
    };

    DRAKO_FORCE_INLINE void log(const char* msg, log_msg_priority prio, const char* file, int line) noexcept
    {
#if defined(DRAKO_PLT_WIN32)
        char buffer[1024];
        std::snprintf(buffer, std::size(buffer), "[DRAKO] [%10s] %s : %d : %s\n", to_string(prio), file, line, msg);
        ::OutputDebugStringA(buffer);

#else
        std::cout << file << ":" << line << ":" << msg << std::endl;

#endif
    }

    DRAKO_FORCE_INLINE void log(const std::string& msg_, log_msg_priority prio_, const char* file_, int line_) noexcept
    {
        log(msg_.data(), prio_, file_, line_);
    }

#if !defined(DRAKO_API_NOLOGGING)

#define DRAKO_LOG_INFO(msg) ::drako::log((msg), ::drako::log_msg_priority::info, __FILE__, __LINE__)

#define DRAKO_LOG_WARNING(msg) ::drako::log((msg), ::drako::log_msg_priority::warning, __FILE__, __LINE__)

#define DRAKO_LOG_FAILURE(msg) ::drako::log((msg), ::drako::log_msg_priority::failure, __FILE__, __LINE__)

#define DRAKO_LOG_ERROR(msg) ::drako::log((msg), ::drako::log_msg_priority::error, __FILE__, __LINE__)

#else

#define DRAKO_LOG_INFO(...)
#define DRAKO_LOG_WARNING(...)
#define DRAKO_LOG_FAILURE(...)
#define DRAKO_LOG_ERROR(...)

#endif

    // log error and exit program
    [[noreturn]] DRAKO_FORCE_INLINE void log_and_exit(const std::string& msg)
    {
        DRAKO_LOG_ERROR(msg);
        std::exit(EXIT_FAILURE);
    }

} // namespace drako

#endif // !DRAKO_DEBUG_LOGGING_HPP