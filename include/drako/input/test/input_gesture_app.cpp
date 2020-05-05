#pragma once
#include "drako/input/gesture_system.hpp"
#include "drako/input/input_system.hpp"
//#include "drako/system/timer.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace drako::engine;

void _timer_callback()
{
    std::cout << "Timer has fired!\n";
}

void _action_1() noexcept { std::cout << "[INPUT] Action 1!\n"; }
void _action_2() noexcept { std::cout << "[INPUT] Action 2!\n"; }
void _action_3() noexcept { std::cout << "[INPUT] Action 3!\n"; }

int main()
{
    input::input_system   input_system;
    input::gesture_system gesture_system;
    /*
    engine::input_device_layout controller;
    controller.input_axis = {
        { "gamepad/lx_stick", -1.f, 1.f },
        { "gamepad/ly_stick", -1.f, 1.f },
        { "gamepad/rx_stick", -1.f, 1.f },
        { "gamepad/ry_stick", -1.f, 1.f },
    };
    controller.input_buttons = {
        { "gamepad/north_button" },
        { "gamepad/south_button" },
        { "gamepad/west_button" },
        { "gamepad/east_button" },
        { "gamepad/dpad/up" },
        { "gamepad/dpad/down" },
        { "gamepad/dpad/left" },
        { "gamepad/dpad/right" }
    };
    */

    const std::vector<input::action> actions = {
        { gesture_system, "action_1", _action_1, input::interaction::button_press },
        { gesture_system, "action_2", _action_2, input::interaction::button_release },
        { gesture_system, "action_3", _action_3, input::interaction::button_timed_hold },
    };
    const std::vector<input::binding> bindings = {
        { "gamepad/buttons/north", actions[0].name },
        { "gamepad/buttons/south", actions[1].name },
        { "gamepad/buttons/west", actions[2].name },
        { "gamepad/buttons/east", actions[2].name }
    };

    for (const auto& action : actions)
        gesture_system.create(action);

    for (const auto& binding : bindings)
        gesture_system.create(binding);


    const auto input_devices = input::query_input_devices();
    if (std::empty(input_devices))
    {
        std::cout << "No devices found!\n";
        std::exit(EXIT_FAILURE);
    }
    else
    {
        for (const auto& device : input_devices)
        {
            std::cout << "Interface: " << device.native_interface << '\n'
                      << "Native ID: " << device.native_device_id << '\n';
        }
    }

    const auto gamepad = input_system.create_device(input_devices[0]);
    //input_system.insert_state_callback(gamepad, _input_callback);

    const std::chrono::milliseconds wait{ 5000 };
    for (;;)
    {
        std::cout << "Polling input...\n";
        input_system.update();

        gesture_system.update();
        std::this_thread::sleep_for(wait);
    }
}