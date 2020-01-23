#pragma once
#ifndef DRAKO_WAVE_FORMAT_API_HPP
#define DRAKO_WAVE_FORMAT_API_HPP

#include <cstdint>
#include <iostream>

#include "drako/audio/audio_input_device.hpp"

namespace drako::audio
{
    enum class waveform_format_tag : uint16_t
    {
        DK_WAVE_FORMAT_PCM = 0x0001,
        DK_WAVE_FORMAT_MULAW = 0x0006,
        DK_WAVE_FORMAT_ALAW = 0x0007,
        DK_WAVE_FORMAT_IBM_MULAW = 257,
        DK_WAVE_FORAMT_IBM_ALAW = 258,
        DK_WAVE_FORMAT_ADPCM = 259
    };

    struct waveform_header
    {
        // vvv ---- RIFF header ---- vvv
        uint32_t    chunk_tag = 0x52'49'46'46;  // RIFF tag, contains ASCII characters "RIFF" in big-endian notation
        uint32_t    chunk_size;                 // RIFF chunck size [bytes]
        uint32_t    format = 0x57'41'56'45;     // WAVE tag, contains ASCII characters "WAVE" in big-endian notation

        // vvv ---- fmt subchunk ---- vvv
        uint32_t    fmt_chunk_id = 0x66'6d'74'20;       // FMT tag, contains ASCII characters "fmt " in big-endian notation    
        uint32_t    fmt_chunk_size = 16;                // Size of the remaining part of the fmt chunk [bytes]                               
        uint16_t    format_tag;                         // Audio format 1=PCM, 6=mulaw, 7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
        uint16_t    channels_count;                     // Number of channels 1=Mono 2=Stereo
        uint32_t    sample_rate;                        // Sampling frequency [Hz]                   

        uint32_t    avg_byte_rate;                      // F * M * Nc : average bytes per second 

        uint16_t    blockAlign;                         // 2=16-bit mono, 4=16-bit stereo 
        uint16_t    bit_depth;                          // Number of bits per sample

        // vvv ---- data subchunk ---- vvv
        uint32_t    data_chunk_id = 0x64'61'74'61;  // FMT tag, contains ASCII characters "data" in big-endian notation  
        uint32_t    data_chunk_size;                // Sampled data length [bytes]

        // vvv ---- data payload ---- vvv
        // ..............................

        constexpr explicit waveform_header(
            const stream_format& format,
            size_t payload_bytes) noexcept
            : chunk_size()
            , channels_count(format.channels_count)
            , sample_rate(format.sample_rate)
            , avg_byte_rate()
        {
        }
    };


    template<typename Alloc>
    class waveform_builder
    {
    public:

        waveform_builder(const stream_format& format, Alloc& allocator) noexcept
            : _format(format)
            , _allocator(allocator)
        {
        }

        void push_data(const stream_format& format, void* data, size_t bytes) noexcept;

        friend std::ostream& operator<<(std::ostream& os, const waveform_builder& builder) noexcept;

    private:

        audio_stream_format _format;
        Alloc& _allocator;
        void* _data;
        size_t _size;
    };


    template<typename Alloc>
    std::ostream& operator<<(std::ostream& os, const waveform_builder<Alloc>& builder) noexcept
    {
        waveform_header header{};

        os.write(static_cast<char*>(&header), sizeof(decltype(header));
        os.write(static_cast<char*>(builder._data, _size);
        return os;
    }
}

#endif // !DRAKO_WAVE_FORMAT_API_HPP
