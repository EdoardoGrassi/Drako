#include "drako/audio/audio_input_device.hpp"

#include <wrl/client.h> // Microsoft::WRL::ComPtr

#include <cassert>
#include <chrono>
#include <memory>
#include <system_error>
#include <tuple>

#if !defined(DRAKO_PLT_WIN32)
#error No target platform
#endif

namespace drako::audio
{
    namespace _chr = std::chrono;
    namespace _wrl = Microsoft::WRL;
    using _ec      = std::error_code;
    using _id      = input_device_id;

    [[nodiscard]] std::tuple<_ec, _id> default_input_device() noexcept
    {
        _wrl::ComPtr<IMMDeviceEnumerator> enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), _id{} };
        }

        _wrl::ComPtr<IMMDevice> device;
        if (const auto hr = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &device);
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), _id{} };
        }

        LPWSTR guid;
        if (const auto hr = device->GetId(&guid); hr != S_OK)
            [[unlikely]] return { _ec(hr, std::system_category()), _id{} };

        const input_device_id result{ guid };
        ::CoTaskMemFree(guid); // we neeed to free the original string

        return { _ec{}, result };
    }

    std::tuple<_ec, std::vector<_id>> query_input_devices() noexcept
    {
        // TODO: end impl

        _wrl::ComPtr<IMMDeviceEnumerator> enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), {} };
        }

        _wrl::ComPtr<IMMDeviceCollection> collection;
        if (const auto hr = enumerator->EnumAudioEndpoints(
                eCapture, DEVICE_STATE_ACTIVE, &collection);
            hr != S_OK)
        {
            return { _ec(hr, std::system_category()), {} };
        }

        UINT device_count;
        if (const auto hr = collection->GetCount(&device_count); hr != S_OK)
            [[unlikely]] return { _ec(hr, std::system_category()), {} };

        std::vector<input_device_id> result(device_count);
        // Iterate on the collection of available devices
        for (auto i = 0u; i < device_count; ++i)
        {
            _wrl::ComPtr<IMMDevice> device;
            if (const auto hr = collection->Item(i, &device); hr != S_OK)
                [[unlikely]] return { _ec(hr, std::system_category()), {} };

            // TODO: get device properties
        }

        return { _ec{}, result };
    }

    std::tuple<_ec, std::vector<_id>> query_input_devices(const stream_format& format) noexcept
    {
        // TODO: end impl

        _wrl::ComPtr<IMMDeviceEnumerator> enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), {} };
        }

        _wrl::ComPtr<IMMDeviceCollection> collection;
        if (const auto hr = enumerator->EnumAudioEndpoints(
                eCapture, DEVICE_STATE_ACTIVE, &collection);
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), {} };
        }

        UINT device_count;
        if (const auto hr = collection->GetCount(&device_count); hr != S_OK)
            [[unlikely]] return { _ec(hr, std::system_category()), {} };

        std::vector<input_device_id> result(device_count);
        // Iterate on the collection of available devices
        for (auto i = 0u; i < device_count; ++i)
        {
            _wrl::ComPtr<IMMDevice> device;
            if (const auto hr = collection->Item(i, &device); hr != S_OK)
                [[unlikely]] return { _ec(hr, std::system_category()), {} };

            // TODO: get device properties
        }

        return { _ec{}, result };
    }

    input_device::input_device(IMMDevice* dev, IAudioClient* acl, IAudioCaptureClient* acc) noexcept
        : _device(dev)
        , _audio_client_interface(acl)
        , _audio_capture_interface(acc)
    {
        assert(_device != nullptr);
        assert(_audio_client_interface != nullptr);
        assert(_audio_capture_interface != nullptr);
    }

    input_device::input_device(input_device&& other) noexcept
        : _device(other._device)
        , _audio_client_interface(other._audio_client_interface)
        , _audio_capture_interface(other._audio_capture_interface)
    {
        other._device                  = nullptr;
        other._audio_client_interface  = nullptr;
        other._audio_capture_interface = nullptr;
    }

    input_device& input_device::operator=(input_device&& other) noexcept
    {
        if (this != &other)
        {
            _device                        = other._device;
            _audio_client_interface        = other._audio_client_interface;
            _audio_capture_interface       = other._audio_capture_interface;
            other._device                  = nullptr;
            other._audio_client_interface  = nullptr;
            other._audio_capture_interface = nullptr;
        }
        return *this;
    }

    input_device::~input_device() noexcept
    {
        // TODO: maybe use smart pointers instead
        if (_audio_capture_interface != nullptr)
            _audio_capture_interface->Release();
        if (_audio_client_interface != nullptr)
            _audio_client_interface->Release();
        if (_device != nullptr)
            _device->Release();
    }

    _ec input_device::enable() noexcept
    {
        return (_audio_client_interface->Start() == S_OK)
                   ? std::error_code{}
                   : std::make_error_code(std::errc::operation_not_permitted);
    }

    _ec input_device::disable() noexcept
    {
        auto hr = _audio_client_interface->Stop();
        return (hr == S_OK || hr == S_FALSE) ? _ec{} : _ec(hr, std::system_category());
    }

    std::tuple<_ec, std::size_t> input_device::read(void* dst_, std::size_t frames_) noexcept
    {
        // TODO: end impl

        // TODO: vvv --- maybe remove the next section as its redundant --- vvv
        UINT32 frame_count;
        if (const auto hr = _audio_capture_interface->GetNextPacketSize(&frame_count);
            hr != S_OK)
        {
            return { _ec(hr, std::system_category()), 0 };
        }
        // ^^^ ------------------------------------------------------------ ^^^

        BYTE* buffer_address;
        DWORD buffer_status;
        if (const auto hr = _audio_capture_interface->GetBuffer(
                &buffer_address, &frame_count, &buffer_status, NULL, NULL);
            hr != S_OK)
        {
            return { _ec(hr, std::system_category()), 0 };
        }

        memcpy(dst_, buffer_address, frames_);

        if (const auto hr = _audio_capture_interface->ReleaseBuffer(frame_count);
            hr != S_OK)
        {
            return { _ec(hr, std::system_category()), 0 };
        }

        return { _ec{}, frame_count };
    }

    std::tuple<_ec, input_device> open(const _id& dev, _chr::microseconds capacity) noexcept
    {
        _wrl::ComPtr<IMMDeviceEnumerator> enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        _wrl::ComPtr<IMMDevice> device;
        if (const auto hr = enumerator->GetDevice(dev.native_id(), &device);
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        _wrl::ComPtr<IAudioClient> client;
        if (const auto hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
                NULL, IID_PPV_ARGS_Helper(&client));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        {
            WAVEFORMATEX* format;
            if (const auto hr = client->GetMixFormat(&format); hr != S_OK)
            {
                [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
            }

            if (const auto hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                    AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED,
                    _chr::duration_cast<_chr::nanoseconds>(capacity).count() / 100, // duration time expressed in 100 ns units
                    0,                                                              // device period: always 0 when the device is shared
                    format,
                    NULL);
                hr != S_OK)
            {
                ::CoTaskMemFree(format);
                [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
            }
            ::CoTaskMemFree(format);
        }

        _wrl::ComPtr<IAudioCaptureClient> capture;
        if (const auto hr = client->GetService(
                __uuidof(IAudioCaptureClient), IID_PPV_ARGS_Helper(&capture));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        return { _ec{}, input_device{ device.Get(), client.Get(), capture.Get() } };
    }

    std::tuple<_ec, input_device>
    open(_id dev, const stream_format& f, _chr::microseconds capacity) noexcept
    {
        _wrl::ComPtr<IMMDeviceEnumerator> enumerator;
        // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        _wrl::ComPtr<IMMDevice> device;
        if (const auto hr = enumerator->GetDevice(dev.native_id(), &device);
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        _wrl::ComPtr<IAudioClient> client;
        if (const auto hr = device->Activate(
                __uuidof(IAudioClient), CLSCTX_ALL, NULL, IID_PPV_ARGS_Helper(&client));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        WAVEFORMATEX format{};
        format.wFormatTag      = WAVE_FORMAT_PCM;
        format.nChannels       = f.channels_count();
        format.nSamplesPerSec  = f.sample_rate();
        format.wBitsPerSample  = f.sample_bitdepth();
        format.cbSize          = 0;
        format.nBlockAlign     = (format.wBitsPerSample * format.nChannels) / 8;
        format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;

        if (const auto hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED,
                _chr::duration_cast<_chr::nanoseconds>(capacity).count() / 100, // duration time expressed in 100 ns units
                0,                                                              // device period: always 0 when the device is shared
                &format,
                NULL);
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        _wrl::ComPtr<IAudioCaptureClient> capture;
        if (const auto hr = client->GetService(
                __uuidof(IAudioCaptureClient), IID_PPV_ARGS_Helper(&capture));
            hr != S_OK)
        {
            [[unlikely]] return { _ec(hr, std::system_category()), input_device{} };
        }

        return { _ec{}, input_device{ device.Get(), client.Get(), capture.Get() } };
    }
} // namespace drako::audio