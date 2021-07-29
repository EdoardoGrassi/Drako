#include "drako/input/mouse_device.hpp"
//#include "drako/system/desktop_window.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <thread>

int main()
{
    //drako::sys::UniqueDesktopWindow main(L"Main window");
    //main.show();

    drako::MouseDevice mouse{};
    //while (main.update())
    while (true)
    {
        const auto events = mouse.poll_events();
        for (const auto& e : events.press)
        {
            const auto [x, y] = e.position;
            std::cout << std::format("[MOUSE] position: ({},{}) time: {}\n",
                x, y, e.timestamp);
        }
        for (const auto& e : events.wheel)
        {
            const auto [x, y] = e.position;
            std::cout << std::format("[MOUSE] position: ({},{}) time: {} delta: {}\n",
                x, y, e.timestamp, e.delta);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}