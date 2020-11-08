#pragma once
#ifndef DRAKO_AUDIO_INPUT_DEVICE_HPP
#define DRAKO_AUDIO_INPUT_DEVICE_HPP

#include "drako/audio/native_audio_api.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Audioclient.h>
#include <Windows.h>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#endif

#include <system_error>
#include <tuple>
#include <vector>

namespace drako::audio
{
    /// @brief System-level unique id of an audio capture device.
    class input_device_id
    {
    public:
#if defined(DRAKO_PLT_WIN32)
        using native_id_type = LPWSTR;
#endif

        explicit input_device_id() noexcept = default;
        explicit input_device_id(native_id_type id) noexcept
            : _guid(id)
        {
        }

        input_device_id(const input_device_id&) = default;
        input_device_id& operator=(const input_device_id&) = default;

        [[nodiscard]] native_id_type native_id() const noexcept { return (WCHAR*)nullptr; }

    private:
#if defined(DRAKO_PLT_WIN32)
        std::wstring _guid;
#else
#error Platform not supported
#endif
    };


    /// @brief Handle of the default device used for audio capture as configured in the system.
    [[nodiscard]] std::tuple<std::error_code, input_device_id>
    default_input_device() noexcept;

    /// @brief Lists all the active devices with an audio capture interface.
    [[nodiscard]] std::tuple<std::error_code, std::vector<input_device_id>>
    query_input_devices() noexcept;

    /// @brief Lists the active devices with an audio capture interface that satisfies the requirements.
    [[nodiscard]] std::tuple<std::error_code, std::vector<input_device_id>>
    query_input_devices(const stream_format& format) noexcept;


    /// @brief Handle of a device that can capture audio data.
    class input_device
    {
    public:
#if defined(DRAKO_PLT_WIN32)

        explicit input_device() noexcept
        {
        }

        explicit input_device(IMMDevice* dev, IAudioClient* acl, IAudioCaptureClient* acc) noexcept;

#endif
        input_device(const input_device&) = delete;
        input_device& operator=(const input_device&) = delete;

        input_device(input_device&&) noexcept;
        input_device& operator=(input_device&&) noexcept;

        ~input_device() noexcept;


        /// @brief Starts recording data on the audio stream.
        [[nodiscard]] std::error_code enable() noexcept;

        /// @brief Stops recording data on the audio stream.
        [[nodiscard]] std::error_code disable() noexcept;

        /// @brief Flushes the backing buffer and resets the audio stream state.
        [[nodiscard]] std::error_code reset() noexcept;

        /// @brief Reads audio data from the backing buffer of the capture device.
        [[nodiscard]] std::tuple<std::error_code, size_t> read(void* dst_, size_t frames_) noexcept;

        /// @brief Returns an invalid device handle.
        [[nodiscard]] static input_device invalid_device() noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)

        IMMDevice*           _device                  = nullptr;
        IAudioClient*        _audio_client_interface  = nullptr;
        IAudioCaptureClient* _audio_capture_interface = nullptr;

#else
#error Platform not supported
#endif
    };


    // Creates an audio capture device with the device-specific default format.
    //
    [[nodiscard]] std::tuple<std::error_code, input_device>
    open(const input_device_id& dev, std::chrono::microseconds buffer_capacity) noexcept;

    // Creates an audio capture device with the specified format.
    //
    [[nodiscard]] std::tuple<std::error_code, input_device>
    open(const input_device_id& dev, const stream_format& f, std::chrono::microseconds buffer_capacity) noexcept;

} // namespace drako::audio

#endif // !DRAKO_AUDIO_INPUT_DEVICE_HPP