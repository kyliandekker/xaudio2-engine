#include <uaudio/system/high_level/Channel.h>

#include <uaudio/wave/high_level/WaveChunks.h>

#include <uaudio/wave/low_level/WaveConverter.h>
#include <uaudio/wave/low_level/WaveEffects.h>

#include <uaudio/system/low_level/AudioBackend.h>

#include "uaudio/utils/Logger.h"

namespace uaudio
{
	Channel::Channel(const Channel& rhs)
	{
		m_Volume = rhs.m_Volume;
		m_Panning = rhs.m_Panning;
		m_Looping = rhs.m_Looping;
		m_Playback = rhs.m_Playback;
		m_IsPlaying = rhs.m_IsPlaying;
		m_DataBuffers = rhs.m_DataBuffers;
		m_CurrentPosition = rhs.m_CurrentPosition;
		m_CurrentSound = rhs.m_CurrentSound;
		m_CurrentSoundData = rhs.m_CurrentSoundData;
	}

	Channel& Channel::operator=(const Channel& rhs)
	{
		if (&rhs != this)
		{
			m_Volume = rhs.m_Volume;
			m_Panning = rhs.m_Panning;
			m_Looping = rhs.m_Looping;
			m_Playback = rhs.m_Playback;
			m_IsPlaying = rhs.m_IsPlaying;
			m_DataBuffers = rhs.m_DataBuffers;
			m_CurrentPosition = rhs.m_CurrentPosition;
			m_CurrentSound = rhs.m_CurrentSound;
			m_CurrentSoundData = rhs.m_CurrentSoundData;
		}
		return *this;
	}

	/// <summary>
	/// Resets the playback position of the channel.
	/// </summary>
	/// <returns></returns>
	UAUDIO_RESULT Channel::ResetPos()
	{
		return SetPosition(0);
	}

