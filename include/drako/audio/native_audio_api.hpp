#pragma once
#ifndef DRAKO_NATIVE_AUDIO_API_HPP
#define DRAKO_NATIVE_AUDIO_API_HPP

#include "drako/core/compiler.hpp"
#include "drako/core/platform.hpp"
#include "drako/devel/logging.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Audioclient.h>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#endif

#include <chrono>
#include <memory>
#include <system_error>
#include <tuple>
#include <vector>

namespace drako::audio
{
    namespace
    {
        // Performs initialization of COM library, required on Windows platform.
        //
        class _audio_env_initializer final
        {
        public:
            _audio_env_initializer() noexcept
            {
                if (::CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY) == S_OK)
                    DRAKO_LOG_INFO("Initialization of COM library.");
                else
                    DRAKO_LOG_WARNING("Multiple initializations of COM library.");
            }
        };

        _audio_env_initializer _runtime_initializer;
    } // namespace

    enum channel_mode : uint8_t
    {
        mono   = 1, // 1-channel mode
        stereo = 2  // 2-channels mode
    };

    enum sample_frequency : uint32_t
    {
        hz_8000  = 8'000,
        hz_16000 = 16'000,
        hz_32000 = 32'000,
        hz_44100 = 44'100,
        hz_48000 = 48'000
    };

    enum sample_bitdepth : uint8_t
    {
        int_8bit,   // 8-bit integer
        int_16bit,  // 16-bit integer
        int_24bit,  // 24-bit integer
        int_32bit,  // 32-bit integer
        float_32bit // 32-bit floating point
    };

    class stream_format
    {
    public:
        explicit constexpr stream_format(uint32_t sample_rate, uint8_t sample_bitdepth, uint8_t channels) noexcept
            : _sample_rate(sample_rate)
            , _sample_bitdepth(sample_bitdepth)
            , _channels(channels)
        {
        }

        [[nodiscard]] DRAKO_FORCE_INLINE constexpr std::uint8_t
        channels_count() const noexcept { return _channels; }

        [[nodiscard]] DRAKO_FORCE_INLINE constexpr std::uint8_t
        sample_bitdepth() const noexcept { return _sample_bitdepth; }

        [[nodiscard]] DRAKO_FORCE_INLINE constexpr std::uint32_t
        sample_rate() const noexcept { return _sample_rate; }

    private:
        uint32_t _sample_rate;     // samples per seconds (hz)
        uint8_t  _sample_bitdepth; // bits per sample
        uint8_t  _channels;        // mono = 1, stereo = 2
    };
} // namespace drako::audio

#endif // !DRAKO_NATIVE_AUDIO_API_HPP
