#pragma once
#ifndef DRAKO_AUDIO_INPUT_DEVICE_HPP
#define DRAKO_AUDIO_INPUT_DEVICE_HPP

#include <chrono>
#include <cstdlib>
#include <memory>
#include <system_error>
#include <tuple>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include "drako/audio/native_audio_api.hpp"

#if defined(DRAKO_PLT_WIN32)
#include "drako/core/drako_api_win.hpp"
#include <Audioclient.h>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#endif

namespace drako::audio
{
    // Unique id of an audio capture device.
    //
    class input_device_guid
    {
    public:

        #if defined(DRAKO_PLT_WIN32)

        explicit input_device_guid() noexcept
            : _win_guid_str(nullptr, CoTaskMemFree)
        {
        }

        explicit input_device_guid(LPWSTR guid) noexcept
            : _win_guid_str(guid, CoTaskMemFree)
        {
        }

        input_device_guid(const input_device_guid&) = delete;
        input_device_guid& operator=(const input_device_guid&) = delete;

        input_device_guid(input_device_guid&& other) noexcept
            : _win_guid_str(std::move(other._win_guid_str))
        {
        }

        input_device_guid& operator=(input_device_guid&& other) noexcept
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
    DRAKO_NODISCARD std::tuple<std::error_code, input_device_guid>
        default_input_device() noexcept;

    // Lists all the active devices with an audio capture interface.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, std::vector<input_device_guid>>
        query_input_devices() noexcept;

    // Lists the active devices with an audio capture interface that satisfies the requirements.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, std::vector<input_device_guid>>
        query_input_devices(const stream_format& format) noexcept;


    // An input device that can provide audio data capture.
    //
    class input_device
    {
    public:

        #if defined(DRAKO_PLT_WIN32)

        explicit input_device() noexcept
        {
        }

        explicit input_device(IMMDevice* dev_, IAudioClient* acl_, IAudioCaptureClient* acc_) noexcept;

        #endif

        ~input_device() noexcept;

        input_device(const input_device&) = delete;
        input_device& operator=(const input_device&) = delete;

        input_device(input_device&&) noexcept;
        input_device& operator=(input_device&&) noexcept;


        // Starts recording data on the audio stream.
        //
        DRAKO_NODISCARD std::error_code enable() noexcept;

        // Stops recording data on the audio stream.
        //
        DRAKO_NODISCARD std::error_code disable() noexcept;

        // Flushes the backing buffer and resets the audio stream state.
        //
        DRAKO_NODISCARD std::error_code reset() noexcept;

        // Reads audio data from the backing buffer of the capture device.
        //
        DRAKO_NODISCARD std::tuple<std::error_code, size_t> read(void* dst_, size_t frames_) noexcept;

        // Returns an invalid device handle.
        //
        DRAKO_NODISCARD static input_device invalid_device() noexcept;

    private:

        #if defined(DRAKO_PLT_WIN32)

        IMMDevice* _device                            = nullptr;
        IAudioClient* _audio_client_interface         = nullptr;
        IAudioCaptureClient* _audio_capture_interface = nullptr;

        #else
        #error Platform not supported
        #endif
    };


    // Creates an audio capture device with the device-specific default format.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, input_device>
        open_device(input_device_guid device_, std::chrono::microseconds buffer_capacity_) noexcept;

    // Creates an audio capture device with the specified format.
    //
    DRAKO_NODISCARD std::tuple<std::error_code, input_device>
        open_device(input_device_guid device_, const stream_format& format_, std::chrono::microseconds buffer_capacity_) noexcept;
}

#endif // !DRAKO_AUDIO_INPUT_DEVICE_HPP
