#include <AudioSystem.h>
#include <wav/WaveFile.h>
#include <xaudio2/XAudio2Channel.h>

#include <utils/Effects.h>
#include <utils/Logger.h>
#include <algorithm>

namespace uaudio::xaudio2
{
	XAudio2Channel::XAudio2Channel(AudioSystem& a_AudioSystem) : m_AudioSystem(&a_AudioSystem)
	{
		m_VoiceCallback = XAudio2Callback();
	}

	XAudio2Channel::XAudio2Channel(const XAudio2Channel& rhs) : m_AudioSystem(rhs.m_AudioSystem)
	{
		m_Volume = rhs.m_Volume;
		m_Panning = rhs.m_Panning;
		m_CurrentSound = rhs.m_CurrentSound;
		m_IsPlaying = rhs.m_IsPlaying;
		m_CurrentPos = rhs.m_CurrentPos;
		m_DataSize = rhs.m_DataSize;
		m_Tick = rhs.m_Tick;
		m_Ticks = rhs.m_Ticks;
		SetSound(*m_CurrentSound);
		m_VoiceCallback = rhs.m_VoiceCallback;
	}

	XAudio2Channel::~XAudio2Channel()
	{
		if (m_SourceVoice)
		{
			Stop();
			m_SourceVoice = nullptr;
		}
	}

	XAudio2Channel& XAudio2Channel::operator=(const XAudio2Channel& rhs)
	{
		if (this != &rhs)
		{
			m_AudioSystem = rhs.m_AudioSystem;
			m_Volume = rhs.m_Volume;
			m_Panning = rhs.m_Panning;
			m_CurrentSound = rhs.m_CurrentSound;
			m_IsPlaying = rhs.m_IsPlaying;
			m_CurrentPos = rhs.m_CurrentPos;
			m_DataSize = rhs.m_DataSize;
			m_Tick = rhs.m_Tick;
			m_Ticks = rhs.m_Ticks;
			m_Ticks = rhs.m_Ticks;
			m_SourceVoice = rhs.m_SourceVoice;
			m_VoiceCallback = rhs.m_VoiceCallback;
		}
		return *this;
	}

	/// <summary>
	/// Sets the sound of a channel and starts the playback.
	/// </summary>
	/// <param name="a_Sound">A pointer to a wave file.</param>
	void XAudio2Channel::SetSound(const WaveFile& a_Sound)
	{
		m_CurrentSound = &a_Sound;
		HRESULT hr;
		if (m_SourceVoice == nullptr)
		{
			WAVEFORMATEX wave;

			// Set WAV format default. (what we expect the user to provide).
			wave.wFormatTag = a_Sound.GetWavFormat().fmtChunk.audioFormat;
			wave.nChannels = a_Sound.GetWavFormat().fmtChunk.numChannels;
			wave.nSamplesPerSec = a_Sound.GetWavFormat().fmtChunk.sampleRate; // 44100 hz (should be standard).
			wave.cbSize = 0;
			wave.wBitsPerSample = a_Sound.GetWavFormat().fmtChunk.bitsPerSample;
			wave.nBlockAlign = a_Sound.GetWavFormat().fmtChunk.blockAlign;
			wave.nAvgBytesPerSec = a_Sound.GetWavFormat().fmtChunk.byteRate;
			if (FAILED(hr = m_AudioSystem->GetEngine().CreateSourceVoice(&m_SourceVoice, &wave, 0, 1.0f, &m_VoiceCallback)))
			{
				logger::log_error("<XAudio2> Creating XAudio Source Voice failed.");
				m_IsPlaying = false;
				return;
			}
		}
		if (FAILED(hr = m_SourceVoice->Start(0, 0)))
		{
			logger::log_error("<XAudio2> Starting XAudio Source Voice failed.");
			m_IsPlaying = false;
		}
	}

	/// <summary>
	/// Starts the playback of the channel.
	/// </summary>
	void XAudio2Channel::Play()
	{
		m_IsPlaying = true;
	}

	/// <summary>
	/// Pauses the playback of the channel.
	/// </summary>
	void XAudio2Channel::Pause()
	{
		m_IsPlaying = false;
	}

	/// <summary>
	/// Stops and resets the channel (removing the sound and flushing the buffers).
	/// </summary>
	void XAudio2Channel::Stop()
	{
		// Stop the source voice.
		m_SourceVoice->Stop();
		m_IsPlaying = false;

		// Flush the buffers.
		m_SourceVoice->FlushSourceBuffers();

		m_SourceVoice->DestroyVoice();
		m_SourceVoice = nullptr;
	}

	/// <summary>
	/// Updates the playback of the channel.
	/// </summary>
	void XAudio2Channel::Update()
	{
		if (m_CurrentSound == nullptr)
			return;

		if (m_SourceVoice == nullptr)
			return;
		
		if (!m_IsPlaying)
			return;

		PlayRanged(m_CurrentPos, m_AudioSystem->GetBufferSize());
	}

	void XAudio2Channel::SetPos(uint32_t a_StartPos)
	{
		m_CurrentPos = a_StartPos;
	}

	float XAudio2Channel::GetPos(TIMEUNIT a_TimeUnit) const
	{
		switch (a_TimeUnit)
		{
			case TIMEUNIT::TIMEUNIT_MS:
			{
				const float seconds = static_cast<float>(m_CurrentPos) / static_cast<float>(m_CurrentSound->GetWavFormat().fmtChunk.byteRate);
				const float milliseconds = seconds * 1000;
				return milliseconds;
				break;
			}
			case TIMEUNIT::TIMEUNIT_S:
			{
				const float seconds = static_cast<float>(m_CurrentPos) / static_cast<float>(m_CurrentSound->GetWavFormat().fmtChunk.byteRate);
				return seconds;
				break;
			}
			case TIMEUNIT::TIMEUNIT_POS:
			{
				return static_cast<float>(m_CurrentPos);
				break;
			}
		}

		return 0.0f;
	}

