#pragma once
#ifndef DRAKO_DESKTOP_WINDOW_HPP
#define DRAKO_DESKTOP_WINDOW_HPP

#include "drako/core/platform.hpp"
#include "drako/devel/assertion.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <string_view>

namespace drako::sys
{
    // Handle of an os-specific native gui window.
    //
    class desktop_window
    {
    public:
#if defined(DRAKO_PLT_WIN32)
        using native_handle_type  = HWND;
        using native_context_type = HINSTANCE;
#endif

        explicit desktop_window(std::wstring_view title);
        ~desktop_window() noexcept;

        desktop_window(const desktop_window&) = delete;
        desktop_window& operator=(const desktop_window&) = delete;

        desktop_window(desktop_window&&) noexcept;
        desktop_window& operator=(desktop_window&&) noexcept;

        // Hides the window.
        void hide() noexcept;

        // Shows the window with its current settings.
        void show() noexcept;

        // Process pending events from the OS native system.
        bool update() noexcept;

        [[nodiscard]] native_handle_type native_handle() const noexcept;

        [[nodiscard]] native_context_type native_context() const noexcept;


#if defined(DRAKO_PLT_WIN32)

        [[nodiscard]] constexpr HWND handle_win32() const noexcept
        {
            return _window;
        }

        [[nodiscard]] constexpr HINSTANCE instance_win32() const noexcept
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