#include "drako/input/device_system.hpp"
#include "drako/input/input_system.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace drako;
using namespace std::chrono_literals;


void on_device_arrival(const DeviceChangeEvent& e) noexcept
{
    std::cout << "Connected device with id " << e.id << '\n';
}
void on_device_removal(const DeviceChangeEvent& e) noexcept
{
    std::cout << "Disconnected device with id " << e.id << '\n';
}

void jump1() noexcept { std::cout << "[INPUT] Jump 1!\n"; }
void jump2() noexcept { std::cout << "[INPUT] Jump 2!\n"; }
void fire1() noexcept { std::cout << "[INPUT] Fire 1!\n"; }
void fire2() noexcept { std::cout << "[INPUT] Fire 2!\n"; }

int main()
{
    DeviceSystem       ds{ { .devices_poll_rate = 5s } };
    input::InputSystemRuntime is;

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

    const std::vector<input::Gesture> presses = {
        { .name = "jump-pressed", .control = input::BooleanControlID{ 1 }, .event = input::EventID{ 1 } }
        //{ "fire-pressed", VKey::xbox_b, EventID{ 1 } }
    };
    for (const auto& gesture : presses)
        is.create_press_gesture(gesture);

    const std::vector<input::Gesture> releases = {
        { .name = "jump-released", .control = input::BooleanControlID{ 1 }, .event = input::EventID{ 2 } }
        //{ "fire-released", VKey::xbox_y }
    };
    for (const auto& gesture : releases)
        is.create_release_gesture(gesture);

    const std::vector<input::GamepadButtonBinding> bindings = {
        { .name = "a1", .id = {}, .button = GamepadButtonID{ 1 }, .control = input::BooleanControlID{ 0 } }
    };
    for (const auto& binding : bindings)
        is.create(binding);

    is.debug_print_on_press_table();
    is.debug_print_on_release_table();



    const std::vector<input::Action> actions = {
        { "fire-pressed", fire1, input::Action::ID{ 1 } },
        { "fire-released", fire2, input::Action::ID{ 2 } },
        { "jump-pressed", jump1, input::Action::ID{ 3 } },
        { "jump-released", jump2, input::Action::ID{ 4 } },
    };
    for (const auto& a : actions)
        is.create(a);

    is.update();
    is.debug_print_actions();


    const auto instances = query_all_devices();
    if (std::empty(instances))
    {
        std::cout << "No devices found!\n";
        std::exit(EXIT_FAILURE);
    }
    else
    {
        for (const auto& d : instances)
        {
            std::cout << "Interface: " << d.description << '\n'
                      << "Native ID: " << d.port << '\n';
            ds.attach_device_arrival_callback(d.port, DeviceSystem::HandlerID{ 1 }, on_device_arrival);
            ds.attach_device_removal_callback(d.port, DeviceSystem::HandlerID{ 1 }, on_device_removal);
        }
    }


    const std::chrono::milliseconds wait{ 33 };
    for (;;)
    {
        //ds.update();
        /*
        if (const auto [ec, s] = read({ 0 }); !ec)
        {
            std::printf("LSX: %f\tLSY: %f\tRSX: %f\tRSY: %f\tLT: %f\tRT: %f\n",
                s.axes[0], s.axes[1], s.axes[2], s.axes[3], s.axes[4], s.axes[5]);
        }
        else
            std::cout << ec.message() << '\n'
                      << "Device not found!\n";
                      */

        is.update();
        std::this_thread::sleep_for(wait);
    }
}