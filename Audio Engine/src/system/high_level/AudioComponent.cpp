#include <uaudio/system/high_level/AudioComponent.h>

#include <uaudio/utils/Logger.h>

#include <uaudio/utils/Utils.h>

namespace uaudio
{
	AudioComponent::AudioComponent(const AudioComponent &rhs) : m_SoundData(rhs.m_SoundData)
	{
	}

	AudioComponent &AudioComponent::operator=(const AudioComponent &rhs)
	{
		if (&rhs != this)
		{
			m_SoundData = rhs.m_SoundData;
		}
		return *this;
	}

	/// <summary>
	/// Plays a sound.
	/// </summary>
	/// <param name="a_Hash"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::Play(UAUDIO_DEFAULT_HASH a_Hash)
	{
		m_SoundData.soundHash = a_Hash;
		m_SoundData.SetState(SOUND_STATE::SOUND_STATE_PLAYING);
		m_SoundData.changed = true;

		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Pauses the playback of a sound.
	/// </summary>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::Pause()
	{
		if (m_SoundData.soundHash != HASH_INVALID)
		{
			m_SoundData.SetState(SOUND_STATE::SOUND_STATE_PAUSED);
			m_SoundData.changed = true;
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		logger::ASSERT(false, "<AudioComponent> Sound has not been set in the audio component.");
		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;
	}

	/// <summary>
	/// Resumes the playback of a sound.
	/// </summary>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::Resume()
	{
		if (m_SoundData.soundHash != HASH_INVALID)
		{
			m_SoundData.SetState(SOUND_STATE::SOUND_STATE_PLAYING);
			m_SoundData.changed = true;
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		logger::ASSERT(false, "<AudioComponent> Sound has not been set in the audio component.");
		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;
	}

	/// <summary>
	/// Checks whether a sound is playing or not.
	/// </summary>
	/// <param name="a_IsPlaying"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::IsPlaying(bool &a_IsPlaying) const
	{
		if (m_SoundData.soundHash != HASH_INVALID)
		{
			a_IsPlaying = m_SoundData.current == SOUND_STATE::SOUND_STATE_PLAYING;
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		logger::ASSERT(false, "<AudioComponent> Sound has not been set in the audio component.");
		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_SET;
	}

	/// <summary>
	/// Sets the volume of a sound.
	/// </summary>
	/// <param name="a_Volume">The volume (ranging from 0 to 1).</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::SetVolume(float a_Volume)
	{
		a_Volume = utils::clamp(a_Volume, UAUDIO_MIN_VOLUME, UAUDIO_MAX_VOLUME);
		m_SoundData.volume = a_Volume;
		m_SoundData.changed = true;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Retrieves the volume of a sound.
	/// </summary>
	/// <param name="a_Volume"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::GetVolume(float &a_Volume) const
	{
		a_Volume = m_SoundData.volume;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets the panning of a sound.
	/// </summary>
	/// <param name="a_Panning">The panning (ranging from -1 to 1, with 0 being neutral).</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::SetPanning(float a_Panning)
	{
		a_Panning = utils::clamp(a_Panning, UAUDIO_MIN_PANNING, UAUDIO_MAX_PANNING);
		m_SoundData.panning = a_Panning;
		m_SoundData.changed = true;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Retrieves the panning of a sound.
	/// </summary>
	/// <param name="a_Panning"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::GetPanning(float &a_Panning) const
	{
		a_Panning = m_SoundData.panning;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets whether a sound should loop or not.
	/// </summary>
	/// <param name="a_IsLooping"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::SetLooping(bool a_IsLooping)
	{
		m_SoundData.looping = a_IsLooping;
		m_SoundData.changed = true;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Checks whether a sound is looping or not.
	/// </summary>
	/// <param name="a_IsLooping"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::IsLooping(bool &a_IsLooping) const
	{
		a_IsLooping = m_SoundData.looping;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	UAUDIO_RESULT AudioComponent::IsActive(bool& a_IsActive) const
	{
		a_IsActive = m_SoundData.soundHash != 0;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	UAUDIO_RESULT AudioComponent::GetHash(UAUDIO_DEFAULT_HASH& a_Hash) const
	{
		a_Hash = m_SoundData.soundHash;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	UAUDIO_RESULT AudioComponent::GetChannelIndex(int32_t& a_Index) const
	{
		a_Index = m_SoundData.channel;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets the playback (whether a channel emits sound).
	/// </summary>
	/// <param name="a_Playback"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::SetPlayback(bool a_Playback)
	{
		m_SoundData.playback = a_Playback;
		m_SoundData.changed = true;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Retrieves whether the channel emits sound.
	/// </summary>
	/// <param name="a_Playback"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioComponent::GetPlayback(bool& a_Playback) const
	{
		a_Playback = m_SoundData.playback;
		return UAUDIO_RESULT::UAUDIO_OK;
	}
}
