#include "drako/input/input_system.hpp"

#include "drako/devel/logging.hpp"
#include "drako/input/device_system_types.hpp"

#include <cassert>
#include <iostream>

namespace drako::input
{
    using _this = input_system;

    _this::input_system(std::span<const on_press> p, std::span<const on_release> r)
    {
        for (const auto& x : p)
        {
            _on_press.events.push_back(x.event);
            _on_press.vkeys.push_back(x.key);
        }
        for (const auto& x : r)
        {
            _on_release.events.push_back(x.event);
            _on_release.vkeys.push_back(x.key);
        }
    }

    /*
    void _this::create(const on_press& a) noexcept
    {
        assert(a.instance);
        _on_press.pending_create.push_back(a);
    }
    */

    /*
    void _this::create(const on_release& a) noexcept
    {
        assert(a.instance);
        _on_release.pending_create.push_back(a);
    }
    */

    void _this::create(const action& a) noexcept
    {
        assert(a.instance);
        _actions.pending_create.push_back(a);
    }

    void _this::destroy(const action::id id) noexcept
    {
        assert(id);
        _actions.pending_destroy.push_back(id);
    }

    void _this::enable(const action::id id) noexcept
    {
        assert(id);
        _actions.pending_enable.push_back(id);
    }

    void _this::disable(const action::id id) noexcept
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

    void _this::update() noexcept
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
            _actions.instances.push_back(a.instance);
            _actions.events.push_back(a.event);
            _actions.names.push_back(a.name);
            _actions.callbacks.push_back(a.reaction);
        }
        _actions.pending_create.clear();

        std::vector<vkey> pressed;  // could use alloca() instead
        std::vector<vkey> released; // ^^^

        const auto [ec, state] = read(device_id{ 0 });
        if (ec)
            return;

        const auto changed_from_last_update = _last_state.buttons ^ state.buttons;
        {
            const auto mask = changed_from_last_update & state.buttons;
            for (auto i = 0; i < std::size(mask); ++i)
                if (mask.test(i))
                    pressed.push_back(vkey(i));
        }
        {
            const auto mask = changed_from_last_update & (~state.buttons);
            for (auto i = 0; i < std::size(mask); ++i)
                if (mask.test(i))
                    released.push_back(vkey(i));
        }
        _last_state = state;

        _temp_event_buffer.clear(); // could use alloca() instead
        for (const auto c : pressed)
            for (auto i = 0; i < std::size(_on_press.vkeys); ++i)
                if (_on_press.vkeys[i] == c)
                    _temp_event_buffer.push_back(_on_press.events[i]);

        for (const auto c : released)
            for (auto i = 0; i < std::size(_on_release.vkeys); ++i)
                if (_on_release.vkeys[i] == c)
                    _temp_event_buffer.push_back(_on_release.events[i]);

        // join selected actions with matching callbacks from action table
        _temp_invoke_buffer.clear();
        for (auto e : _temp_event_buffer)
            for (auto i = 0; i < std::size(_actions.instances); ++i)
                if (e == _actions.events[i])
                    _temp_invoke_buffer.push_back(_actions.callbacks[i]);

        for (auto c : _temp_invoke_buffer)
            std::invoke(c);
    }

    void _this::dbg_print_actions() const noexcept
    {
        std::cout << "[INFO] Input System [<actions> table]:\n"
                  << "\t[instance-id]\t[name]\n";
        for (auto i = 0; i < std::size(_actions.instances); ++i)
            std::cout << '\t' << _actions.instances[i]
                      << '\t' << _actions.names[i]
                      << std::endl;
    }

    void _this::dbg_print_bindings() const noexcept
    {
        std::cout << "[INFO] Input System [<bindings> table]:\n"
                  << "\t[name]\t[keycode]\n";
        for (auto i = 0; i < std::size(_bindings.names); ++i)
            std::cout << '\t' << _bindings.names[i]
                      << '\t' << _bindings.vkeys[i]
                      << std::endl;
    }

    void _this::dbg_print_on_press_table() const noexcept
    {
        std::cout << "[INFO] Input System [<on-press> table]:\n"
                  << "\t[event]\t[keycode]\n";
        for (auto i = 0; i < std::size(_on_press.events); ++i)
            std::cout << '\t' << _on_press.events[i]
                      << '\t' << _on_press.vkeys[i]
                      << std::endl;
    }

    void _this::dbg_print_on_release_table() const noexcept
    {
        std::cout << "[INFO] Input System [<on-release> table]:\n"
                  << "\t[event]\t[keycode]\n";
        for (auto i = 0; i < std::size(_on_release.events); ++i)
            std::cout << '\t' << _on_release.events[i]
                      << '\t' << _on_release.vkeys[i]
                      << std::endl;
    }

} // namespace drako::input