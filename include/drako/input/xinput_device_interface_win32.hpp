#pragma once
#ifndef DRAKO_XINPUT_INTERFACE_HPP_
#define DRAKO_XINPUT_INTERFACE_HPP_

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/input/input_system_types.hpp"
#include "drako/input/input_system_utils.hpp"

#include <Windows.h>
// ^^^/vvv preserve inclusion order
#include <Xinput.h>
#include <winerror.h>

#include <system_error>
#include <tuple>
#include <type_traits>
#include <vector>

#if !defined(DRAKO_PLT_WIN32)
#error Wrong target platform - this header should be included only in Windows builds
#endif

namespace drako::engine
{
    //std::vector<std::string_view> xinput_names   = {};
    //std::vector<std::size_t>      xinput_indexes = {};


    [[nodiscard]] std::vector<size_t> xinput_active_gamepad_indexes() noexcept;

    class xinput_device_interface : public input_device_interface
    {
    public:
        explicit xinput_device_interface(size_t user_index) noexcept
            : _user_index(user_index)
        {
            DRAKO_ASSERT(user_index < XUSER_MAX_COUNT, "Range allowed by XInput API");
        }

        void set_vibration(float left, float right) noexcept;

        //[[nodiscard]] std::vector<float> axis_states() noexcept override;

        //[[nodiscard]] std::vector<bool> button_states() noexcept override;

        //[[nodiscard]] std::vector<_native_input_event> poll_events() noexcept override;

        [[nodiscard]] std::tuple<std::error_code, input_device_state>
        poll_state() noexcept override;

    private:
        XINPUT_STATE _last_state;
        DWORD        _user_index;
    };


} // namespace drako::engine

#endif // !DRAKO_XINPUT_INTERFACE_HPP_