#include <uaudio/system/high_level/AudioSystem.h>

#include <uaudio/Includes.h>

#include <uaudio/system/high_level/AudioSystem.h>
#include <uaudio/system/high_level/Channel.h>

#include <uaudio/utils/Logger.h>

#include <uaudio/allocation/Allocator.h>

#if (UAUDIO_DEFAULT_BACKEND == 1)
#include <uaudio/system/low_level/backends/xaudio2/XAudio2Backend.h>
#endif

namespace uaudio
{
	AudioSystem::AudioSystem()
	{
#if (UAUDIO_DEFAULT_BACKEND == 1)
		m_AudioBackend = new_object<xaudio2::XAudio2Backend>();
#endif
	}

	AudioSystem::~AudioSystem()
	{
		delete_object(m_AudioBackend);
	}

	/// <summary>
	/// Creates an audio component with a channel dedicated to it.
	/// </summary>
	/// <returns></returns>
	AudioComponent& AudioSystem::CreateComponent()
	{
		if (m_AudioBackend != nullptr)
		{
			const size_t size = m_Components.size();
			m_Components.push_back(AudioComponent());
			return m_Components[size];
		}
		logger::ASSERT(false, "<AudioSystem> No backend has been set.");
		return m_Components[0];
	}

	// TODO: Multithread.
	UAUDIO_RESULT AudioSystem::Update()
	{
		if (m_AudioBackend == nullptr)
		{
			logger::ASSERT(false, "<AudioSystem> No backend has been set.");
			return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
		}

		for (uint32_t i = 0; i < m_Components.size(); i++)
		{
			AudioComponent& comp = m_Components[i];
			SoundData& sound = comp.m_SoundData;
			if (sound.changed)
			{
				sound.changed = false;
				switch (sound.current)
				{
					case SOUND_STATE::SOUND_STATE_CREATED:
					{
						sound.channel = m_AudioBackend->CreateSoundChannel(sound);
						Channel* channel = m_AudioBackend->GetChannel(sound.channel);
						channel->SetVolume(sound.volume);
						channel->SetPanning(sound.panning);
						channel->SetLooping(sound.looping);
						channel->SetPlayback(sound.playback);
						break;
					}
					case SOUND_STATE::SOUND_STATE_PAUSED:
					{
						Channel* channel = m_AudioBackend->GetChannel(sound.channel);
						channel->Pause();
						break;
					}
					case SOUND_STATE::SOUND_STATE_PLAYING:
					{
						switch (sound.prev)
						{
							case SOUND_STATE::SOUND_STATE_CREATED:
							{
								sound.channel = m_AudioBackend->CreateSoundChannel(sound);
								Channel* channel = m_AudioBackend->GetChannel(sound.channel);
								sound.SetState(SOUND_STATE::SOUND_STATE_PLAYING);
								channel->SetVolume(sound.volume);
								channel->SetPanning(sound.panning);
								channel->SetLooping(sound.looping);
								channel->SetPlayback(sound.playback);
								break;
							}
							case SOUND_STATE::SOUND_STATE_FINISHED:
							{
								Channel* channel = m_AudioBackend->GetChannel(sound.channel);
								channel->RemoveSound();
								sound.channel = m_AudioBackend->CreateSoundChannel(sound);
								channel = m_AudioBackend->GetChannel(sound.channel);
								sound.SetState(SOUND_STATE::SOUND_STATE_PLAYING);
								channel->SetVolume(sound.volume);
								channel->SetPanning(sound.panning);
								channel->SetLooping(sound.looping);
								channel->SetPlayback(sound.playback);
								sound.SetState(SOUND_STATE::SOUND_STATE_PLAYING);
								break;
							}
							case SOUND_STATE::SOUND_STATE_PAUSED:
							{
								Channel* channel = m_AudioBackend->GetChannel(sound.channel);
								channel->Play();
								break;
							}
							case SOUND_STATE::SOUND_STATE_INVALID:
							case SOUND_STATE::SOUND_STATE_PLAYING:
								break;
						}
						break;
					}
					case SOUND_STATE::SOUND_STATE_INVALID:
					case SOUND_STATE::SOUND_STATE_FINISHED:
						break;
				}
			}
			else if (sound.current == SOUND_STATE::SOUND_STATE_PLAYING)
			{
				bool result = false;
				comp.IsPlaying(result);
				if (result)
				{
					Channel* channel = m_AudioBackend->GetChannel(sound.channel);
					channel->SetVolume(sound.volume);
					channel->SetPanning(sound.panning);
					channel->SetLooping(sound.looping);
					channel->SetPlayback(sound.playback);
					sound.SetState(SOUND_STATE::SOUND_STATE_PLAYING);
				}
				else
					sound.SetState(SOUND_STATE::SOUND_STATE_FINISHED);
			}
		}

		if (m_AudioBackend != nullptr)
			m_AudioBackend->Update();
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Sets the master volume.
	/// </summary>
	/// <param name="a_Volume">The volume (ranging from 0.0 to 1.0).</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::SetMasterVolume(float a_Volume) const
	{
		if (m_AudioBackend != nullptr)
		{
			m_AudioBackend->SetMasterVolume(a_Volume);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Retrieves the master volume.
	/// </summary>
	/// <param name="a_Volume"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::GetMasterVolume(float& a_Volume) const
	{
		if (m_AudioBackend != nullptr)
		{
			a_Volume = m_AudioBackend->GetMasterVolume();
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Sets the master panning.
	/// </summary>
	/// <param name="a_Panning">The panning (ranging from -1.0f to 1.0f with 0.0f being neutral).</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::SetMasterPanning(float a_Panning) const
	{
		if (m_AudioBackend != nullptr)
		{
			m_AudioBackend->SetMasterPanning(a_Panning);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="a_Panning"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::GetMasterPanning(float& a_Panning) const
	{
		if (m_AudioBackend != nullptr)
		{
			a_Panning = m_AudioBackend->GetMasterPanning();
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Sets the buffer size.
	/// </summary>
	/// <param name="a_BufferSize">The buffer size.</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::SetBufferSize(BUFFERSIZE a_BufferSize) const
	{
		if (m_AudioBackend != nullptr)
		{
			m_AudioBackend->SetBufferSize(a_BufferSize);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Retrieves the buffer size.
	/// </summary>
	/// <param name="a_BufferSize"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::GetBufferSize(BUFFERSIZE& a_BufferSize) const
	{
		if (m_AudioBackend != nullptr)
		{
			a_BufferSize = m_AudioBackend->GetBufferSize();
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Sets whether the audio system is active.
	/// </summary>
	/// <param name="a_Active">Active status.</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::SetActive(bool a_Active) const
	{
		if (m_AudioBackend != nullptr)
		{
			m_AudioBackend->SetActive(a_Active);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Retrieves whether the audio system is active or not.
	/// </summary>
	/// <param name="a_Active"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::GetActive(bool& a_Active) const
	{
		if (m_AudioBackend != nullptr)
		{
			a_Active = m_AudioBackend->GetActive();
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Sets the playback of the audio system.
	/// </summary>
	/// <param name="a_Playback">The playback.</param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::SetPlayback(bool a_Playback) const
	{
		if (m_AudioBackend != nullptr)
		{
			m_AudioBackend->SetPlayback(a_Playback);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Retrieves whether or not the audio system is playing back.
	/// </summary>
	/// <param name="a_Playback"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::GetPlayback(bool& a_Playback) const
	{
		if (m_AudioBackend != nullptr)
		{
			a_Playback = m_AudioBackend->GetPlayback();
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	/// <summary>
	/// Returns the amount of components.
	/// </summary>
	/// <param name="a_Size"></param>
	/// <returns></returns>
	UAUDIO_RESULT AudioSystem::GetComponentsSize(size_t& a_Size) const
	{
		a_Size = m_Components.size();
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	AudioComponent& AudioSystem::GetComponent(uint32_t a_Index)
	{
		return m_Components[a_Index];
	}

	UAUDIO_RESULT AudioSystem::CreateSoundChannel(int32_t& a_Channel, SoundData& a_SoundData)
	{
		if (m_AudioBackend != nullptr)
		{
			a_Channel = m_AudioBackend->CreateSoundChannel(a_SoundData);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	UAUDIO_RESULT AudioSystem::CreateSoundChannel(int32_t& a_Channel)
	{
		if (m_AudioBackend != nullptr)
		{
			a_Channel = m_AudioBackend->CreateSoundChannel();
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}

	UAUDIO_RESULT AudioSystem::GetSoundChannel(Channel*& a_Channel, int32_t a_Index)
	{
		if (m_AudioBackend != nullptr)
		{
			a_Channel = m_AudioBackend->GetChannel(a_Index);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_error("<AudioSystem> No backend has been set.");
		return UAUDIO_RESULT::UAUDIO_ERR_NO_BACKEND;
	}
}
