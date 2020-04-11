#pragma once
#include "drako/input/gesture_system.hpp"
#include "drako/input/input_system.hpp"

#include <chrono>
#include <thread>

using namespace drako;

int main()
{
    engine::input_system        input_system;
    engine::input_device_layout controller;

    const std::chrono::milliseconds wait{ 10 };
    for (;;)
    {
        input_system.update();

        std::this_thread::sleep_for(wait);
    }
}