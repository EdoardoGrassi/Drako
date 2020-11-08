#pragma once
#ifndef DRAKO_AUDIO_OUTPUT_DEVICE_HPP
#define DRAKO_AUDIO_OUTPUT_DEVICE_HPP

#include "drako/audio/native_audio_api.hpp"
#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#include <memory>
#include <system_error>
#include <tuple>

namespace drako::audio
{
    /// @brief System-level unique id of an audio rendering device.
    class output_device_id
    {
    public:
#if defined(DRAKO_PLT_WIN32)

        explicit output_device_id() noexcept
            : _win_guid_str(nullptr, CoTaskMemFree)
        {
        }

        explicit output_device_id(LPWSTR guid) noexcept
            : _win_guid_str(guid, CoTaskMemFree)
        {
        }

        output_device_id(const output_device_id&);
        output_device_id& operator=(const output_device_id&);

        output_device_id(output_device_id&& other) noexcept
            : _win_guid_str(std::move(other._win_guid_str))
        {
        }

        output_device_id& operator=(output_device_id&& other) noexcept
        {
            if (this != &other)
            {
                _win_guid_str = std::move(other._win_guid_str);
            }
            return *this;
        }

        [[nodiscard]] DRAKO_FORCE_INLINE const LPWSTR native_guid() const noexcept { return _win_guid_str.get(); }

    private:
        std::unique_ptr<WCHAR, decltype(&CoTaskMemFree)> _win_guid_str;

#else
#error Platform not supported
#endif
    };


    /// @brief Handle of the default device used for audio capture as configured in the system.
    [[nodiscard]] std::tuple<std::error_code, output_device_id> default_output_device() noexcept;

    /// @brief Lists all the active devices with an audio capture interface.
    [[nodiscard]] std::tuple<std::error_code, std::vector<output_device_id>>
    query_output_devices() noexcept;

    /// @brief Lists the active devices with an audio capture interface that satisfies the requirements.
    [[nodiscard]] std::tuple<std::error_code, std::vector<output_device_id>>
    query_output_devices(const stream_format& format) noexcept;


    /// @brief Handle of a device that can reproduce audio data.
    class output_device
    {
    public:
    private:
    };
} // namespace drako::audio

#endif // !DRAKO_AUDIO_OUTPUT_DEVICE_HPP