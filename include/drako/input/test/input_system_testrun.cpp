#include "drako/input/device_system.hpp"
#include "drako/input/input_system.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

void jump1() noexcept { std::cout << "[INPUT] Jump 1!\n"; }
void jump2() noexcept { std::cout << "[INPUT] Jump 2!\n"; }
void fire1() noexcept { std::cout << "[INPUT] Fire 1!\n"; }
void fire2() noexcept { std::cout << "[INPUT] Fire 2!\n"; }

int main()
{
    using namespace input;
    using namespace std::chrono_literals;

    InputSystemRuntime is;

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

    /*
    ActionMap        map;
    std::ifstream    f{ "test/action_map.dson" };
    drako::dson::DOM dom{};
    dom >> map;
    */

    const std::vector<Gesture> presses = {
        { .name = "jump-pressed", .control = BooleanControlID{ 1 }, .event = EventID{ 1 } }
        //{ "fire-pressed", VKey::xbox_b, EventID{ 1 } }
    };
    for (const auto& gesture : presses)
        is.create_press_gesture(gesture);

    const std::vector<Gesture> releases = {
        { .name = "jump-released", .control = BooleanControlID{ 1 }, .event = EventID{ 2 } }
        //{ "fire-released", VKey::xbox_y }
    };
    for (const auto& gesture : releases)
        is.create_release_gesture(gesture);

    /*
    const std::vector<GamepadButtonBinding> bindings = {
        { .name = "a1", .id = BindingID{ 1 }, .button = drako::GamepadButtonID{ 1 }, .control = BooleanControlID{ 1 } }
    };
    for (const auto& binding : bindings)
        is.create(binding);*/

    is.debug_print_on_press_table();
    is.debug_print_on_release_table();

    const std::vector<Action> actions = {
        { "fire-pressed", fire1, Action::ID{ 1 } },
        { "fire-released", fire2, Action::ID{ 2 } },
        { "jump-pressed", jump1, Action::ID{ 3 } },
        { "jump-released", jump2, Action::ID{ 4 } },
    };
    for (const auto& a : actions)
        is.create(a);

    is.update();
    is.debug_print_actions();

    for (;;)
    {
        is.update();
        /*
        const auto [pressed, released, ec] = query_gamepad_events(GamepadPlayerPort{ 0 });
        std::cout << "State: " << ec.value() << ", " << ec.message() << '\n';

        std::cout << "Pressed buttons:\n";
        for (const auto code : pressed)
            std::cout << '\t' << translate_native_code(code) << '\n';

        std::cout << "Released buttons:\n";
        for (const auto code : released)
            std::cout << '\t' << translate_native_code(code) << '\n';
            */
        std::this_thread::sleep_for(200ms);
    }
}