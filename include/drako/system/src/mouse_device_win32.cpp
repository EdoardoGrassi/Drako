#include "drako/system/mouse_device.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"

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
    void mouse_device::poll_state() noexcept
    {
    }

    std::vector<mouse_event> mouse_device::poll_events() noexcept
    {
        // TODO: add wheel events
        std::vector<mouse_event> events;
        // std::vector<mouse_wheel_event> wheel_events;

        for (MSG msg = {}; ::PeekMessageW(&msg, _context.handle_win32(),
                 WM_MOUSEFIRST, WM_MOUSELAST,
                 PM_REMOVE | PM_QS_INPUT);)
        {
            auto x = GET_X_LPARAM(msg.lParam);
            auto y = GET_Y_LPARAM(msg.lParam);

            const ivec2                     position(x, y);
            const std::chrono::milliseconds timestamp{ msg.time };
            mouse_action                    action;

            switch (msg.message)
            {
                case WM_LBUTTONDOWN:
                    action = mouse_action::left_btn_press;
                    break;
                case WM_LBUTTONUP:
                    action = mouse_action::left_btn_release;
                    break;
                case WM_LBUTTONDBLCLK:
                    action = mouse_action::left_btn_double;
                    break;

                case WM_RBUTTONDOWN:
                    action = mouse_action::right_btn_press;
                    break;
                case WM_RBUTTONUP:
                    action = mouse_action::right_btn_release;
                    break;
                case WM_RBUTTONDBLCLK:
                    action = mouse_action::right_btn_double;
                    break;

                case WM_MBUTTONDOWN:
                    action = mouse_action::mid_btn_press;
                    break;
                case WM_MBUTTONUP:
                    action = mouse_action::mid_btn_release;
                    break;
                case WM_MBUTTONDBLCLK:
                    action = mouse_action::mid_btn_double;
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