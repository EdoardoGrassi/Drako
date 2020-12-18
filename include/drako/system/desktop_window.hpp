#pragma once
#ifndef DRAKO_DESKTOP_WINDOW_HPP
#define DRAKO_DESKTOP_WINDOW_HPP

#include "drako/core/platform.hpp"
#include "drako/devel/assertion.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <string_view>
#include <type_traits>

namespace drako::sys
{
    /// @brief Handle of an os-specific native gui window.
    class UniqueDesktopWindow
    {
    public:
#if defined(DRAKO_PLT_WIN32)
        using native_handle_type  = HWND;
        using native_context_type = HINSTANCE;
#endif

        explicit UniqueDesktopWindow(std::wstring_view title);
        ~UniqueDesktopWindow() noexcept;

        UniqueDesktopWindow(const UniqueDesktopWindow&) = delete;
        UniqueDesktopWindow& operator=(const UniqueDesktopWindow&) = delete;

        UniqueDesktopWindow(UniqueDesktopWindow&&) noexcept;
        UniqueDesktopWindow& operator=(UniqueDesktopWindow&&) noexcept;

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
    static_assert(!std::is_copy_constructible_v<UniqueDesktopWindow>,
        DRAKO_STRINGIZE(UniqueDesktopWindow) " must be a movable-only type.");
    static_assert(!std::is_copy_assignable_v<UniqueDesktopWindow>,
        DRAKO_STRINGIZE(UniqueDesktopWindow) " must be a movable-only type.");
    static_assert(std::is_nothrow_move_constructible_v<UniqueDesktopWindow>,
        DRAKO_STRINGIZE(UniqueDesktopWindow) " must be nothrow movable.");
    static_assert(std::is_nothrow_move_assignable_v<UniqueDesktopWindow>,
        DRAKO_STRINGIZE(UniqueDesktopWindow) " must be nothrow movable.");

} // namespace drako::sys

#endif // !DRAKO_NATIVE_WINDOW_HPP