	void XAudio2Channel::PlayRanged(uint32_t a_StartPos, uint32_t a_Size)
	{
		if (m_CurrentSound == nullptr)
			return;

		if (m_SourceVoice == nullptr)
			return;

		// If the sound is done playing, check whether it needs to be repeated or whether it needs to be stopped entirely.
		if (m_CurrentSound->IsEndOfFile(a_StartPos))
		{
			// If the sound is not set to repeat, then stop the channel.
			if (!m_CurrentSound->IsLooping())
			{
				m_IsPlaying = false;
				return;
			}
			a_StartPos = 0;
		}
		XAUDIO2_VOICE_STATE state;
		m_SourceVoice->GetState(&state);
		if (state.BuffersQueued < m_CurrentSound->GetWavFormat().GetBufferSize())
		{
			unsigned char* data;

			// Read the part of the wave file and store it back in the read buffer.
			m_CurrentSound->Read(a_StartPos, a_Size, data);

			// Other effects.
			ApplyEffects(data, a_Size);

			m_DataSize = a_Size;

			// Make sure the new pos is the current pos.
			m_CurrentPos = a_StartPos;

			// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
			m_CurrentPos += a_Size;

			XAUDIO2_BUFFER x_buffer = { 0, 0, nullptr, 0, 0, 0, 0, 0, nullptr };
			x_buffer.AudioBytes = a_Size;		 // Buffer containing audio data.
			x_buffer.pAudioData = data; // Size of the audio buffer in bytes.

			HRESULT hr;
			if (FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&x_buffer)))
				logger::log_error("<XAudio2> Submitting data to XAudio Source Voice failed.");
		}
	}

	/// <summary>
	/// Resets the data position of the channel.
	/// </summary>
	void XAudio2Channel::ResetPos()
	{
		m_CurrentPos = 0;
	}

	/// <summary>
	/// Removes the sound.
	/// </summary>
	void XAudio2Channel::RemoveSound()
	{
		m_CurrentSound = nullptr;
	}

	/// <summary>
	/// Returns the XAudio2 source voice.
	/// </summary>
	/// <returns>The XAudio2 source voice.</returns>
	IXAudio2SourceVoice& XAudio2Channel::GetSourceVoice() const
	{
		return *m_SourceVoice;
	}

	/// <summary>
	/// Returns the XAudio2 voice callback.
	/// </summary>
	/// <returns>The XAudio2 voice callback</returns>
	XAudio2Callback& XAudio2Channel::GetVoiceCallback()
	{
		return m_VoiceCallback;
	}

	/// <summary>
	/// Sets the volume of the channel.
	/// </summary>
	/// <param name="a_Volume">The volume.</param>
	void XAudio2Channel::SetVolume(float a_Volume)
	{
		a_Volume = std::clamp(a_Volume, 0.0f, 1.0f);
		m_Volume = a_Volume;
	}

	/// <summary>
	/// Returns the volume of the channel.
	/// </summary>
	/// <returns>The volume of the channel.</returns>
	float XAudio2Channel::GetVolume() const
	{
		return m_Volume;
	}

	/// <summary>
	/// Sets the panning of the channel.
	/// </summary>
	/// <param name="a_Panning">The panning of the channel.</param>
	void XAudio2Channel::SetPanning(float a_Panning)
	{
		a_Panning = std::clamp(a_Panning, -1.0f, 1.0f);
		m_Panning = a_Panning;
	}

	/// <summary>
	/// Returns the panning of the channel.
	/// </summary>
	/// <returns>The panning of the channel.</returns>
	float XAudio2Channel::GetPanning() const
	{
		return m_Panning;
	}

	/// <summary>
	/// Returns whether or not the channel is playing audio.
	/// </summary>
	/// <returns>Whether or not the channel is playing audio.</returns>
	bool XAudio2Channel::IsPlaying() const
	{
		return m_IsPlaying;
	}

	/// <summary>
	/// Returns whether the channel has a sound.
	/// </summary>
	/// <returns>whether the channel has a sound.</returns>
	bool XAudio2Channel::IsInUse() const
	{
		return m_CurrentSound != nullptr;
	}

	/// <summary>
	/// Applies all the effects.
	/// </summary>
	/// <param name="a_Data">The pcm data that needs to be processed.</param>
	/// <param name="a_Size">The size of the pcm data block.</param>
	/// <returns></returns>
	void XAudio2Channel::ApplyEffects(unsigned char*& a_Data, uint32_t a_Size) const
	{
		// Master volume.
		effects::ChangeVolume(a_Data, a_Size, m_AudioSystem->GetMasterVolume());

		// Channel volume.
		effects::ChangeVolume(a_Data, a_Size, m_Volume);

		// Sound volume (not sure why you would want this but I want it in here damn it)
		effects::ChangeVolume(a_Data, a_Size, m_CurrentSound->GetVolume());

		// Master panning.
		effects::ChangePanning(a_Data, a_Size, m_AudioSystem->GetMasterPanning(), m_CurrentSound->GetWavFormat().fmtChunk.numChannels);

		// Channel panning.
		effects::ChangePanning(a_Data, a_Size, m_Panning, m_CurrentSound->GetWavFormat().fmtChunk.numChannels);
	}

	/// <summary>
	/// Returns the sound.
	/// </summary>
	/// <returns>The sound.</returns>
	const WaveFile& XAudio2Channel::GetSound() const
	{
		return *m_CurrentSound;
	}
}