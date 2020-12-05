#include "drako/system/keyboard_device.hpp"

#include "drako/core/platform.hpp"

#include <Windows.h>
// vvv/^^^ preserve inclusion order
#include <WinUser.h>

#include <chrono>
#include <vector>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    ScanCode map(const VirtCode vc) noexcept
    {
        // returns zero if there is no translation available
        const auto sc = ::MapVirtualKeyW(static_cast<UINT>(vc), MAPVK_VK_TO_VSC_EX);
        return (sc != 0) ? static_cast<ScanCode>(sc) : ScanCode::undefined;
    }

    VirtCode map(const ScanCode sc) noexcept
    {
        // returns zero if there is no translation available
        const auto vc = ::MapVirtualKeyW(static_cast<UINT>(sc), MAPVK_VSC_TO_VK_EX);
        return (vc != 0) ? static_cast<VirtCode>(vc) : VirtCode::undefined;
    }

    std::vector<Keyboard::Event> Keyboard::poll_events() noexcept
    {
        std::vector<Event> events;

        for (MSG msg; ::PeekMessageW(&msg, NULL,
                 WM_KEYFIRST, WM_KEYLAST, PM_REMOVE | PM_QS_INPUT);) // deque keyboard related messages
        {
            Event e{};
            e.timestamp = std::chrono::milliseconds{ msg.time };
            e.virt_code = static_cast<VirtCode>(msg.wParam);
            switch (msg.message)
            {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                    e.action = Event::Action::key_press;
                    break;

                case WM_KEYUP:
                case WM_SYSKEYUP:
                    e.action = Event::Action::key_release;
                    break;

                default:
                    ::DefWindowProcW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                    continue;
            }
            ::TranslateMessage(&msg);
            events.emplace_back(e);
        }
        return events;
    }

} // namespace drako::sys