#include "drako/system/keyboard_device.hpp"

#include <chrono>
#include <vector>

#include <minwindef.h>
#include <WinUser.h>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    keyboard_scan_code map(keyboard_virt_code virt_code) noexcept
    {
        auto scan_code = ::MapVirtualKeyW(static_cast<UINT>(virt_code), MAPVK_VK_TO_VSC);
        return (scan_code != 0) ? static_cast<keyboard_scan_code>(scan_code)
                                : keyboard_scan_code::undefined;
    }

    keyboard_virt_code map(keyboard_scan_code scan_code) noexcept
    {
        auto virt_code = ::MapVirtualKeyW(static_cast<UINT>(scan_code), MAPVK_VSC_TO_VK_EX);
        return (virt_code != 0) ? static_cast<keyboard_virt_code>(virt_code)
                                : keyboard_virt_code::undefined;
    }

    // TODO: end impl
    std::vector<keyboard_event> keyboard_device::poll_events() noexcept
    {
        std::vector<keyboard_event> events;

        for (MSG msg; ::PeekMessageW(&msg, NULL,
                 WM_KEYFIRST, WM_KEYLAST, PM_REMOVE | PM_QS_INPUT);) // deque keyboard related messages
        {
            const std::chrono::milliseconds timestamp{ msg.time };

            switch (msg.message)
            {
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYDOWN:

                case WM_SYSKEYUP:
                    break;

                default:
                    ::DefWindowProcW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                    continue;
            }
            ::TranslateMessage(&msg);
            events.emplace_back();
        }
        return events;
    }

} // namespace drako::sys