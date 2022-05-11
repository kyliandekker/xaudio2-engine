#include <uaudio/system/low_level/backends/xaudio2/XAudio2Channel.h>

#include <comdef.h>

#include <uaudio/utils/Logger.h>
#include <uaudio/system/low_level/backends/xaudio2/XAudio2Backend.h>
#include <uaudio/wave/low_level/WaveEffects.h>
#include <uaudio/system/high_level/Channel.h>

#include <uaudio/wave/low_level/WaveConverter.h>

#include <uaudio/wave/high_level/WaveChunks.h>

namespace uaudio::xaudio2
{
	XAudio2Channel::XAudio2Channel(XAudio2Backend& a_AudioSystem) : m_AudioSystem(&a_AudioSystem)
	{ }

	XAudio2Channel::XAudio2Channel(const XAudio2Channel& rhs) : Channel(rhs), m_SourceVoice(rhs.m_SourceVoice), m_VoiceCallback(rhs.m_VoiceCallback), m_AudioSystem(rhs.m_AudioSystem)
	{ }

	XAudio2Channel::~XAudio2Channel()
	{
		Channel::Stop();

		// Stop the source voice.
		if (m_SourceVoice != nullptr)
		{
			m_SourceVoice->Stop();
			m_SourceVoice->FlushSourceBuffers();

			m_SourceVoice->DestroyVoice();
			m_SourceVoice = nullptr;
		}
	}

	XAudio2Channel& XAudio2Channel::operator=(const XAudio2Channel& rhs)
	{
		Channel::operator=(rhs);
		if (&rhs != this)
		{
			m_SourceVoice = rhs.m_SourceVoice;
			m_VoiceCallback = rhs.m_VoiceCallback;
			m_AudioSystem = rhs.m_AudioSystem;
		}
		return *this;
	}
	
	/// <summary>
	/// Sets the sound and sound data.
	/// </summary>
	/// <param name="a_Sound">The sound (wave).</param>
	/// <param name="a_SoundData">The sound data.</param>
	/// <returns></returns>
	UAUDIO_RESULT XAudio2Channel::SetSound(const WaveFormat& a_Sound, const SoundData& a_SoundData)
	{
		Channel::SetSound(a_Sound, a_SoundData);
		if (a_SoundData.looping)
			m_Looping = a_SoundData.looping;

		if (m_SourceVoice != nullptr)
		{
			Stop();
			// Stop the source voice.
			if (m_SourceVoice != nullptr)
			{
				m_SourceVoice->Stop();
				m_SourceVoice->FlushSourceBuffers();

				m_SourceVoice->DestroyVoice();
				m_SourceVoice = nullptr;
			}
		}

		if (m_SourceVoice == nullptr)
		{
			WAVEFORMATEX wave;

			FMT_Chunk fmt_chunk;
			m_CurrentSound->GetChunkFromData<FMT_Chunk>(fmt_chunk, FMT_CHUNK_ID);

			// Set WAV format default. (what we expect the user to provide).
			wave.wFormatTag = fmt_chunk.audioFormat;
			wave.nChannels = fmt_chunk.numChannels;
			wave.nSamplesPerSec = fmt_chunk.sampleRate; // 44100 hz (should be standard).
			wave.cbSize = 0;
			wave.wBitsPerSample = fmt_chunk.bitsPerSample;
			wave.nBlockAlign = fmt_chunk.blockAlign;
			wave.nAvgBytesPerSec = fmt_chunk.byteRate;

			if (m_AudioSystem->GetEngine() == nullptr)
				return UAUDIO_RESULT::UAUDIO_ERR_XAUDIO2_NO_SYSTEM;
			HRESULT hr;
			if (FAILED(hr = m_AudioSystem->GetEngine()->CreateSourceVoice(&m_SourceVoice, &wave, 0, 2.0f, &m_VoiceCallback)))
			{
				logger::ASSERT(false, "<XAudio2> Creating XAudio2 Source Voice failed.");
				m_IsPlaying = false;
				return UAUDIO_RESULT::UAUDIO_ERR_XAUDIO2_SOURCEVOICE_CREATION_FAILED;
			}
		}
		HRESULT hr;
		if (FAILED(hr = m_SourceVoice->Start(0, 0)))
		{
			logger::ASSERT(false, "<XAudio2> Starting XAudio2 Source Voice failed.");
			m_IsPlaying = false;
		}
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Retrieves the XAudio2 Source Voice.
	/// </summary>
	/// <returns></returns>
	IXAudio2SourceVoice& XAudio2Channel::GetSourceVoice() const
	{
		return *m_SourceVoice;
	}

	/// <summary>
	/// Retrieves the Voice Callback.
	/// </summary>
	/// <returns></returns>
	XAudio2Callback& XAudio2Channel::GetVoiceCallback()
	{
		return m_VoiceCallback;
	}

	/// <summary>
	/// Plays a buffer based on the range provided.
	/// </summary>
	/// <param name="a_StartingPosition">The starting position.</param>
	/// <param name="a_BufferSize">The buffer size.</param>
	/// <returns></returns>
	UAUDIO_RESULT XAudio2Channel::PlayRanged(uint32_t a_StartingPosition, uint32_t a_BufferSize)
	{
		const UAUDIO_RESULT result = Channel::PlayRanged(a_StartingPosition, a_BufferSize);
		if (result != UAUDIO_RESULT::UAUDIO_OK)
			return result;

		if (m_SourceVoice == nullptr)
			return UAUDIO_RESULT::UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE;

		XAUDIO2_VOICE_STATE state;
		m_SourceVoice->GetState(&state);
		if (state.BuffersQueued < GetWaveBufferSize())
		{
			unsigned char* data = GetBufferPart(a_StartingPosition, a_BufferSize);

			ApplyEffects(data, a_BufferSize, m_AudioSystem);

			if (data != nullptr)
			{
				// Make sure the new pos is the current pos.
				m_CurrentPosition = a_StartingPosition;

				// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
				m_CurrentPosition += a_BufferSize;

				PlayBuffer(data, a_BufferSize);
				m_DataBuffers.push(data);
			}
		}
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Plays a sound buffer.
	/// </summary>
	/// <param name="a_DataBuffer">The data buffer.</param>
	/// <param name="a_Size">The buffer size.</param>
	/// <returns></returns>
	UAUDIO_RESULT XAudio2Channel::PlayBuffer(const unsigned char* a_DataBuffer, uint32_t a_Size)
	{
		XAUDIO2_BUFFER x_buffer = { 0, 0, nullptr, 0, 0, 0, 0, 0, nullptr };
		x_buffer.AudioBytes = a_Size;		// Buffer containing audio data.
		x_buffer.pAudioData = a_DataBuffer; // Size of the audio buffer in bytes.

		if (HRESULT hr; FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&x_buffer)))
		{
			const _com_error err(hr);
			const LPCTSTR errMsg = err.ErrorMessage();
			logger::log_warning("<XAudio2> Submitting data to XAudio2 Source Voice failed: 0x%08x: %s.", hr, errMsg);
			return UAUDIO_RESULT::UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE;
		}
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Updates the channel.
	/// </summary>
	/// <returns></returns>
	UAUDIO_RESULT XAudio2Channel::Update()
	{
		const UAUDIO_RESULT result = Channel::Update();
		if (result != UAUDIO_RESULT::UAUDIO_OK)
			return result;

		if (m_SourceVoice == nullptr)
			return UAUDIO_RESULT::UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE;

		return PlayRanged(m_CurrentPosition, static_cast<int>(m_AudioSystem->GetBufferSize()));
	}
}
