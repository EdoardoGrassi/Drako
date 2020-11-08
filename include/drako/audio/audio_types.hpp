#pragma once
#ifndef DRAKO_AUDIO_TYPES_HPP
#define DRAKO_AUDIO_TYPES_HPP

#include "drako/core/intrinsics.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace drako::audio
{
    struct format
    {
        std::uint32_t sample_rate;
        std::uint16_t sample_depth;
        std::uint16_t channels;
    };

    struct clip
    {
        clip(const std::span<std::byte> bytes)
        {
            std::memcpy(reinterpret_cast<std::byte*>(&format), bytes.data(), sizeof(decltype(format)));
            buffer = bytes.subspan(sizeof(decltype(format)));
        }

        format               format; // format of the audio data
        std::span<std::byte> buffer; // raw audio data
    };

    [[nodiscard]] std::vector<std::byte> as_bytes(const clip& c)
    {
        std::vector<std::byte> bytes;
        bytes.resize(sizeof(clip::format) + c.buffer.size_bytes());
        const auto f = reinterpret_cast<const std::byte*>(&c.format);
        for (auto i = 0; i < sizeof(c.format); ++i)
            bytes[i] = f[i];

        for (auto i = 0; i < c.buffer.size_bytes(); ++i)
            bytes[i + sizeof(c.format)] = c.buffer[i];

        return bytes;
    }

} // namespace drako::audio

#endif // !DRAKO_AUDIO_TYPES_HPP