#pragma once
#ifndef DRAKO_AUDIO_OUTPUT_DEVICE_HPP
#define DRAKO_AUDIO_OUTPUT_DEVICE_HPP

#include <memory>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#include "drako/audio/native_audio_api.hpp"

namespace drako::audio
{
    // Unique id of an audio rendering device.
    //
    class output_device_guid
    {
    public:

        #if defined(DRAKO_PLT_WIN32)

        explicit output_device_guid() noexcept
            : _win_guid_str(nullptr, CoTaskMemFree)
        {
        }

        explicit output_device_guid(LPWSTR guid) noexcept
            : _win_guid_str(guid, CoTaskMemFree)
        {
        }

        output_device_guid(const output_device_guid&) = delete;
        output_device_guid& operator=(const output_device_guid&) = delete;

        output_device_guid(output_device_guid&& other) noexcept
            : _win_guid_str(std::move(other._win_guid_str))
        {
        }

        output_device_guid& operator=(output_device_guid&& other) noexcept
        {
            if (this != &other)
            {
                _win_guid_str = std::move(other._win_guid_str);
            }
            return *this;
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE const LPWSTR native_guid() const noexcept { return _win_guid_str.get(); }

    private:

        std::unique_ptr<WCHAR, decltype(&CoTaskMemFree)> _win_guid_str;

        #else
        #error Platform not supported
        #endif
    };


    // Gets the guid of the default device used for audio capture as configured in the system.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, output_device_guid>
        default_output_device() noexcept;

    // Lists all the active devices with an audio capture interface.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, std::vector<output_device_guid>>
        query_output_devices() noexcept;

    // Lists the active devices with an audio capture interface that satisfies the requirements.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, std::vector<output_device_guid>>
        query_output_devices(const stream_format& format) noexcept;


    // An output device that can render audio data.
    //
    class output_device
    {
    public:

    private:
    };
}

#endif // !DRAKO_AUDIO_OUTPUT_DEVICE_HPP