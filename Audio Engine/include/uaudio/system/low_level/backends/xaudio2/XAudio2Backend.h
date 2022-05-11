#pragma once

#include <xaudio2.h>
#include <uaudio/system/low_level/AudioBackend.h>
#include <uaudio/system/low_level/backends/xaudio2/XAudio2Channel.h>

namespace uaudio::xaudio2
{
	class XAudio2Backend : public AudioBackend
	{
	public:
		XAudio2Backend();
		~XAudio2Backend() override;

		IXAudio2* GetEngine() const;

		int32_t CreateSoundChannel(SoundData& a_SoundData) override;
		int32_t CreateSoundChannel() override;

		void Update() override;
	private:
		Channel* GetChannel(int32_t a_Index) override;

		IXAudio2* m_Engine = nullptr;
		IXAudio2MasteringVoice* m_MasterVoice = nullptr;

		utils::uaudio_vector<xaudio2::XAudio2Channel> m_Channels;
	};
}
