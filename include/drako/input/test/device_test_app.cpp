#include "drako/input/device_system.hpp"
#include "drako/input/input_system.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace drako;
using namespace std::chrono_literals;


void on_device_arrival(const device_change_event& e) noexcept
{
    std::cout << "Connected device with id " << e.id << '\n';
}
void on_device_removal(const device_change_event& e) noexcept
{
    std::cout << "Disconnected device with id " << e.id << '\n';
}

void jump1() noexcept { std::cout << "[INPUT] Jump 1!\n"; }
void jump2() noexcept { std::cout << "[INPUT] Jump 2!\n"; }
void fire1() noexcept { std::cout << "[INPUT] Fire 1!\n"; }
void fire2() noexcept { std::cout << "[INPUT] Fire 2!\n"; }

int main()
{
    using namespace input;

    const std::vector<input::on_press> presses = {
        { "jump-pressed", vkey::xbox_a },
        { "fire-pressed", vkey::xbox_b }
    };

    const std::vector<input::on_release> releases = {
        { "jump-released", vkey::xbox_x },
        { "fire-released", vkey::xbox_y }
    };

    device_system devices{ { .devices_poll_rate = 5s } };
    input_system  inputs{ presses, releases };

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

    const std::vector<input::binding> bindings = {
        { .name = "a1", .code = vkey::dpad_up },
        { "a2", vkey::dpad_down },
        { "a3", vkey::dpad_left },
        { "a4", vkey::dpad_right },
        { "a5", vkey::button_north },
        { "a6", vkey::button_south },
        { "a7", vkey::button_west },
        { "a8", vkey::button_east }
    };

    inputs.dbg_print_on_press_table();
    inputs.dbg_print_on_release_table();



    const std::vector<input::action> actions = {
        { "fire-pressed", fire1, input::action::id{ 1 } },
        { "fire-released", fire2, input::action::id{ 2 } },
        { "jump-pressed", jump1, input::action::id{ 3 } },
        { "jump-released", jump2, input::action::id{ 4 } },
    };
    for (const auto& a : actions)
        inputs.create(a);

    inputs.update();
    inputs.dbg_print_actions();


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
            devices.attach_device_arrival_callback(1, on_device_arrival);
            devices.attach_device_removal_callback(2, on_device_removal);
        }
    }


    const std::chrono::milliseconds wait{ 33 };
    for (;;)
    {
        //devices.update();
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

        inputs.update();
        std::this_thread::sleep_for(wait);
    }
}