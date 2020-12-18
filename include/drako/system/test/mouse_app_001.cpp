#include "drako/system/desktop_window.hpp"
#include "drako/system/mouse_device.hpp"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    drako::sys::UniqueDesktopWindow main(L"Main window");
    main.show();

    drako::sys::MouseDevice mouse(main);

    while (main.update())
    {
        const auto events = mouse.poll_events();
        for (const auto& e : events)
        {
            std::cout << "[MOUSE]\tposition: " << e.cursor_local_position()
                      << "\ttimestamp: " << e.local_timestamp().count()<< '\n';
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}