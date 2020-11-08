#include "drako/file_formats/wave/wave.hpp"

#include "drako/core/builtins/byte_utils.hpp"

#include <array>
#include <span>
#include <system_error>
#include <vector>

namespace drako::file_formats::wave
{
    constexpr const std::uint32_t riff = 0x52'49'46'46; // (ASCII) "RIFF" tag
    constexpr const std::uint32_t wave = 0x57'41'56'45; // (ASCII) "wave" tag
    constexpr const std::uint32_t fmt  = 0x66'6d'74'20; // (ASCII) "fmt " tag
    constexpr const std::uint32_t data = 0x64'61'74'61; // (ASCII) "data" tag

    constexpr const std::array<char, 4> riff_tag_id = { 'R', 'I', 'F', 'F' };
    constexpr const std::array<char, 4> fmt_tag_id  = { 'f', 'm', 't', ' ' };
    constexpr const std::array<char, 4> data_tag_id = { 'd', 'a', 't', 'a' };

    struct _riff_chunk_header
    {
        _riff_chunk_header(const std::byte* bytes) noexcept
            : tag{ from_little_endian<uint32_t>(bytes) }
            , size{ from_little_endian<uint32_t>(bytes + sizeof(tag)) } {}

        std::uint32_t tag;
        std::uint32_t size;
    };

    struct _fmt_chunk_data
    {
        _fmt_chunk_data(const std::byte* bytes) noexcept
            : audio_format{ from_little_endian<uint32_t>(bytes) }
            , sample_rate{ from_little_endian<uint32_t>(bytes) }
            , byte_rate{ from_little_endian<uint32_t>(bytes) }
            , block_align{ from_little_endian<uint16_t>(bytes) }
            , sample_depth{ from_little_endian<uint16_t>(bytes) }
            , channels_count{ from_little_endian<uint16_t>(bytes) } {}

        std::uint32_t audio_format;   // audio format code (1 = PCM)
        std::uint32_t sample_rate;    // number of samples per second (Hz)
        std::uint32_t byte_rate;      // == sample_rate * channels * sample_depth / 8
        std::uint16_t block_align;    // == channels * sample_depth / 8
        std::uint16_t sample_depth;   // number of storage bits per sample
        std::uint16_t channels_count; // number of channels (1 = Mono, 2 = Stereo)
    };

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


    [[nodiscard]] content parse(const std::span<std::byte> data)
    {
        audio::format          format;
        std::vector<std::byte> bytes{};

        if (std::size(data) < sizeof(_riff_chunk_header))
            throw std::runtime_error{ "Invalid .wave file." };

        const _riff_chunk_header header{ data.data() };
        if (header.tag != riff)
            throw std::runtime_error{ "Invalid 'RIFF' tag of .wave file." };

        if (header.size != std::size(data))
            throw std::runtime_error{ "Invalid lenght of .wave file." };

        const auto seek = data.data() + sizeof(header);
        while (true)
        {
            const _riff_chunk_header h{ seek };
            if (h.tag == fmt)
            {
                const _fmt_chunk_data fmt{ seek + sizeof(_riff_chunk_header) };
                format.channels = fmt.channels_count;
                format.sample_depth = fmt.sample_depth;
                format.sample_rate  = fmt.sample_rate;
            }
        }

        return { .format = format, .data = bytes };
    }
} // namespace drako::file_formats::wave