#include "drako/input/input_system.hpp"

#include "drako/devel/logging.hpp"
#include "drako/input/device_types.hpp"

#include <cassert>
#include <iostream>

namespace input
{
    void InputSystemRuntime::create(const Action& a)
    {
        assert(a.instance);
        _actions.pending_create.push_back(a);
    }

    void InputSystemRuntime::create(const GamepadButtonBinding& b)
    {
        assert(b.id);
        assert(b.button);
        assert(b.control);

        auto& t = _gamepad_button_bindings;
        t.binding.push_back(b.id);
        t.button.push_back(b.button);
        t.control.push_back(b.control);
        t.name.push_back(b.name);
    }

    void InputSystemRuntime::destroy(const Action::ID id) noexcept
    {
        assert(id);
        _actions.pending_destroy.push_back(id);
    }

    void InputSystemRuntime::enable(const Action::ID id) noexcept
    {
        assert(id);
        _actions.pending_enable.push_back(id);
    }

    void InputSystemRuntime::disable(const Action::ID id) noexcept
    {
        assert(id);
        _actions.pending_enable.push_back(id);
    }

    /*
    void _this::bind(const std::string_view name, const vkey key) noexcept
    {
        if (const auto it = std::find(
                std::cbegin(_on_press.events), std::cend(_on_press.events), name);
            it != std::cend(_on_press.events))
        {
            const auto index = std::distance(std::cbegin(_on_press.events), it);

            _on_press.vkeys[index] = key;
        }
    }

    void _this::bind(const std::string_view name, const vkey key) noexcept
    {
        if (const auto it = std::find(
                std::cbegin(_on_release.events), std::cend(_on_release.events), name);
            it != std::cend(_on_release.events))
        {
            const auto index = std::distance(std::cbegin(_on_release.events), it);

            _on_release.vkeys[index] = key;
        }
    }
    */

    void InputSystemRuntime::bind(BindingID b, GamepadButtonID button) noexcept
    {
        auto& t = _gamepad_button_bindings;
        if (const auto f = std::find(std::cbegin(t.binding), std::cend(t.binding), b);
            f != std::cend(t.binding))
        {
            const auto index = static_cast<std::size_t>(std::distance(std::cbegin(t.binding), f));
            t.button[index]  = button;
        }
    }

    void InputSystemRuntime::create_press_gesture(const Gesture& g)
    {
        assert(g.control);
        assert(g.event);

        auto& t = _on_press;
        t.control.push_back(g.control);
        t.event.push_back(g.event);
        t.name.push_back(g.name);
    }

    void InputSystemRuntime::create_release_gesture(const Gesture& g)
    {
        assert(g.control);
        assert(g.event);

        auto& t = _on_release;
        t.control.push_back(g.control);
        t.event.push_back(g.event);
        t.name.push_back(g.name);
    }

    void InputSystemRuntime::update() noexcept
    {
        /*
        for (const auto& a : _on_press.pending_create)
        {
            _on_press.events.push_back(a.event);
            _on_press.vkeys.push_back(a.key);
        }
        _on_press.pending_create.clear();

        for (const auto& a : _on_release.pending_create)
        {
            _on_release.events.push_back(a.event);
            _on_release.vkeys.push_back(a.key);
        }
        _on_release.pending_destroy.clear();
        */

        for (const auto& a : _actions.pending_create)
        {
            _actions.action.push_back(a.instance);
            _actions.event.push_back(a.event);
            _actions.name.push_back(a.name);
            _actions.callback.push_back(a.reaction);
        }
        _actions.pending_create.clear();

        const auto [state, ec] = query_gamepad_state(GamepadPlayerPort{ 0 });
        if (ec)
            return;

        std::vector<BooleanControlID> pressed, released; // could use alloca() instead

        const auto changed_from_last_update = _last_state.buttons ^ state.buttons;
        {
            const auto mask = changed_from_last_update & state.buttons;
            for (std::uint32_t i = 0; i < std::size(mask); ++i)
                if (mask.test(i))
                    pressed.push_back(BooleanControlID{ i });
        }
        {
            const auto mask = changed_from_last_update & (~state.buttons);
            for (std::uint32_t i = 0; i < std::size(mask); ++i)
                if (mask.test(i))
                    released.push_back(BooleanControlID{ i });
        }
        _last_state = state;

        _temp_event_buffer.clear(); // could use alloca() instead
        for (const auto c : pressed)
            for (auto i = 0; i < std::size(_on_press.control); ++i)
                if (_on_press.control[i] == c)
                    _temp_event_buffer.push_back(_on_press.event[i]);

        for (const auto c : released)
            for (auto i = 0; i < std::size(_on_release.control); ++i)
                if (_on_release.control[i] == c)
                    _temp_event_buffer.push_back(_on_release.event[i]);

        /*vvv join selected actions with matching callbacks from actions table vvv*/
        _temp_invoke_buffer.clear();
        for (const auto e : _temp_event_buffer)
            for (auto i = 0; i < std::size(_actions.action); ++i)
                if (e == _actions.event[i])
                    _temp_invoke_buffer.push_back(_actions.callback[i]);

        for (auto c : _temp_invoke_buffer)
            std::invoke(c);
    }

    void InputSystemRuntime::debug_print_actions() const noexcept
    {
        std::cout << "[INFO] Input System [actions] table:\n"
                  << "\t[instance-id]\t[name]\t[event]\n";
        for (auto i = 0; i < std::size(_actions.action); ++i)
            std::cout << '\t' << _actions.action[i]
                      << '\t' << _actions.name[i]
                      << '\t' << _actions.event[i]
                      << std::endl;
    }

    void InputSystemRuntime::debug_print_gamepad_bindings() const noexcept
    {
        const auto& t = _gamepad_button_bindings;
        std::cout << "[INFO] Input System [gamepad-bindings] table:\n"
                  << "\t[name]\t[id]\t[button]\t[control]\n";
        for (auto i = 0; i < std::size(t.name); ++i)
            std::cout << '\t' << t.name[i]
                      << '\t' << t.binding[i]
                      << '\t' << t.button[i]
                      << '\t' << t.control[i]
                      << std::endl;

        // TODO: also print axis table
    }

    void InputSystemRuntime::debug_print_on_press_table() const noexcept
    {
        const auto& t = _on_press;
        std::cout << "[INFO] Input System [on-press] table:\n"
                  << "\t[Event ID]\t[Control ID]\n";
        for (auto i = 0; i < std::size(t.event); ++i)
            std::cout << '\t' << t.event[i]
                      << '\t' << t.control[i]
                      << std::endl;
    }

    void InputSystemRuntime::debug_print_on_release_table() const noexcept
    {
        const auto& t = _on_release;
        std::cout << "[INFO] Input System [on-release] table:\n"
                  << "\t[Event ID]\t[Control ID]\n";
        for (auto i = 0; i < std::size(t.event); ++i)
            std::cout << '\t' << t.event[i]
                      << '\t' << t.control[i]
                      << std::endl;
    }

} // namespace input