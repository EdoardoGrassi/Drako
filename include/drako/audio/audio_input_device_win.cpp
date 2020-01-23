#include "drako/audio/audio_input_device.hpp"

#if !defined(DRAKO_PLT_WIN32)
#error No target platform
#endif

namespace drako::audio
{
    std::tuple<std::error_code, input_device_guid>
        default_input_device() noexcept
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
                input_device_guid{});
        }

        // std::unique_ptr<IMMDevice, decltype(IMMDevice::Release)> device;
        IMMDevice* device;
        if (const auto hr = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &device);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            enumerator->Release();
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted),
                input_device_guid{});
        }

        LPWSTR guid;
        if (const auto hr = device->GetId(&guid);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            enumerator->Release();
            device->Release();
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted),
                input_device_guid{});
        }
        enumerator->Release();
        device->Release();
        return std::tuple(std::error_code{}, input_device_guid(guid));
    }

    std::tuple<std::error_code, std::vector<input_device_guid>>
        query_input_devices() noexcept
    {
        // TODO: end impl

        IMMDeviceEnumerator* enumerator;
         // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            std::exit(EXIT_FAILURE);
        }

        IMMDeviceCollection* collection;
        if (DRAKO_UNLIKELY(enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &collection) != S_OK))
        {
            std::exit(EXIT_FAILURE);
        }

        UINT device_count;
        if (DRAKO_UNLIKELY(collection->GetCount(&device_count) != S_OK))
        {
            // std::make_error_code();
            std::exit(EXIT_FAILURE);
        }

        std::vector<input_device_guid> result(device_count);
        // Iterate on the collection of available devices
        for (auto i = 0u; i < device_count; ++i)
        {
            IMMDevice* device;
            if (collection->Item(i, &device) != S_OK)
                std::exit(EXIT_FAILURE);

            // TODO: get device properties

            device->Release();
        }

        // Release kernel resources
        collection->Release();
        enumerator->Release();

        return std::tuple(std::error_code{}, std::move(result));
    }

    std::tuple<std::error_code, std::vector<input_device_guid>>
        query_input_devices(const stream_format& format) noexcept
    {
        // TODO: end impl

        IMMDeviceEnumerator* enumerator;
         // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            std::exit(EXIT_FAILURE);
        }

        IMMDeviceCollection* collection;
        if (DRAKO_UNLIKELY(enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &collection) != S_OK))
        {
            std::exit(EXIT_FAILURE);
        }

        UINT device_count;
        if (DRAKO_UNLIKELY(collection->GetCount(&device_count) != S_OK))
        {
            // std::make_error_code();
            std::exit(EXIT_FAILURE);
        }

        std::vector<input_device_guid> result(device_count);
        // Iterate on the collection of available devices
        for (auto i = 0u; i < device_count; ++i)
        {
            IMMDevice* device;
            if (collection->Item(i, &device) != S_OK)
                std::exit(EXIT_FAILURE);

            // TODO: get device properties

            device->Release();
        }

        // Release kernel resources
        collection->Release();
        enumerator->Release();

        return std::tuple(std::error_code{}, std::move(result));
    }

    input_device::input_device(IMMDevice* dev, IAudioClient* acl, IAudioCaptureClient* acc) noexcept
        : _device(dev)
        , _audio_client_interface(acl)
        , _audio_capture_interface(acc)
    {
        DRAKO_ASSERT(_device != nullptr);
        DRAKO_ASSERT(_audio_client_interface != nullptr);
        DRAKO_ASSERT(_audio_capture_interface != nullptr);
    }

    input_device::input_device(input_device&& other) noexcept
        : _device(other._device)
        , _audio_client_interface(other._audio_client_interface)
        , _audio_capture_interface(other._audio_capture_interface)
    {
        other._device = nullptr;
        other._audio_client_interface = nullptr;
        other._audio_capture_interface = nullptr;
    }

    input_device& input_device::operator=(input_device&& other) noexcept
    {
        if (this != &other)
        {
            _device = other._device;
            _audio_client_interface = other._audio_client_interface;
            _audio_capture_interface = other._audio_capture_interface;
            other._device = nullptr;
            other._audio_client_interface = nullptr;
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

    std::error_code input_device::enable() noexcept
    {
        return (_audio_client_interface->Start() == S_OK) ?
            std::error_code{} :
            std::make_error_code(std::errc::operation_not_permitted);
    }

    std::error_code input_device::disable() noexcept
    {
        auto hr = _audio_client_interface->Stop();
        return (hr == S_OK || hr == S_FALSE) ?
            std::error_code{} :
            std::make_error_code(std::errc::operation_not_permitted);
    }

    std::tuple<std::error_code, size_t>
        input_device::read(void* dst_, size_t frames_) noexcept
    {
        // TODO: end impl

        // TODO: vvv --- maybe remove the next section as its redundant --- vvv
        UINT32 frame_count;
        if (const auto hr = _audio_capture_interface->GetNextPacketSize(&frame_count);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), 0);
        }
        // ^^^ ------------------------------------------------------------ ^^^

        BYTE* buffer_address;
        DWORD buffer_status;
        if (const auto hr = _audio_capture_interface->GetBuffer(&buffer_address, &frame_count, &buffer_status, NULL, NULL);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            std::exit(EXIT_FAILURE);
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), 0);
        }

        memcpy(dst_, buffer_address, frames_);

        if (const auto hr = _audio_capture_interface->ReleaseBuffer(frame_count);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            std::exit(EXIT_FAILURE);
        }

        return std::tuple(std::error_code{}, frame_count);
    }

    std::tuple<std::error_code, input_device>
        open_device(input_device_guid device_, std::chrono::microseconds buffer_capacity_) noexcept
    {
        IMMDeviceEnumerator* enumerator;
         // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        IMMDevice* audio_device;
        if (const auto hr = enumerator->GetDevice(device_.native_guid(), &audio_device);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            enumerator->Release();
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::no_such_device), input_device{});
        }
        enumerator->Release();

        IAudioClient* audio_client;
        if (const auto hr = audio_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
            NULL, IID_PPV_ARGS_Helper(&audio_client));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        WAVEFORMATEX* format;
        if (const auto hr = audio_client->GetMixFormat(&format);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        if (const auto hr = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED,
            std::chrono::duration_cast<std::chrono::nanoseconds>(buffer_capacity_).count() / 100, // duration time expressed in 100 ns units
            0,  // device period: always 0 when the device is shared
            format,
            NULL);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        IAudioCaptureClient* audio_capture;
        if (const auto hr = audio_client->GetService(__uuidof(IAudioCaptureClient), IID_PPV_ARGS_Helper(&audio_capture));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        return std::tuple(std::error_code{}, input_device(audio_device, audio_client, audio_capture));
    }

    std::tuple<std::error_code, input_device>
        open_device(input_device_guid device_, const stream_format& format_, std::chrono::microseconds buffer_capacity_) noexcept
    {
        IMMDeviceEnumerator* enumerator;
         // TODO: change CLSCTX_ALL flag, some options aren't needed
        if (const auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        IMMDevice* audio_device;
        if (const auto hr = enumerator->GetDevice(device_.native_guid(), &audio_device);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            enumerator->Release();
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::no_such_device), input_device{});
        }
        enumerator->Release();

        IAudioClient* audio_client;
        if (const auto hr = audio_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, IID_PPV_ARGS_Helper(&audio_client));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        WAVEFORMATEX format{};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = format_.channels_count();
        format.nSamplesPerSec = format_.sample_rate();
        format.wBitsPerSample = format_.sample_bitdepth();
        format.cbSize = 0;
        format.nBlockAlign = (format.wBitsPerSample * format.nChannels) / 8;
        format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;

        if (const auto hr = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED,
            std::chrono::duration_cast<std::chrono::nanoseconds>(buffer_capacity_).count() / 100, // duration time expressed in 100 ns units
            0,  // device period: always 0 when the device is shared
            &format,
            NULL);
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        IAudioCaptureClient* audio_capture;
        if (const auto hr = audio_client->GetService(__uuidof(IAudioCaptureClient), IID_PPV_ARGS_Helper(&audio_capture));
            DRAKO_UNLIKELY(hr != S_OK))
        {
            DRAKO_LOG_ERROR(std::system_category().message(hr));
            return std::tuple(std::make_error_code(std::errc::operation_not_permitted), input_device{});
        }

        return std::tuple(std::error_code{}, input_device(audio_device, audio_client, audio_capture));
    }
}