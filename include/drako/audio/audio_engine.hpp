#pragma once
#ifndef DRAKO_AUDIO_ENGINE_HPP
#define DRAKO_AUDIO_ENGINE_HPP

#include "drako/audio/audio_types.hpp"
#include "drako/core/platform.hpp"
#include "drako/core/typed_handle.hpp"

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
    DRAKO_DEFINE_TYPED_ID(ClipID, std::uint32_t);

    class audio_engine
    {
    public:
        explicit audio_engine();

        ~audio_engine() noexcept;

        void update();

        void play(const ClipID) noexcept;

        void stop(const ClipID) noexcept;

    private:
        _wrl::ComPtr<IXAudio2>  _instance;
        IXAudio2MasteringVoice* _master;

        std::vector<ClipID> _play_list;
        std::vector<ClipID> _stop_list;

        struct _clip_table_t
        {
            std::vector<ClipID> ids;
            std::vector<clip>   clips;
        } _clips;

        struct _playing_table_t
        {
            std::array<IXAudio2SourceVoice*, 100> voices = {};
            std::array<ClipID, 100>               ids    = {};
        } _playing;

        [[nodiscard]] std::size_t _find_clip_index(const ClipID) noexcept;
        [[nodiscard]] std::size_t _find_playing_index(const ClipID) noexcept;
        [[nodiscard]] std::size_t _find_free_slot_index(const ClipID) noexcept;
    };


} // namespace drako::audio

#endif // !DRAKO_AUDIO_ENGINE_HPP