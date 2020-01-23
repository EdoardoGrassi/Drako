#pragma once
#ifndef DRAKO_NATIVE_AUDIO_API_HPP
#define DRAKO_NATIVE_AUDIO_API_HPP

#include <chrono>
#include <cstdlib>
#include <memory>
#include <system_error>
#include <tuple>
#include <vector>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#if defined(DRAKO_PLT_WIN32)
#include "drako/core/drako_api_win.hpp"
#include <Audioclient.h>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#endif

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
                if (CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY) == S_OK)
                    DRAKO_LOG_INFO("Initialization of COM library.");
                else
                    DRAKO_LOG_WARNING("Multiple initializations of COM library.");
            }
        };

        _audio_env_initializer _runtime_initializer;
    }

    enum channel_mode : uint8_t
    {
        DK_MONO = 1,    // 1-channel mode
        DK_STEREO = 2   // 2-channels mode
    };

    enum sample_frequency : uint32_t
    {
        DK_8000_HZ = 8'000,
        DK_16000_HZ = 16'000,
        DK_32000_HZ = 32'000,
        DK_44100_HZ = 44'100,
        DK_48000_HZ = 48'000
    };

    enum sample_bitdepth : uint8_t
    {
        DK_8BIT_INT,    // 8-bit integer
        DK_16BIT_INT,   // 16-bit integer
        DK_24BIT_INT,   // 24-bit integer
        DK_32BIT_INT,   // 32-bit integer
        DK_32BIT_FLT    // 32-bit floating point
    };

    struct stream_format
    {
    public:

        explicit constexpr stream_format(uint32_t sample_rate, uint8_t sample_bitdepth, uint8_t channels) noexcept
            : _sample_rate(sample_rate)
            , _sample_bitdepth(sample_bitdepth)
            , _channels(channels)
        {
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            uint8_t channels_count() const noexcept { return _channels; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            uint8_t sample_bitdepth() const noexcept { return _sample_bitdepth; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            uint32_t sample_rate() const noexcept { return _sample_rate; }

    private:

        uint32_t    _sample_rate;       // samples per seconds (hz)
        uint8_t     _sample_bitdepth;   // bits per sample
        uint8_t     _channels;          // mono = 1, stereo = 2
    };
}

#endif // !DRAKO_NATIVE_AUDIO_API_HPP
