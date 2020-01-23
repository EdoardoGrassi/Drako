#include <cstdlib>
#include <thread>

#include "drako/core/system/native_mouse_device.hpp"
#include "drako/devel/logging.hpp"

using namespace drako;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    sys::native_window main(hInstance, L"Main window");
    main.show();

    sys::native_mouse_device mouse(main);

    while (main.update())
    {
        const auto events = mouse.poll_events();
        for (const auto& e : events)
        {
            DRAKO_LOG_INFO("[MOUSE]\tposition: " + to_string(e.cursor_local_position()) + "\ttimestamp: ");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return EXIT_SUCCESS;
}