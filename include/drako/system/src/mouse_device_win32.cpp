#include "drako/system/mouse_device.hpp"

#include "drako/core/platform.hpp"

// #include <WinUser.h>
#include <Windows.h>
/* vvv include after vvv */
#include <windowsx.h> // GET_*_LPARAM()

#include <chrono>
#include <vector>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    void MouseDevice::poll_state() noexcept
    {
    }

    std::vector<MouseEvent> MouseDevice::poll_events() noexcept
    {
        // TODO: add wheel events
        std::vector<MouseEvent> events;
        // std::vector<mouse_wheel_event> wheel_events;

        for (MSG msg = {}; ::PeekMessageW(&msg, _context.handle_win32(),
                 WM_MOUSEFIRST, WM_MOUSELAST,
                 PM_REMOVE | PM_QS_INPUT);)
        {
            auto x = GET_X_LPARAM(msg.lParam);
            auto y = GET_Y_LPARAM(msg.lParam);

            const ivec2                     position(x, y);
            const std::chrono::milliseconds timestamp{ msg.time };
            MouseAction                     action;

            switch (msg.message)
            {
                case WM_LBUTTONDOWN:
                    action = MouseAction::left_btn_press;
                    break;
                case WM_LBUTTONUP:
                    action = MouseAction::left_btn_release;
                    break;
                case WM_LBUTTONDBLCLK:
                    action = MouseAction::left_btn_double;
                    break;

                case WM_RBUTTONDOWN:
                    action = MouseAction::right_btn_press;
                    break;
                case WM_RBUTTONUP:
                    action = MouseAction::right_btn_release;
                    break;
                case WM_RBUTTONDBLCLK:
                    action = MouseAction::right_btn_double;
                    break;

                case WM_MBUTTONDOWN:
                    action = MouseAction::mid_btn_press;
                    break;
                case WM_MBUTTONUP:
                    action = MouseAction::mid_btn_release;
                    break;
                case WM_MBUTTONDBLCLK:
                    action = MouseAction::mid_btn_double;
                    break;

                    /*case WM_MOUSEWHEEL:
                {
                    short delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
                    //wheel_events.emplace_back({});
                    break;
                }*/

                default:
                    ::DefWindowProcW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                    continue;
            }
            events.emplace_back(action, position, timestamp);
        }
        return events;
    }
} // namespace drako::sys