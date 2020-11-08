#pragma once
#ifndef DRAKO_AUDIO_ENGINE_HPP
#define DRAKO_AUDIO_ENGINE_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/core/typed_handle.hpp"
#include "drako/audio/audio_types.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>

#include <wrl/client.h> // Microsoft::WRL::ComPtr
#include <xaudio2.h>

namespace _wrl = Microsoft::WRL;
#endif

#include <array>
#include <span>
#include <system_error>
#include <vector>

namespace drako::audio
{
    class audio_engine
    {
    public:
        explicit audio_engine();

        ~audio_engine() noexcept;

        void update();

        void play(const handle<clip>) noexcept;

        void stop(const handle<clip>) noexcept;

    private:
        _wrl::ComPtr<IXAudio2>  _instance;
        IXAudio2MasteringVoice* _master;
        IXAudio2SourceVoice*    _source;

        std::vector<handle<clip>> _play_list;
        std::vector<handle<clip>> _stop_list;

        struct _clip_table_t
        {
            std::vector<handle<clip>> ids;
            std::vector<clip>         clips;
        } _clips;

        struct _playing_table_t
        {
            std::array<IXAudio2SourceVoice*, 100> voices = {};
            std::array<handle<clip>, 100>         ids    = {};
        } _playing;

        [[nodiscard]] std::size_t _find_clip_index(const handle<clip>) noexcept;
        [[nodiscard]] std::size_t _find_playing_index(const handle<clip>) noexcept;
        [[nodiscard]] std::size_t _find_free_slot_index(const handle<clip>) noexcept;
    };


} // namespace drako::audio

#endif // !DRAKO_AUDIO_ENGINE_HPP