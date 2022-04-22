#include <uaudio/AudioSystem.h>
#include <uaudio/wave/high_level/WaveFile.h>
#include <uaudio/xaudio2/XAudio2Channel.h>
#include <comdef.h>

#include <uaudio/wave/low_level/WaveEffects.h>
#include <uaudio/utils/Logger.h>
#include <algorithm>

#include <uaudio/wave/high_level/WaveChunks.h>
#include <uaudio/wave/low_level/WaveConverter.h>

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
		if (m_SourceVoice != nullptr)
			Stop();
		if (m_SourceVoice == nullptr)
		{
			WAVEFORMATEX wave;

			const FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
			// Set WAV format default. (what we expect the user to provide).
			wave.wFormatTag = fmt_chunk.audioFormat;
			wave.nChannels = fmt_chunk.numChannels;
			wave.nSamplesPerSec = fmt_chunk.sampleRate; // 44100 hz (should be standard).
			wave.cbSize = 0;
			wave.wBitsPerSample = fmt_chunk.bitsPerSample;
			wave.nBlockAlign = fmt_chunk.blockAlign;
			wave.nAvgBytesPerSec = fmt_chunk.byteRate;
			if (FAILED(hr = m_AudioSystem->GetEngine().CreateSourceVoice(&m_SourceVoice, &wave, 0, 2.0f, &m_VoiceCallback)))
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
		while (!m_DataBuffers.empty())
		{
			unsigned char *buffer = m_DataBuffers.front();
			UAUDIO_DEFAULT_FREE(buffer);
			m_DataBuffers.pop();
		}
		// Stop the source voice.
		if (m_SourceVoice != nullptr)
		{
			m_SourceVoice->Stop();
			m_SourceVoice->FlushSourceBuffers();

			m_SourceVoice->DestroyVoice();
			m_SourceVoice = nullptr;
		}
		m_IsPlaying = false;

		m_CurrentPos = IsInUse() ? m_CurrentSound->GetStartPosition() : 0;
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

	/// <summary>
	/// Sets the position of the channel playback.
	/// </summary>
	/// <param name="a_StartPos"></param>
	void XAudio2Channel::SetPos(uint32_t a_StartPos)
	{
		m_CurrentPos = a_StartPos;
	}

	/// <summary>
	/// Returns the position of the channel playback in the specified time unit.
	/// </summary>
	/// <param name="a_TimeUnit">The time unit (ms, s, bytes)</param>
	/// <returns></returns>
	float XAudio2Channel::GetPos(TIMEUNIT a_TimeUnit) const
	{
		const FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
		switch (a_TimeUnit)
		{
		case TIMEUNIT::TIMEUNIT_MS:
		{
			return utils::PosToMilliseconds(m_CurrentPos, fmt_chunk.byteRate);
			break;
		}
		case TIMEUNIT::TIMEUNIT_S:
		{
			return utils::PosToSeconds(m_CurrentPos, fmt_chunk.byteRate);
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

	/// <summary>
	/// Returns the buffer size of the current sound.
	/// </summary>
	/// <returns></returns>
	uint32_t XAudio2Channel::GetBufferSize() const
	{
		if (m_CurrentSound != nullptr)
		{
			const FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
			return fmt_chunk.bitsPerSample / 8;
		}
		else
			return 0;
	}

	/// <summary>
	/// Plays the sound buffer from starting point and size.
	/// </summary>
	/// <param name="a_StartPos">The start of the sound buffer.</param>
	/// <param name="a_Size">The size of the sound buffer.</param>
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
			if (m_DataBuffers.size() == 2)
			{
				unsigned char *buffer = m_DataBuffers.front();
				UAUDIO_DEFAULT_FREE(buffer);
				m_DataBuffers.pop();
			}

			// If the sound is done playing, check whether it needs to be repeated or whether it needs to be stopped entirely.
			if (m_CurrentSound->IsEndOfBuffer(a_StartPos))
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

			unsigned char *new_data = reinterpret_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(a_Size));
			UAUDIO_DEFAULT_MEMCPY(new_data, data, a_Size);

			// Other effects.
			ApplyEffects(new_data, a_Size);

			// Make sure the new pos is the current pos.
			m_CurrentPos = a_StartPos;

			// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
			m_CurrentPos += a_Size;

			if (!m_Active)
			{
				const FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);
				effects::ChangeVolume<int16_t>(new_data, a_Size, UAUDIO_MIN_VOLUME, fmt_chunk.blockAlign, fmt_chunk.numChannels);
			}

			PlayBuffer(new_data, a_Size);
			m_DataBuffers.push(new_data);
		}
	}

	/// <summary>
	/// Plays a sound buffer.
	/// </summary>
	/// <param name="a_DataBuffer">The sound buffer.</param>
	/// <param name="a_Size">The sound buffer size.</param>
	void XAudio2Channel::PlayBuffer(const unsigned char *a_DataBuffer, uint32_t a_Size) const
	{
		XAUDIO2_BUFFER x_buffer = {0, 0, nullptr, 0, 0, 0, 0, 0, nullptr};
		x_buffer.AudioBytes = a_Size;		// Buffer containing audio data.
		x_buffer.pAudioData = a_DataBuffer; // Size of the audio buffer in bytes.

		if (HRESULT hr; FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&x_buffer)))
		{
			const _com_error err(hr);
			const LPCTSTR errMsg = err.ErrorMessage();
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
		a_Volume = utils::clamp(a_Volume, UAUDIO_MIN_VOLUME, UAUDIO_MAX_VOLUME);
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
		a_Panning = utils::clamp(a_Panning, UAUDIO_MIN_PANNING, UAUDIO_MAX_PANNING);
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
	/// <param name="a_DataBuffer">The pcm data that needs to be processed.</param>
	/// <param name="a_BufferSize">The size of the pcm data block.</param>
	/// <returns></returns>
	void XAudio2Channel::ApplyEffects(unsigned char *&a_DataBuffer, uint32_t a_BufferSize) const
	{
		const FMT_Chunk fmt_chunk = m_CurrentSound->GetWaveFormat().GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);

		// Master volume.
		effects::ChangeVolume<int16_t>(a_DataBuffer, a_BufferSize, m_AudioSystem->GetMasterVolume(), fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Channel volume.
		effects::ChangeVolume<int16_t>(a_DataBuffer, a_BufferSize, m_Volume, fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Sound volume (not sure why you would want this but I want it in here damn it)
		effects::ChangeVolume<int16_t>(a_DataBuffer, a_BufferSize, m_CurrentSound->GetVolume(), fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Master panning.
		effects::ChangePanning<int16_t>(a_DataBuffer, a_BufferSize, m_AudioSystem->GetMasterPanning(), fmt_chunk.numChannels);

		//// Channel panning.
		effects::ChangePanning<int16_t>(a_DataBuffer, a_BufferSize, m_Panning, fmt_chunk.numChannels);
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
