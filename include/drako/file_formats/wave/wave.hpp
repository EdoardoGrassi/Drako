#pragma once
#ifndef DRAKO_FORMATS_WAVE_HPP
#define DRAKO_FORMATS_WAVE_HPP

#include <array>
#include <span>
#include <string>
#include <vector>

namespace drako::file_formats::wave
{
    enum class waveform_format : std::uint16_t
    {
        pcm       = 0x0001, // Microsoft PCM
        mulaw     = 0x0006,
        alaw      = 0x0007,
        ibm_mulaw = 257,
        ibm_alaw  = 258,
        adpcm     = 259
    };

    struct metadata
    {
        std::uint16_t audio_format; // audio format code (1 = PCM)
        std::uint16_t channels;     // number of channels (1 = Mono, >1 = Stereo)
        std::uint32_t sample_rate;  // number of samples per second (Hz)
        std::uint32_t byte_rate;    // == sample_rate * channels * sample_depth / 8
        std::uint16_t block_align;  // == channels * sample_depth / 8
        std::uint16_t sample_depth; // number of storage bits per sample
    };

    struct content
    {
        metadata               meta;
        std::vector<std::byte> data;
        std::string            info;
    };
    [[nodiscard]] content parse(std::span<const std::byte> data);

} // namespace drako::file_formats::wave

#endif // !DRAKO_FORMATS_WAVE_HPP
