#include "Audio/XAudio2Player.h"
#include "Audio/WaveFile.h"
#include "Audio/XAudio2Channel.h"

#include "Audio/Effects.h"
#include "Audio/Logger.h"

XAudio2Channel::XAudio2Channel(XAudio2Player &a_SoundSystem) : BaseChannel(&a_SoundSystem)
{
	m_VoiceCallback = XAudio2Callback();
}

XAudio2Channel::~XAudio2Channel()
{
	if (m_SourceVoice)
	{
		m_SourceVoice->DestroyVoice();
		m_SourceVoice = nullptr;
	}
}

/// <summary>
/// Sets the sound of a channel.
/// </summary>
/// <param name="a_Sound"></param>
void XAudio2Channel::SetSound(const WaveFile &a_Sound)
{
	m_CurrentSound = &a_Sound;
	HRESULT hr;
	if (m_SourceVoice == nullptr)
	{
		WAVEFORMATEX wave;

		// Set WAV format default. (what we expect the user to provide).
		wave.wFormatTag = a_Sound.GetWavFormat().audioFormat;
		wave.nChannels = a_Sound.GetWavFormat().numChannels;
		wave.nSamplesPerSec = a_Sound.GetWavFormat().sampleRate; // 44100 hz (should be standard).
		wave.cbSize = 0;
		wave.wBitsPerSample = a_Sound.GetWavFormat().bitsPerSample;
		wave.nBlockAlign = a_Sound.GetWavFormat().blockAlign;
		wave.nAvgBytesPerSec = wave.nSamplesPerSec * (wave.wBitsPerSample / a_Sound.GetWavFormat().blockAlign);
		if (FAILED(hr = reinterpret_cast<XAudio2Player*>(m_Player)->GetEngine().CreateSourceVoice(&m_SourceVoice, &wave, 0, 1.0f, &m_VoiceCallback)))
		{
			logger::log_error("<XAudio2> Creating XAudio Source Voice failed.");
			return;
		}
	}
	if (FAILED(hr = m_SourceVoice->Start(0, 0)))
	{
		logger::log_error("<XAudio2> Starting XAudio Source Voice failed.");
		return;
	}
	m_IsPlaying = true;
}

/// <summary>
/// Sets playback to resumed.
/// </summary>
void XAudio2Channel::Resume()
{
	m_IsPlaying = true;
}

/// <summary>
/// Sets playback to paused.
/// </summary>
void XAudio2Channel::Pause()
{
	m_IsPlaying = false;
}

/// <summary>
/// Plays the current sound.
/// </summary>
void XAudio2Channel::Update()
{
	if (m_CurrentSound == nullptr)
		return;

	if (m_SourceVoice == nullptr)
		return;

	if (!m_IsPlaying)
		return;

	// If the sound is done playing, check whether it needs to be repeated or whether it needs to be stopped entirely.
	if (m_CurrentSound->IsEndOfFile(m_CurrentPos))
	{
		// If the sound is not set to repeat, then stop the channel.
		if (!m_CurrentSound->IsLooping())
		{
			Stop();
			return;
		}
		m_CurrentPos = 0;
	}
	XAUDIO2_VOICE_STATE state;
	m_SourceVoice->GetState(&state);
	if (state.BuffersQueued < m_CurrentSound->GetWavFormat().bufferSize)
	{
		// The initial size we want to retrieve from the audio file.
		// TODO: Understand what I am doing here.
		uint32_t size = 8184;

		// Read the part of the wave file and store it back in the read buffer.
		m_CurrentSound->Read(m_CurrentPos, size, m_Data);

		m_Data = effects::change_volume(m_Data, size, m_Player->GetVolume());
		m_Data = effects::change_volume(m_Data, size, m_CurrentSound->GetVolume());
		m_Data = effects::change_panning(m_Data, size, m_Panning);
		m_Data = effects::change_panning(m_Data, size, m_Player->GetPanning());
		m_Data = ApplyEffects(m_Data, size);

		m_CurrentDataSize = size;

		// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
		m_CurrentPos += size;

		XAUDIO2_BUFFER xBuffer = {0, 0, nullptr, 0, 0, 0, 0, 0, nullptr};
		xBuffer.AudioBytes = size;		 // Buffer containing audio data.
		xBuffer.pAudioData = m_Data; // Size of the audio buffer in bytes.
		HRESULT hr;

		// Submit it.
		if (FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&xBuffer)))
		{
			logger::log_error("<XAudio2> Submitting data to XAudio Source Voice failed.");
			return;
		}
	}
}

/// <summary>
/// Stops the source voice, removes the sound and sets the channel to inactive.
/// </summary>
void XAudio2Channel::Stop()
{
	m_CurrentSound = nullptr;
	Reset();

	// Stop the source voice.
	m_SourceVoice->Stop();

	// Flush the buffers.
	m_SourceVoice->FlushSourceBuffers();
}

/// <summary>
/// Resets the playback position.
/// </summary>
void XAudio2Channel::Reset()
{
	m_CurrentPos = 0;
}

/// <summary>
/// Removes the current sound.
/// </summary>
void XAudio2Channel::RemoveSound()
{
	m_CurrentSound = nullptr;
}

/// <summary>
/// Returns the source voice.
/// </summary>
/// <returns></returns>
IXAudio2SourceVoice &XAudio2Channel::GetSourceVoice() const
{
	return *m_SourceVoice;
}

/// <summary>
/// Returns the voice callback.
/// </summary>
/// <returns></returns>
XAudio2Callback &XAudio2Channel::GetVoiceCallback()
{
	return m_VoiceCallback;
}