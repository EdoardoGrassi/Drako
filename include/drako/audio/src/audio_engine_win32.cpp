#include "drako/audio/audio_engine.hpp"

#include "drako/audio/audio_types.hpp"
#include "drako/core/typed_handle.hpp"

#include <cassert>
#include <vector>

namespace drako::audio
{
    using _this = audio_engine;

    [[nodiscard]] std::size_t _this::_find_clip_index(const ClipID h) noexcept
    {
        const auto it = std::find(std::cbegin(_clips.ids), std::cend(_clips.ids), h);
        return std::distance(std::cbegin(_clips.ids), it);
    }

    [[nodiscard]] std::size_t _this::_find_playing_index(const ClipID h) noexcept
    {
        const auto it = std::find(std::cbegin(_playing.ids), std::cend(_playing.ids), h);
        return std::distance(std::cbegin(_playing.ids), it);
    }

    [[nodiscard]] std::size_t _this::_find_free_slot_index(const ClipID h) noexcept
    {
        auto i = 0;
        while (_playing.ids[i] != 0)
            ++i;
        return i;
    }

    _this::audio_engine()
    {
        if (const auto ec = ::XAudio2Create(&_instance, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
            ec != S_OK)
            throw std::system_error{ ec, std::system_category() };

        if (const auto ec = _instance->CreateMasteringVoice(&_master); ec != S_OK)
            throw std::system_error{ ec, std::system_category() };
    }

    _this::~audio_engine()
    {
        for (auto v : _playing.voices)
            if (v != nullptr)
                v->DestroyVoice();
        _master->DestroyVoice();
    }

    void _this::update()
    {
        for (const auto h : _stop_list)
        {
            const auto i = _find_playing_index(h);
            if (const auto ec = _playing.voices[i]->Stop(); ec != S_OK)
                throw std::system_error{ ec, std::system_category() };
            _playing.ids[i] = ClipID{};
        }
        _stop_list.clear();

        for (const auto h : _play_list)
        {
            const auto  i    = _find_clip_index(h);
            const auto& clip = _clips.clips[i];
            const auto  slot = _find_free_slot_index(h);

            const WAVEFORMATEX wave{
                .wFormatTag     = WAVE_FORMAT_PCM,
                .nChannels      = clip.format.channels,
                .nSamplesPerSec = clip.format.sample_rate,
                .wBitsPerSample = clip.format.sample_depth,
                .cbSize         = 0 // no extensions used
            };
            _playing.ids[slot] = h;

            if (const auto ec = _instance->CreateSourceVoice(&_playing.voices[slot], &wave); ec != S_OK)
                throw std::system_error{ ec, std::system_category() };
            const auto voice = _playing.voices[slot];

            const XAUDIO2_BUFFER buffer{
                .Flags      = 0,
                .AudioBytes = static_cast<UINT32>(clip.buffer.size_bytes()),
                .pAudioData = reinterpret_cast<BYTE*>(clip.buffer.data()),
                .PlayBegin  = 0,
                .PlayLength = 0, // magic value for the entire buffer
                .LoopBegin  = 0,
                .LoopLength = 0,
                .LoopCount  = 0, // no looping
                .pContext   = NULL
            };
            if (const auto ec = voice->SubmitSourceBuffer(&buffer); ec != S_OK)
                throw std::system_error{ ec, std::system_category() };

            if (const auto ec = voice->Start(); ec != S_OK)
                throw std::system_error{ ec, std::system_category() };
        }
        _play_list.clear();
    }

    void _this::play(const ClipID h) noexcept
    {
        assert(h);
        _play_list.push_back(h);
    }

    void _this::stop(const ClipID h) noexcept
    {
        assert(h);
        _stop_list.push_back(h);
    }
} // namespace drako::audio