	/// <summary>
	/// Plays a buffer based on the range provided.
	/// </summary>
	/// <param name="a_StartingPosition">The starting position.</param>
	/// <param name="a_BufferSize">The buffer size.</param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::PlayRanged(uint32_t, uint32_t)
	{
		if (m_CurrentSound == nullptr)
			return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets the position of the playback.
	/// </summary>
	/// <param name="a_StartPos">The position in bytes.</param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::SetPosition(uint32_t a_StartPos)
	{
		if (m_CurrentSound != nullptr)
		{
			uint32_t size = 0;
			m_CurrentSound->GetChunkSize(size, DATA_CHUNK_ID);
			a_StartPos = utils::clamp<uint32_t>(a_StartPos, 0, size);
			m_CurrentPosition = a_StartPos;
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;
	}

	/// <summary>
	/// Retrieves the position of the playback.
	/// </summary>
	/// <param name="a_Position"></param>
	/// <param name="a_TimeUnit">The time unit (bytes, s, ms).</param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::GetPosition(uint32_t& a_Position, TIMEUNIT a_TimeUnit) const
	{
		if (m_CurrentSound == nullptr)
			return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

		FMT_Chunk fmt_chunk;
		m_CurrentSound->GetChunkFromData<FMT_Chunk>(fmt_chunk, FMT_CHUNK_ID);
		switch (a_TimeUnit)
		{
			case TIMEUNIT::TIMEUNIT_MS:
			{
				a_Position = static_cast<uint32_t>(utils::PosToMilliseconds(m_CurrentPosition, fmt_chunk.byteRate));
				break;
			}
			case TIMEUNIT::TIMEUNIT_S:
			{
				a_Position = static_cast<uint32_t>(utils::PosToSeconds(m_CurrentPosition, fmt_chunk.byteRate));
				break;
			}
			case TIMEUNIT::TIMEUNIT_POS:
			{
				a_Position = m_CurrentPosition;
				break;
			}
		}

		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Retrieves the sound.
	/// </summary>
	/// <param name="a_WaveFormat"></param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::GetSound(const WaveFormat*& a_WaveFormat) const
	{
		if (m_CurrentSound != nullptr)
		{
			a_WaveFormat = m_CurrentSound;
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;
	}

	/// <summary>
	/// Removes the current sound.
	/// </summary>
	/// <returns></returns>
	UAUDIO_RESULT Channel::RemoveSound()
	{
		m_CurrentSound = nullptr;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets whether the playback emits sound.
	/// </summary>
	/// <param name="a_Playback"></param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::SetPlayback(bool a_Playback)
	{
		m_Playback = a_Playback;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Returns whether or not the playback emits sound.
	/// </summary>
	/// <param name="a_Playback"></param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::GetPlayback(bool& a_Playback) const
	{
		a_Playback = m_Playback;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Returns whether the channel currently has a sound or not.
	/// </summary>
	/// <param name="a_IsInUse"></param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::IsInUse(bool& a_IsInUse) const
	{
		a_IsInUse = m_CurrentSound != nullptr;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets the sound and sound data.
	/// </summary>
	/// <param name="a_Sound">The sound (wave).</param>
	/// <param name="a_SoundData">The sound data.</param>
	/// <returns></returns>
	UAUDIO_RESULT Channel::SetSound(const WaveFormat& a_Sound, const SoundData& a_SoundData)
	{
		ResetPos();
		m_CurrentSound = &a_Sound;
		m_CurrentSoundData = &a_SoundData;

		bool hasChunk = false;
		m_CurrentSound->HasChunk(hasChunk, FMT_CHUNK_ID);
		if (!hasChunk)
		{
			logger::log_error("<Channel> Sound has no FMT Chunk.");
			return UAUDIO_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;
		}

		m_CurrentSound->HasChunk(hasChunk, DATA_CHUNK_ID);
		if (!hasChunk)
		{
			logger::log_error("<Channel> Sound has no DATA Chunk.");
			return UAUDIO_RESULT::UAUDIO_ERR_NO_DATA_CHUNK;
		}

		FMT_Chunk fmt_chunk;
		m_CurrentSound->GetChunkFromData<FMT_Chunk>(fmt_chunk, FMT_CHUNK_ID);

		if (fmt_chunk.numChannels != WAVE_CHANNELS_MONO && fmt_chunk.numChannels != WAVE_CHANNELS_STEREO)
		{
			logger::log_error("<Channel> Sound is neither mono nor stereo.");
			return UAUDIO_RESULT::UAUDIO_ERR_NUM_CHANNELS_INVALID;
		}

		if (fmt_chunk.bitsPerSample != WAVE_BITS_PER_SAMPLE_16 && fmt_chunk.bitsPerSample != WAVE_BITS_PER_SAMPLE_24 && fmt_chunk.bitsPerSample != WAVE_BITS_PER_SAMPLE_32)
		{
			logger::log_error("<Channel> Sound does not have a valid bits per sample value.");
			return UAUDIO_RESULT::UAUDIO_ERR_BITS_PER_SAMPLE_INVALID;
		}

		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Returns the buffer size based on the current sound.
	/// </summary>
	/// <returns></returns>
	uint32_t Channel::GetWaveBufferSize() const
	{
		if (m_CurrentSound != nullptr)
		{
			FMT_Chunk fmt_chunk;
			m_CurrentSound->GetChunkFromData<FMT_Chunk>(fmt_chunk, FMT_CHUNK_ID);
			return fmt_chunk.bitsPerSample / 8;
		}
		return 0;
	}

	/// <summary>
	/// Updates the channel.
	/// </summary>
	/// <returns></returns>
	UAUDIO_RESULT Channel::Update()
	{
		if (m_CurrentSound == nullptr)
			return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

		if (!m_IsPlaying)
			return UAUDIO_RESULT::UAUDIO_ERR_CHANNEL_NOT_PLAYING;

		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Empties the buffers.
	/// </summary>
	void Channel::EmptyBuffers()
	{
		while (!m_DataBuffers.empty())
		{
			unsigned char* buffer = m_DataBuffers.front();
			UAUDIO_DEFAULT_FREE(buffer);
			m_DataBuffers.pop();
		}
	}

	/// <summary>
	/// Stops the channel, resets the position and empties the buffers.
	/// </summary>
	void Channel::Stop()
	{
		Pause();
		EmptyBuffers();
		ResetPos();
	}

	/// <summary>
	/// Applies effects to a sound buffer.
	/// </summary>
	/// <param name="a_DataBuffer">The data buffer.</param>
	/// <param name="a_BufferSize">The buffer size.</param>
	/// <param name="a_AudioBackend">The backend (needed for the panning and volume).</param>
	void Channel::ApplyEffects(unsigned char* a_DataBuffer, uint32_t a_BufferSize, AudioBackend* a_AudioBackend) const
	{
		if (m_CurrentSound == nullptr)
			return;

		FMT_Chunk fmt_chunk;
		m_CurrentSound->GetChunkFromData<FMT_Chunk>(fmt_chunk, FMT_CHUNK_ID);

		//// Master volume.
		effects::ChangeVolume(a_DataBuffer, a_BufferSize, a_AudioBackend->GetMasterVolume(), fmt_chunk.blockAlign, fmt_chunk.numChannels);

		//// Channel volume.
		effects::ChangeVolume(a_DataBuffer, a_BufferSize, m_Volume, fmt_chunk.blockAlign, fmt_chunk.numChannels);

		//// Channel volume.
		effects::ChangeVolume(a_DataBuffer, a_BufferSize, m_CurrentSoundData->volume, fmt_chunk.blockAlign, fmt_chunk.numChannels);

		// Master panning.
		effects::ChangePanning(a_DataBuffer, a_BufferSize, a_AudioBackend->GetMasterPanning(), fmt_chunk.numChannels);

		// Channel panning.
		effects::ChangePanning(a_DataBuffer, a_BufferSize, m_Panning, fmt_chunk.numChannels);

		if (!m_Playback)
			effects::ChangeVolume(a_DataBuffer, a_BufferSize, UAUDIO_MIN_VOLUME, fmt_chunk.blockAlign, fmt_chunk.numChannels);
	}

	/// <summary>
	/// Retrieves the buffer part.
	/// </summary>
	/// <param name="a_StartingPosition">The starting position.</param>
	/// <param name="a_BufferSize">The size starting from the starting position.</param>
	/// <returns></returns>
	unsigned char* Channel::GetBufferPart(uint32_t& a_StartingPosition, uint32_t& a_BufferSize)
	{
		if (m_DataBuffers.size() == GetWaveBufferSize())
		{
			unsigned char* buffer = m_DataBuffers.front();
			UAUDIO_DEFAULT_FREE(buffer);
			m_DataBuffers.pop();
		}

		uint32_t size = 0;
		m_CurrentSound->GetChunkSize(size, DATA_CHUNK_ID);

		// If the sound is done playing, check whether it needs to be repeated or whether it needs to be stopped entirely.
		if (a_StartingPosition >= size)
		{
			// If the sound is not set to repeat, then stop the channel.
			bool isLooping = false;
			m_CurrentSound->IsLooping(isLooping);
			if (m_CurrentSoundData->looping || m_Looping || isLooping)
			{
				m_CurrentSound->GetStartPosition(m_CurrentPosition);
				m_CurrentSound->GetStartPosition(a_StartingPosition);
			}
			else
			{
				RemoveSound();
				Stop();
				return nullptr;
			}
		}

		DATA_Chunk data_chunk;
		m_CurrentSound->GetChunkFromData<DATA_Chunk>(data_chunk, DATA_CHUNK_ID);

		conversion::CalculateBufferSize(a_StartingPosition, a_BufferSize, size, a_BufferSize);
		unsigned char* data = reinterpret_cast<unsigned char*>(UAUDIO_DEFAULT_ALLOC(a_BufferSize));

		// Read the part of the wave file and store it back in the read buffer.
		conversion::ReadDataBuffer(a_StartingPosition, a_BufferSize, data, data_chunk.data);

		return data;
	}
}
