#include "drako/system/desktop_window.hpp"
#include "drako/system/keyboard_device.hpp"

#include <chrono>
#include <ostream>
#include <string>
#include <thread>

int main()
{
    drako::sys::UniqueDesktopWindow      main(L"Keyboard Test App");
    const std::chrono::milliseconds timeout{ 100 };
    drako::sys::Keyboard     keyboard{ main };

    while (true)
    {
        std::this_thread::sleep_for(timeout);

        const auto events = keyboard.poll_events();
        for (auto& e : events)
        {
            std::cout << "action:\t" << to_string(e.action) << '\n'
                      << "timestamp:\t" << std::to_string(e.timestamp.count()) << '\n'
                      << "scan code:\t" << to_string(e.scan_code) << '\n'
                      << "virtual code:\t" << to_string(e.virt_code) << '\n';
        }
        std::cout.flush();
    }
}