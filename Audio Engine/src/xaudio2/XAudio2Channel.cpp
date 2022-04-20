#include <AudioSystem.h>
#include <wave/WaveFile.h>
#include <xaudio2/XAudio2Channel.h>
#include <comdef.h>

#include <wave/WaveEffects.h>
#include <utils/Logger.h>
#include <algorithm>

#include "wave/Chunks.h"
#include "wave/WaveConverter.h"

namespace uaudio::xaudio2
{
	XAudio2Channel::XAudio2Channel(AudioSystem &a_AudioSystem) : m_AudioSystem(&a_AudioSystem)
	{
		m_VoiceCallback = XAudio2Callback();
	}

	XAudio2Channel::XAudio2Channel(const XAudio2Channel &rhs) : m_AudioSystem(rhs.m_AudioSystem)
	{
		m_Volume = rhs.m_Volume;
		m_Panning = rhs.m_Panning;
		m_CurrentSound = rhs.m_CurrentSound;
		m_IsPlaying = rhs.m_IsPlaying;
		m_CurrentPos = rhs.m_CurrentPos;
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

	XAudio2Channel &XAudio2Channel::operator=(const XAudio2Channel &rhs)
	{
		if (this != &rhs)
		{
			m_AudioSystem = rhs.m_AudioSystem;
			m_Volume = rhs.m_Volume;
			m_Panning = rhs.m_Panning;
			m_CurrentSound = rhs.m_CurrentSound;
			m_IsPlaying = rhs.m_IsPlaying;
			m_CurrentPos = rhs.m_CurrentPos;
			m_SourceVoice = rhs.m_SourceVoice;
			m_VoiceCallback = rhs.m_VoiceCallback;
		}
		return *this;
	}

	/// <summary>
	/// Sets the sound of a channel and starts the playback.
	/// </summary>
	/// <param name="a_Sound">A pointer to a wave file.</param>
	void XAudio2Channel::SetSound(const WaveFile &a_Sound)
	{
		m_CurrentSound = &a_Sound;
		if (m_CurrentSound->IsLooping())
			m_Looping = m_CurrentSound->IsLooping();

		m_CurrentPos = a_Sound.GetStartPosition();
		HRESULT hr;
		if (m_SourceVoice == nullptr)
		{
			WAVEFORMATEX wave;

			FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
			// Set WAV format default. (what we expect the user to provide).
			wave.wFormatTag = fmt_chunk.audioFormat;
			wave.nChannels = fmt_chunk.numChannels;
			wave.nSamplesPerSec = fmt_chunk.sampleRate; // 44100 hz (should be standard).
			wave.cbSize = 0;
			wave.wBitsPerSample = fmt_chunk.bitsPerSample;
			wave.nBlockAlign = fmt_chunk.blockAlign;
			wave.nAvgBytesPerSec = fmt_chunk.byteRate;
			if (FAILED(hr = m_AudioSystem->GetEngine().CreateSourceVoice(&m_SourceVoice, &wave, 0, 1.0f, &m_VoiceCallback)))
			{
				logger::ASSERT(false, "<XAudio2> Creating XAudio2 Source Voice failed.");
				m_IsPlaying = false;
				return;
			}
		}
		if (FAILED(hr = m_SourceVoice->Start(0, 0)))
		{
			logger::ASSERT(false, "<XAudio2> Starting XAudio2 Source Voice failed.");
			m_IsPlaying = false;
		}
	}

	/// <summary>
	/// Sets whether the channel submits sound.
	/// </summary>
	/// <param name="a_Active"></param>
	void XAudio2Channel::SetActive(bool a_Active)
	{
		m_Active = a_Active;
	}

	/// <summary>
	/// Returns whether the channel is currently submitting sound.
	/// </summary>
	/// <returns></returns>
	bool XAudio2Channel::GetActive() const
	{
		return m_Active;
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

		m_CurrentPos = m_CurrentSound->GetStartPosition();
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

		PlayRanged(m_CurrentPos, static_cast<int>(m_AudioSystem->GetBufferSize()));
	}

	void XAudio2Channel::SetPos(uint32_t a_StartPos)
	{
		m_CurrentPos = a_StartPos;
	}

	float XAudio2Channel::GetPos(TIMEUNIT a_TimeUnit) const
	{
		FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
		switch (a_TimeUnit)
		{
			case TIMEUNIT::TIMEUNIT_MS:
			{
				return conversion::PosToMilliseconds(m_CurrentPos, fmt_chunk.byteRate);
				break;
			}
			case TIMEUNIT::TIMEUNIT_S:
			{
				return conversion::PosToSeconds(m_CurrentPos, fmt_chunk.byteRate);
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

	uint32_t XAudio2Channel::GetBufferSize() const
	{
		FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
		return fmt_chunk.bitsPerSample / 8;
	}

	void XAudio2Channel::PlayRanged(uint32_t a_StartPos, uint32_t a_Size)
	{
		if (m_CurrentSound == nullptr)
			return;

		if (m_SourceVoice == nullptr)
			return;

		XAUDIO2_VOICE_STATE state;
		m_SourceVoice->GetState(&state);
		if (state.BuffersQueued < GetBufferSize())
		{
			// If the sound is done playing, check whether it needs to be repeated or whether it needs to be stopped entirely.
			if (m_CurrentSound->IsEndOfFile(a_StartPos))
			{
				m_CurrentPos = m_CurrentSound->GetStartPosition();
				a_StartPos = m_CurrentSound->GetStartPosition();
				// If the sound is not set to repeat, then stop the channel.
				if (!m_CurrentSound->IsLooping() && !m_Looping)
				{
					Stop();
					RemoveSound();
					return;
				}
			}

			unsigned char *data = {};

			// Read the part of the wave file and store it back in the read buffer.
			m_CurrentSound->Read(a_StartPos, a_Size, data);

			// Other effects.
			ApplyEffects(data, a_Size);

			// Make sure the new pos is the current pos.
			m_CurrentPos = a_StartPos;

			// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
			m_CurrentPos += a_Size;

			if (!m_Active)
			{
				FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
				effects::ChangeVolume<int16_t>(data, a_Size, 0.0f, fmt_chunk.blockAlign, fmt_chunk.numChannels);
			}

			PlayBuffer(data, a_Size);
		}
	}

	void XAudio2Channel::PlayBuffer(unsigned char* a_Buffer, uint32_t a_Size)
	{
		XAUDIO2_BUFFER x_buffer = { 0, 0, nullptr, 0, 0, 0, 0, 0, nullptr };
		x_buffer.AudioBytes = a_Size; // Buffer containing audio data.
		x_buffer.pAudioData = a_Buffer;	  // Size of the audio buffer in bytes.

		HRESULT hr;
		if (FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&x_buffer)))
		{
			_com_error err(hr);
			LPCTSTR errMsg = err.ErrorMessage();
			logger::log_warning("<XAudio2> Submitting data to XAudio2 Source Voice failed: 0x%08x: %s.", hr, errMsg);
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
	IXAudio2SourceVoice &XAudio2Channel::GetSourceVoice() const
	{
		return *m_SourceVoice;
	}

	/// <summary>
	/// Returns the XAudio2 voice callback.
	/// </summary>
	/// <returns>The XAudio2 voice callback</returns>
	XAudio2Callback &XAudio2Channel::GetVoiceCallback()
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
	/// Returns whether the channel needs to repeat itself.
	/// </summary>
	/// <returns></returns>
	bool XAudio2Channel::IsLooping() const
	{
		return m_Looping;
	}

	/// <summary>
	/// Sets whether the channel should repeat itself.
	/// </summary>
	/// <param name="a_Looping"></param>
	void XAudio2Channel::SetLooping(bool a_Looping)
	{
		m_Looping = a_Looping;
	}

	/// <summary>
	/// Applies all the effects.
	/// </summary>
	/// <param name="a_Data">The pcm data that needs to be processed.</param>
	/// <param name="a_Size">The size of the pcm data block.</param>
	/// <returns></returns>
	void XAudio2Channel::ApplyEffects(unsigned char *&a_Data, uint32_t a_Size) const
	{
		FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);

		// Master volume.
		effects::ChangeVolume<int16_t>(a_Data, a_Size, m_AudioSystem->GetMasterVolume(), fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Channel volume.
		effects::ChangeVolume<int16_t>(a_Data, a_Size, m_Volume, fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Sound volume (not sure why you would want this but I want it in here damn it)
		effects::ChangeVolume<int16_t>(a_Data, a_Size, m_CurrentSound->GetVolume(), fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Master panning.
		effects::ChangePanning<int16_t>(a_Data, a_Size, m_AudioSystem->GetMasterPanning(), fmt_chunk.numChannels);

		//// Channel panning.
		effects::ChangePanning<int16_t>(a_Data, a_Size, m_Panning, fmt_chunk.numChannels);
	}

	/// <summary>
	/// Returns the sound.
	/// </summary>
	/// <returns>The sound.</returns>
	const WaveFile &XAudio2Channel::GetSound() const
	{
		return *m_CurrentSound;
	}
}
