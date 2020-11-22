#include "drako/file_formats/wave/wave.hpp"

#include "drako/core/byte_utils.hpp"
#include "drako/io/memory_stream.hpp"

#include <array>
#include <iostream>
#include <span>
#include <system_error>
#include <vector>

namespace drako::file_formats::wave
{
    // compose RIFF tag code from ASCII string of 4 (+ terminator) chars
    [[nodiscard]] consteval std::uint32_t _make_riff_tag(const char (&s)[5])
    {
        return static_cast<std::uint32_t>(s[0] << 24) |
               static_cast<std::uint32_t>(s[1] << 16) |
               static_cast<std::uint32_t>(s[2] << 8) |
               static_cast<std::uint32_t>(s[3] << 0);
    }

    enum _riff_tag : std::uint32_t
    {
        riff = _make_riff_tag("RIFF"), // "RIFF" chunk tag
        wave = _make_riff_tag("WAVE"), // "wave" chunk tag
        fmt  = _make_riff_tag("fmt "), // "fmt " chunk tag
        data = _make_riff_tag("data"), // "data" chunk tag
        info = _make_riff_tag("INFO")  // "INFO" chunk tag
    };
    static_assert(_riff_tag::riff == 0x52'49'46'46);
    static_assert(_riff_tag::wave == 0x57'41'56'45);
    static_assert(_riff_tag::fmt == 0x66'6d'74'20);
    static_assert(_riff_tag::data == 0x64'61'74'61);
    static_assert(_riff_tag::info == 0x49'4e'46'4f);


    struct _riff_chunk_header
    {
        _riff_chunk_header(const std::byte* bytes) noexcept
            : tag{ from_be_bytes<uint32_t>(bytes) }
            , size{ from_le_bytes<uint32_t>(bytes + 4) }
            , format{ from_be_bytes<uint32_t>(bytes + 8) } {}

        std::uint32_t tag;
        std::uint32_t size;
        std::uint32_t format;
    };
    static_assert(sizeof(_riff_chunk_header) == 12);

    struct _fmt_chunk_data
    {
        _fmt_chunk_data(const std::byte* bytes) noexcept
            : audio_format{ from_le_bytes<uint16_t>(bytes) }
            , channels_count{ from_le_bytes<uint16_t>(bytes + 2) }
            , sample_rate{ from_le_bytes<uint32_t>(bytes + 4) }
            , byte_rate{ from_le_bytes<uint32_t>(bytes + 8) }
            , block_align{ from_le_bytes<uint16_t>(bytes + 12) }
            , sample_depth{ from_le_bytes<uint16_t>(bytes + 14) } {}

        std::uint16_t audio_format;   // audio format code (1 = PCM)
        std::uint16_t channels_count; // number of channels (1 = Mono, 2 = Stereo)
        std::uint32_t sample_rate;    // number of samples per second (Hz)
        std::uint32_t byte_rate;      // == sample_rate * channels * sample_depth / 8
        std::uint16_t block_align;    // == channels * sample_depth / 8
        std::uint16_t sample_depth;   // number of storage bits per sample
    };
    static_assert(sizeof(_fmt_chunk_data) == 16);


    struct waveform_header
    {
        // vvv ---- RIFF header ---- vvv
        uint32_t chunk_tag = 0x52'49'46'46; // RIFF tag, contains ASCII characters "RIFF" in big-endian notation
        uint32_t chunk_size;                // RIFF chunck size [bytes]
        uint32_t format = 0x57'41'56'45;    // WAVE tag, contains ASCII characters "WAVE" in big-endian notation

        // vvv ---- fmt subchunk ---- vvv
        uint32_t fmt_chunk_id   = 0x66'6d'74'20; // FMT tag, contains ASCII characters "fmt " in big-endian notation
        uint32_t fmt_chunk_size = 16;            // Size of the remaining part of the fmt chunk [bytes]
        uint16_t format_tag;                     // Audio format 1=PCM, 6=mulaw, 7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t channels_count;                 // Number of channels 1=Mono 2=Stereo
        uint32_t sample_rate;                    // Sampling frequency [Hz]

        uint32_t avg_byte_rate; // F * M * Nc : average bytes per second

        uint16_t block_align; // 2=16-bit mono, 4=16-bit stereo
        uint16_t bit_depth;   // Number of bits per sample

        // vvv ---- data subchunk ---- vvv
        uint32_t data_chunk_id = 0x64'61'74'61; // FMT tag, contains ASCII characters "data" in big-endian notation
        uint32_t data_chunk_size;               // Sampled data length [bytes]

        // vvv ---- data payload ---- vvv
        // ..............................
    };


    [[nodiscard]] content parse(const std::span<const std::byte> bytes)
    {
        io::memory_stream_reader stream{ bytes }; // TODO: << maybe use this

        std::vector<std::byte> data{};

        auto seek = bytes.data();
        if (std::size(bytes) < sizeof(_riff_chunk_header))
            throw std::runtime_error{ "Invalid .wave file." };

        const _riff_chunk_header file_header{ bytes.data() };
        seek += sizeof(decltype(file_header));

        if (file_header.tag != _riff_tag::riff)
            throw std::runtime_error{ "Invalid 'RIFF' tag of .wave file." };

        // declared size doesn't include the first 8 bytes of the file header
        if (file_header.size != (std::size(bytes) - 8))
            throw std::runtime_error{ "RIFF header: invalid lenght of .wave file." };

        const _fmt_chunk_data fmt{ bytes.data() + sizeof(_riff_chunk_header) };
        seek += sizeof(decltype(fmt));

        const metadata meta{
            .audio_format = fmt.audio_format,
            .channels     = fmt.channels_count,
            .sample_rate  = fmt.sample_rate,
            .byte_rate    = fmt.byte_rate,
            .block_align  = fmt.block_align,
            .sample_depth = fmt.sample_depth,
        };

        while (seek < (bytes.data() + bytes.size_bytes()))
        {
            const _riff_chunk_header h{ seek };
            seek += sizeof(_riff_chunk_header);
            switch (h.tag)
            {
                case _riff_tag::data:
                {
                }

                    // default: ignore unknow tags
            }
        }

        return { .meta = meta, .data = data, .info = {} };
    }
} // namespace drako::file_formats::wave