#include "drako/input/mouse_device.hpp"

#if !defined(_WIN32)
#error This source file should be included only on Windows builds
#endif

#if defined(_WIN32)
// #include <WinUser.h>
#include <Windows.h>
/* vvv include after vvv */
#include <windowsx.h> // GET_*_LPARAM()
#endif

#include <cassert>
#include <chrono>
#include <vector>

namespace drako
{
    [[nodiscard]] MouseAction _decode_action_win32(const UINT tag)
    {
        assert(tag >= WM_LBUTTONDOWN);
        assert(tag <= WM_MBUTTONDBLCLK);
        switch (tag)
        {
            case WM_LBUTTONDOWN: return MouseAction::left_btn_press;
            case WM_LBUTTONUP: return MouseAction::left_btn_release;
            case WM_LBUTTONDBLCLK: return MouseAction::left_btn_double;
            case WM_RBUTTONDOWN: return MouseAction::right_btn_press;
            case WM_RBUTTONUP: return MouseAction::right_btn_release;
            case WM_RBUTTONDBLCLK: return MouseAction::right_btn_double;
            case WM_MBUTTONDOWN: return MouseAction::mid_btn_press;
            case WM_MBUTTONUP: return MouseAction::mid_btn_release;
            case WM_MBUTTONDBLCLK: return MouseAction::mid_btn_double;
        }
        throw std::invalid_argument{ "unexpected value" };
    }

    [[nodiscard]] UINT _encode_action_win32(const MouseAction a)
    {
        switch (a)
        {
            case MouseAction::left_btn_press: return WM_LBUTTONDOWN;
            case MouseAction::left_btn_release: return WM_LBUTTONUP;
            case MouseAction::left_btn_double: return WM_LBUTTONDBLCLK;
            case MouseAction::right_btn_press: return WM_RBUTTONDOWN;
            case MouseAction::right_btn_release: return WM_RBUTTONUP;
            case MouseAction::right_btn_double: return WM_RBUTTONDBLCLK;
            case MouseAction::mid_btn_press: return WM_MBUTTONDOWN;
            case MouseAction::mid_btn_release: return WM_MBUTTONUP;
            case MouseAction::mid_btn_double: return WM_MBUTTONDBLCLK;
        }
        throw std::invalid_argument{ "unexpected value" };
    }

    [[nodiscard]] std::chrono::milliseconds _decode_time_win32(const MSG& m) noexcept
    {
        return std::chrono::milliseconds{ m.time };
    }

    [[nodiscard]] DWORD _encode_time_win32(const std::chrono::milliseconds m) noexcept
    {
        return m.count();
    }

    [[nodiscard]] DWORD _encode_delta_win32(const MouseWheelEvent::Delta delta) noexcept
    {
        return delta;
    }

    [[nodiscard]] MouseWheelEvent::Delta _decode_delta_win32(const DWORD delta) noexcept
    {
        return delta;
    }


    void MouseDevice::poll_state() {}

    MouseDevice::EventJournal MouseDevice::poll_events()
    {
        EventJournal j;

        for (MSG msg = {}; ::PeekMessageW(&msg, NULL, //_context.handle_win32(),
                 WM_MOUSEFIRST, WM_MOUSELAST,
                 PM_REMOVE | PM_QS_INPUT);)
        {
            auto x = GET_X_LPARAM(msg.lParam);
            auto y = GET_Y_LPARAM(msg.lParam);

            const MousePosition             pos(x, y);
            const std::chrono::milliseconds time = _decode_time_win32(msg);

            if (WM_LBUTTONDOWN <= msg.message && msg.message <= WM_MBUTTONDBLCLK)
            {
                const auto action = _decode_action_win32(msg.message);
                j.press.emplace_back(time, pos, action);
            }
            else if (msg.message == WM_MOUSEWHEEL)
            {
                const auto delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
                j.wheel.emplace_back(time, pos, delta);
            }
            else
            {
                ::DefWindowProcW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
            }
        }
        return j;
    }

    void MouseDevice::syntetize(const MousePressEvent& e)
    {
        INPUT input{
            .type = INPUT_MOUSE,
            .mi   = {
                .dx        = static_cast<LONG>(e.position.x),
                .dy        = static_cast<LONG>(e.position.y),
                .mouseData = 0,
                .dwFlags   = MOUSEEVENTF_ABSOLUTE | _encode_action_win32(e.action),
                .time      = _encode_time_win32(e.timestamp),
            }
        };
        if (::SendInput(1, &input, sizeof(INPUT)) != 1)
            throw std::system_error(::GetLastError(), std::system_category());
    }

    void MouseDevice::syntetize(const MouseWheelEvent& e)
    {
        INPUT input{
            .type = INPUT_MOUSE,
            .mi   = {
                .dx        = static_cast<LONG>(e.position.x),
                .dy        = static_cast<LONG>(e.position.y),
                .mouseData = _encode_delta_win32(e.delta),
                .dwFlags   = MOUSEEVENTF_WHEEL,
                .time      = _encode_time_win32(e.timestamp),
            }
        };
        if (::SendInput(1, &input, sizeof(INPUT)) != 1)
            throw std::system_error(::GetLastError(), std::system_category());
    }

} // namespace drako