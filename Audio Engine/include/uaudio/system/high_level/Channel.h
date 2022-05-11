#pragma once

#include <queue>
#include <uaudio/Includes.h>

#include <uaudio/wave/low_level/WaveFormat.h>
#include <uaudio/system/low_level/SoundData.h>

namespace uaudio
{
	class AudioBackend;
	class AudioSystem;
	
	class Channel
	{
	public:
		virtual ~Channel()
		{
			EmptyBuffers();
		}

		Channel(const Channel& rhs);

		Channel& operator=(const Channel& rhs);

		virtual UAUDIO_RESULT ResetPos();
		virtual UAUDIO_RESULT PlayRanged(uint32_t a_StartingPosition, uint32_t a_BufferSize);

		virtual UAUDIO_RESULT PlayBuffer(const unsigned char*, uint32_t) { return UAUDIO_RESULT::UAUDIO_OK;  }

		UAUDIO_RESULT SetPosition(uint32_t a_StartPos);
		UAUDIO_RESULT GetPosition(uint32_t& a_Position, TIMEUNIT a_TimeUnit) const;

		UAUDIO_RESULT GetSound(const WaveFormat*& a_WaveFormat) const;
		UAUDIO_RESULT RemoveSound();

		UAUDIO_RESULT SetPlayback(bool a_Playback);
		UAUDIO_RESULT GetPlayback(bool& a_Playback) const;

		UAUDIO_RESULT IsInUse(bool& a_IsInUse) const;

		virtual UAUDIO_RESULT SetSound(const WaveFormat& a_Sound, const SoundData& a_SoundData);

		UAUDIO_RESULT Play() { m_IsPlaying = true; return UAUDIO_RESULT::UAUDIO_OK; }
		UAUDIO_RESULT Pause() { m_IsPlaying = false; return UAUDIO_RESULT::UAUDIO_OK; }

		UAUDIO_RESULT SetVolume(float a_Volume) { m_Volume = a_Volume; return UAUDIO_RESULT::UAUDIO_OK; }
		UAUDIO_RESULT GetVolume(float& a_Volume) const { a_Volume = m_Volume; return UAUDIO_RESULT::UAUDIO_OK; }

		UAUDIO_RESULT SetPanning(float a_Panning) { m_Panning = a_Panning; return UAUDIO_RESULT::UAUDIO_OK; }
		UAUDIO_RESULT GetPanning(float& a_Panning) const { a_Panning = m_Panning; return UAUDIO_RESULT::UAUDIO_OK; }

		UAUDIO_RESULT SetLooping(bool a_Looping) { m_Looping = a_Looping; return UAUDIO_RESULT::UAUDIO_OK; }
		UAUDIO_RESULT IsLooping(bool& a_Looping) const { a_Looping = m_Looping; return UAUDIO_RESULT::UAUDIO_OK; }

		UAUDIO_RESULT IsPlaying(bool& a_Playing) const { a_Playing = m_IsPlaying; return UAUDIO_RESULT::UAUDIO_OK; }
	protected:
		Channel() = default;

		uint32_t GetWaveBufferSize() const;

		float m_Volume = UAUDIO_DEFAULT_VOLUME;
		float m_Panning = UAUDIO_DEFAULT_PANNING;
		bool m_Looping = false;
		bool m_Playback = true;
		bool m_IsPlaying = true;

		// TODO: Check if this allocates differently.
		std::queue<unsigned char*> m_DataBuffers;

		uint32_t m_CurrentPosition = 0;

		const WaveFormat* m_CurrentSound = nullptr;
		const SoundData* m_CurrentSoundData = nullptr;

		virtual UAUDIO_RESULT Update();

		void EmptyBuffers();
		virtual void Stop();

		void ApplyEffects(unsigned char* a_DataBuffer, uint32_t a_BufferSize, AudioBackend* AudioBackend) const;
		unsigned char* GetBufferPart(uint32_t& a_StartingPosition, uint32_t& a_BufferSize);

		friend AudioBackend;
		friend AudioSystem;
	};
}
