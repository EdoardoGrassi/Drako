#include "drako/audio/audio_output_device.hpp"

#if !defined(DRAKO_PLT_WIN32)
#error No target platform
#endif

namespace drako::audio
{
    std::tuple<std::error_code, output_device_guid>
        default_output_device() noexcept
    {
        // TODO: use smart pointers instead of raws

        IMMDeviceEnumerator* enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted),
                output_device_guid{});
        }

        // std::unique_ptr<IMMDevice, decltype(IMMDevice::Release)> device;
        IMMDevice* device;
        if (const auto hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            enumerator->Release();
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted),
                output_device_guid{});
        }

        LPWSTR guid;
        if (const auto hr = device->GetId(&guid);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            enumerator->Release();
            device->Release();
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted),
                output_device_guid{});
        }
        enumerator->Release();
        device->Release();
        return std::tuple(std::error_code{}, output_device_guid(guid));
    }

    std::tuple<std::error_code, std::vector<output_device_guid>>
        query_output_devices() noexcept
    {
        // TODO: impl
    }

    std::tuple<std::error_code, std::vector<output_device_guid>>
        query_output_devices(const stream_format& format) noexcept
    {
        // TODO: impl
    }
}