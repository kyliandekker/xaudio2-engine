#pragma once

#include <queue>
#include <xaudio2.h>

#include "XAudio2Callback.h"
#include "Includes.h"

namespace uaudio
{
	class WaveFile;
	class AudioSystem;

	namespace xaudio2
	{
		class XAudio2Channel
		{
		public:
			XAudio2Channel() = default;
			XAudio2Channel(AudioSystem& a_AudioSystem);
			XAudio2Channel(const XAudio2Channel& rhs);

			~XAudio2Channel();

			XAudio2Channel& operator=(const XAudio2Channel& rhs);

			void SetSound(const WaveFile& a_Sound);

			void SetActive(bool a_Active);
			bool GetActive() const;
			void Play();
			void Pause();
			void Stop();
			void Update();
			void SetPos(uint32_t a_StartPos);
			float GetPos(TIMEUNIT a_TimeUnit) const;
			void PlayRanged(uint32_t a_StartPos, uint32_t a_Size);
			void ResetPos();
			void RemoveSound();

			IXAudio2SourceVoice& GetSourceVoice() const;
			XAudio2Callback& GetVoiceCallback();

			void SetVolume(float a_Volume);
			float GetVolume() const;

			void SetPanning(float a_Panning);
			float GetPanning() const;

			bool IsPlaying() const;
			bool IsInUse() const;

			bool IsLooping() const;
			void SetLooping(bool a_Looping);

			void ApplyEffects(unsigned char*& a_Data, uint32_t a_BufferSize) const;

			const WaveFile& GetSound() const;
		private:
			bool m_Looping = false;

			std::queue<unsigned char*> m_Buffers;
			float m_Volume = 1;
			float m_Panning = 0.0f;

			const WaveFile* m_CurrentSound = nullptr;

			bool m_IsPlaying = false, m_Active = true;

			AudioSystem* m_AudioSystem = nullptr;

			uint32_t m_CurrentPos = 0;

			IXAudio2SourceVoice* m_SourceVoice = nullptr;
			XAudio2Callback m_VoiceCallback;
		};
	}
}