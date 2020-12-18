#include "drako/system/desktop_window.hpp"

#include "drako/devel/logging.hpp"

#include <cassert>
#include <string>
#include <string_view>
#include <system_error>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

// #include <WinUser.h>
// #include <libloaderapi.h> // ::GetModuleHandleW()
#include <Windows.h>

namespace drako::sys
{
    LRESULT CALLBACK _window_message_handler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_SIZE:
            {
                //int width = LOWORD(lParam);
                //int height = HIWORD(lParam);

                break;
            }

            case WM_CLOSE:
            {
                if (::MessageBoxW(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
                {
                    ::DestroyWindow(hwnd);
                }
                break;
            }

            case WM_DESTROY:
            {
                ::PostQuitMessage(ERROR_SUCCESS);
                break;
            }

            default:
                return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
        return ERROR_SUCCESS;
    }

    UniqueDesktopWindow::UniqueDesktopWindow(std::wstring_view title)
        : _instance(::GetModuleHandleW(nullptr))
    {
        assert(!std::empty(title));

        const std::wstring_view CLASS_NAME = L"Drako Window Class";

        WNDCLASSEXW wc   = {};
        wc.cbSize        = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc   = _window_message_handler;
        wc.hInstance     = _instance;
        wc.lpszMenuName  = std::data(title);
        wc.lpszClassName = std::data(CLASS_NAME);

        ::RegisterClassExW(&wc);

        _window = ::CreateWindowExW(
            0,                     // optional window styles
            std::data(CLASS_NAME), // window class
            std::data(title),      // window title
            WS_OVERLAPPEDWINDOW,   // window style

            // window size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,      // parent window
            NULL,      // menu
            _instance, // instance handle
            nullptr    // additional application data
        );
        if (_window == NULL)
            [[unlikely]] throw std::system_error(::GetLastError(), std::system_category());
    }

    UniqueDesktopWindow::~UniqueDesktopWindow() noexcept
    {
        if (_window != NULL)
            if (!::DestroyWindow(_window))
                [[unlikely]] std::terminate();
    }

    UniqueDesktopWindow::UniqueDesktopWindow(UniqueDesktopWindow&& other) noexcept
        : _instance(other._instance), _window(other._window)
    {
        other._window = NULL;
    }

    UniqueDesktopWindow& UniqueDesktopWindow::operator=(UniqueDesktopWindow&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            _instance     = other._instance;
            _window       = other._window;
            other._window = NULL;
        }
        return *this;
    }

    void UniqueDesktopWindow::hide() noexcept
    {
        ::ShowWindow(_window, SW_HIDE);
    }

    void UniqueDesktopWindow::show() noexcept
    {
        ::ShowWindow(_window, SW_SHOW);
    }

    bool UniqueDesktopWindow::update() noexcept
    {
        bool has_close_msg;
        if (MSG msg; has_close_msg = ::PeekMessageW(&msg, _window, WM_CLOSE, WM_CLOSE, PM_REMOVE))
        {
            ::DefWindowProcW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        }
        return !has_close_msg;
    }
} // namespace drako::sys