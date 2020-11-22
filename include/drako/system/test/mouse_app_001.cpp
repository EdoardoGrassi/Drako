#include "drako/devel/logging.hpp"
#include "drako/system/desktop_window.hpp"
#include "drako/system/mouse_device.hpp"

#include <chrono>
#include <thread>

int main()
{
    drako::sys::desktop_window main(L"Main window");
    main.show();

    drako::sys::mouse_device mouse(main);

    while (main.update())
    {
        const auto events = mouse.poll_events();
        for (const auto& e : events)
        {
            DRAKO_LOG_INFO("[MOUSE]\tposition: " + to_string(e.cursor_local_position()) + "\ttimestamp: ");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}