#pragma once
#ifndef DRAKO_FORMATS_WAVE_HPP
#define DRAKO_FORMATS_WAVE_HPP

#include "drako/audio/audio_types.hpp"
#include "drako/core/builtins/byte_utils.hpp"

#include <array>
#include <span>
#include <vector>

namespace drako::file_formats::wave
{
    enum class waveform_format : std::uint16_t
    {
        pcm       = 0x0001,
        mulaw     = 0x0006,
        alaw      = 0x0007,
        ibm_mulaw = 257,
        ibm_alaw  = 258,
        adpcm     = 259
    };

    struct content
    {
        audio::format          format;
        std::vector<std::byte> data;
    };
    [[nodiscard]] content parse(const std::span<std::byte> data);

} // namespace drako::file_formats::wave

#endif // !DRAKO_FORMATS_WAVE_HPP
