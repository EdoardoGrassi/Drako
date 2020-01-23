#pragma once
#ifndef DRAKO_NATIVE_WINDOW_HPP
#define DRAKO_NATIVE_WINDOW_HPP

#include <string_view>

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

namespace drako::sys
{
    // Handle of an os-specific native gui window.
    //
    class native_window
    {
    public:
        explicit native_window(std::wstring_view title) noexcept;
        ~native_window() noexcept;

        native_window(const native_window&) = delete;
        native_window& operator=(const native_window&) = delete;

        constexpr native_window(native_window&&) noexcept;
        constexpr native_window& operator=(native_window&&) noexcept;

        // Hides the window.
        void hide() noexcept;

        // Shows the window with its current settings.
        void show() noexcept;

        // Process pending events from the OS native system.
        bool update() noexcept;


#if defined(DRAKO_PLT_WIN32)

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr HWND
        window_handle_win32() const noexcept
        {
            return _window;
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr HINSTANCE
        instance_handle_win32() const noexcept
        {
            return _instance;
        }

#endif

    private:
#if defined(DRAKO_PLT_WIN32)
        HINSTANCE _instance;
        HWND      _window;
#endif
    };
} // namespace drako::sys

#endif // !DRAKO_NATIVE_WINDOW_HPP