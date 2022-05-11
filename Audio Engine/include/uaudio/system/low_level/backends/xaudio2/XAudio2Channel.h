#pragma once

#include <xaudio2.h>

#include <uaudio/system/low_level/backends/xaudio2/XAudio2Callback.h>
#include <uaudio/system/high_level/Channel.h>

#include <uaudio/system/low_level/SoundData.h>

namespace uaudio
{
	namespace xaudio2
	{
		class XAudio2Backend;

		class XAudio2Channel : public Channel
		{
		public:
			XAudio2Channel(const XAudio2Channel& rhs);
			~XAudio2Channel() override;

			XAudio2Channel& operator=(const XAudio2Channel& rhs);

			UAUDIO_RESULT SetSound(const WaveFormat& a_Sound, const SoundData& a_SoundData) override;

			IXAudio2SourceVoice& GetSourceVoice() const;
			XAudio2Callback& GetVoiceCallback();

			UAUDIO_RESULT PlayRanged(uint32_t a_StartingPosition, uint32_t a_BufferSize) override;
			UAUDIO_RESULT PlayBuffer(const unsigned char* a_DataBuffer, uint32_t a_Size) override;
		private:
			XAudio2Channel(XAudio2Backend& a_AudioSystem);

			IXAudio2SourceVoice* m_SourceVoice = nullptr;
			XAudio2Callback m_VoiceCallback;
			XAudio2Backend* m_AudioSystem = nullptr;

			UAUDIO_RESULT Update() override;

			friend XAudio2Backend;
		};
	}
}
