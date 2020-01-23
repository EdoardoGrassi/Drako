#include "drako/core/system/native_keyboard_device.hpp"

#include <chrono>
#include <ostream>
#include <thread>

int main()
{
    const std::chrono::milliseconds    timeout{ 100 };
    drako::sys::native_keyboard_device keyboard;

    while (true)
    {
        std::this_thread::sleep_for(timeout);

        const auto events = keyboard.poll_events();
        for (auto& e : events)
        {
            std::cout << e;
        }
        std::cout.flush();
    }
}