#include "drako/audio/audio_output_device.hpp"

#include <system_error>
#include <tuple>
#include <vector>

#if !defined(DRAKO_PLT_WIN32)
#error No target platform
#endif

namespace drako::audio
{
    using _ec = std::error_code;
    using _id = output_device_id;

    std::tuple<_ec, _id> default_output_device() noexcept
    {
        // TODO: use smart pointers instead of raws

        IMMDeviceEnumerator* enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            return { std::error_code(hr, std::system_category()), _id{} };
        }

        // std::unique_ptr<IMMDevice, decltype(IMMDevice::Release)> device;
        IMMDevice* device;
        if (const auto hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
            hr != S_OK)
        {
            enumerator->Release();
            return { std::error_code(hr, std::system_category()), _id{} };
        }

        LPWSTR guid;
        if (const auto hr = device->GetId(&guid); hr != S_OK)
        {
            enumerator->Release();
            device->Release();
            return { std::error_code(hr, std::system_category()), _id{} };
        }
        enumerator->Release();
        device->Release();
        return { _ec{}, _id{ guid } };
    }

    //std::tuple<_ec, std::vector<_id>> query_output_devices() noexcept;

    //std::tuple<_ec, std::vector<_id>> query_output_devices(const stream_format& format) noexcept;
} // namespace drako::audio