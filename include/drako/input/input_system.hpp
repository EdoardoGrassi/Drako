#pragma once
#ifndef DRAKO_INPUT_SYSTEM_HPP
#define DRAKO_INPUT_SYSTEM_HPP

#include "drako/core/typed_handle.hpp"
#include "drako/input/device_system.hpp"
#include "drako/input/device_system_types.hpp"
#include "drako/input/input_system_types.hpp"

#include <cassert>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace drako::input
{
    //using pkey = std::uint32_t;
    using event_id = std::size_t;

    struct binding
    {
        std::string name;
        vkey        code;
    };

    struct action
    {
        DRAKO_DEFINE_TYPED_ID(id, std::uint32_t);
        using callback = void (*)(void);

        action(const std::string_view name, callback c, id id) noexcept
            : name{ name }
            , reaction{ c }
            , instance{ id }
            , event{ std::hash<std::string_view>{}(name) } {}

        std::string name;
        callback    reaction;
        const id    instance;
        event_id    event;
    };

    struct on_press
    {
        DRAKO_DEFINE_TYPED_ID(id, std::size_t);

        on_press(const std::string& name, const vkey k) noexcept
            : key{ k }, event{ std::hash<std::string>{}(name) } {}

        vkey     key;
        event_id event;
    };

    struct on_release
    {
        DRAKO_DEFINE_TYPED_ID(id, std::size_t);

        on_release(const std::string& name, vkey k) noexcept
            : key{ k }, event{ std::hash<std::string>{}(name) } {}

        vkey     key;
        event_id event;
    };


    class input_system
    {
    public:
        explicit input_system() = default;
        explicit input_system(std::span<const on_press>, std::span<const on_release>);

        input_system(const input_system&) = delete;
        input_system& operator=(const input_system&) = delete;

        input_system(input_system&&) = delete;
        input_system& operator=(input_system&&) = delete;

        //void create(const on_press&) noexcept;
        //void create(const on_release&) noexcept;
        void create(const action&) noexcept;

        //void destroy(const on_press::id) noexcept;
        //void destroy(const on_release::id) noexcept;
        void destroy(const action::id) noexcept;

        //void enable(const on_press::id) noexcept;
        //void enable(const on_release::id) noexcept;
        void enable(const action::id) noexcept;

        //void disable(const on_press::id) noexcept;
        //void disable(const on_release::id) noexcept;
        void disable(const action::id) noexcept;

        // Bind a named event with a physical key.
        //void bind(const std::string& interaction, const std::string& action) noexcept;

        // Bind a named event with a physical key.
        void bind(const std::string_view name, const vkey key) noexcept;

        void update() noexcept;

        // Run update cycle with incoming event.
        void update(const device_input_state&) noexcept;

        // Run update cycle with events generated from an input system.
        void update(const std::vector<device_input_state>&) noexcept;

#if !defined(DRAKO_API_RELEASE)
        void dbg_print_actions() const noexcept;
        void dbg_print_bindings() const noexcept;
        void dbg_print_on_press_table() const noexcept;
        void dbg_print_on_release_table() const noexcept;
#endif

    private:
        device_input_state _last_state;

        std::vector<event_id>         _temp_event_buffer;
        std::vector<action::callback> _temp_invoke_buffer;

        struct _bindings_soa
        {
            std::vector<vkey>        vkeys;
            std::vector<std::string> names;
        } _bindings;

        struct _actions_table
        {
            // TODO: thread safety
            std::vector<action>     pending_create;
            std::vector<action::id> pending_destroy;
            std::vector<action::id> pending_enable;
            std::vector<action::id> pending_disable;
            // ^^^

            std::vector<action::id>       instances; // unique id of each action instance
            std::vector<event_id>         events;    // trigger event
            std::vector<std::string>      names;     // debug name
            std::vector<action::callback> callbacks; // reaction to the trigger
        } _actions;

        struct _on_press_table
        {
            // TODO: vvv these need to be threadsafe
            //std::vector<on_press>     pending_create;
            //std::vector<on_press::id> pending_destroy;
            //std::vector<on_press::id> pending_enable;
            //std::vector<on_press::id> pending_disable;
            // ^^^

            //std::vector<on_press::id> ids;
            std::vector<std::size_t> events;
            std::vector<vkey>        vkeys;
            //std::vector<std::string>  names;

        } _on_press;

        struct _on_release_table
        {
            // TODO: vvv these need to be threadsafe
            //std::vector<on_release>     pending_create;
            //std::vector<on_release::id> pending_destroy;
            //std::vector<on_release::id> pending_enable;
            //std::vector<on_release::id> pending_disable;
            // ^^^

            //std::vector<on_release::id> ids;
            std::vector<std::size_t> events;
            std::vector<vkey>        vkeys;
            //std::vector<std::string>    names;
        } _on_release;

        /*struct _on_hold_table
        {
            std::vector<on_hold::id> ids;
            std::vector<std::string> names;
            std::vector<action::id>  actions;
            std::vector<float>       duration;
            std::vector<float>       elapsed;
        } _on_hold;*/
    };

} // namespace drako::input

#endif // !DRAKO_INPUT_SYSTEM_HPP