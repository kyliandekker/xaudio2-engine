#include "Audio/AudioSystem.h"
#include "Audio/WaveFile.h"
#include "Audio/XAudio2Channel.h"

#include "Audio/Effects.h"
#include "Audio/Logger.h"
#include "Audio/math.h"

XAudio2Channel::XAudio2Channel(AudioSystem& a_AudioSystem) : m_AudioSystem(a_AudioSystem)
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
/// Sets the sound of a channel and starts the playback.
/// </summary>
/// <param name="a_Sound">A pointer to a wave file.</param>
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
		if (FAILED(hr = m_AudioSystem.GetEngine().CreateSourceVoice(&m_SourceVoice, &wave, 0, 1.0f, &m_VoiceCallback)))
		{
			logger::log_error("<XAudio2> Creating XAudio Source Voice failed.");
			return;
		}
	}
	if (FAILED(hr = m_SourceVoice->Start(0, 0)))
	{
		logger::log_error("<XAudio2> Starting XAudio Source Voice failed.");
	}
}

/// <summary>
/// Starts the playback of the channel.
/// </summary>
void XAudio2Channel::Start()
{
	m_IsPlaying = true;
}

/// <summary>
/// Resumes the playback of the channel.
/// </summary>
void XAudio2Channel::Resume()
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

		// Master volume.
		m_Data = effects::ChangeVolume(m_Data, size, m_AudioSystem.GetVolume());

		// Channel volume.
		m_Data = effects::ChangeVolume(m_Data, size, m_Volume);

		// Sound volume (not sure why you would want this but I want it in here damn it)
		m_Data = effects::ChangeVolume(m_Data, size, m_CurrentSound->GetVolume());

		// Master panning.
		m_Data = effects::ChangePanning(m_Data, size, m_AudioSystem.GetPanning());

		// Channel panning.
		m_Data = effects::ChangePanning(m_Data, size, m_Panning);

		// Other effects.
		m_Data = ApplyEffects(m_Data, size);

		m_DataSize = size;

		// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
		m_CurrentPos += size;

		XAUDIO2_BUFFER xBuffer = {0, 0, nullptr, 0, 0, 0, 0, 0, nullptr};
		xBuffer.AudioBytes = size;		 // Buffer containing audio data.
		xBuffer.pAudioData = m_Data; // Size of the audio buffer in bytes.
		HRESULT hr;

		// Submit it.
		if (FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&xBuffer)))
			logger::log_error("<XAudio2> Submitting data to XAudio Source Voice failed.");
	}
}

/// <summary>
/// Stops and resets the channel (removing the sound and flushing the buffers).
/// </summary>
void XAudio2Channel::Stop()
{
	RemoveSound();

	Reset();

	// Stop the source voice.
	m_SourceVoice->Stop();

	// Flush the buffers.
	m_SourceVoice->FlushSourceBuffers();
}

/// <summary>
/// Resets the data position of the channel.
/// </summary>
void XAudio2Channel::Reset()
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
	a_Volume = math::ClampF(a_Volume, 0.0f, 1.0f);
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
	a_Panning = math::ClampF(a_Panning, -1.0f, 1.0f);
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
/// Returns the current data position (the playback point).
/// </summary>
/// <returns>The current data position (the playback point).</returns>
uint32_t XAudio2Channel::GetCurrentDataPos() const
{
	return m_CurrentPos;
}

/// <summary>
/// Returns the data size.
/// </summary>
/// <returns>The data size.</returns>
uint32_t XAudio2Channel::GetDataSize() const
{
	return m_DataSize;
}

/// <summary>
/// Returns the data that is currently being played.
/// </summary>
/// <returns>The data that is currently being played.</returns>
unsigned char* XAudio2Channel::GetData() const
{
	return m_Data;
}

/// <summary>
/// Applies all the effects.
/// </summary>
/// <param name="a_Data">The pcm data that needs to be processed.</param>
/// <param name="a_BufferSize">The size of the pcm data block.</param>
/// <returns></returns>
unsigned char* XAudio2Channel::ApplyEffects(unsigned char* a_Data, uint32_t)
{
	return a_Data;
}

/// <summary>
/// Returns the sound.
/// </summary>
/// <returns>The sound.</returns>
const WaveFile& XAudio2Channel::GetSound() const
{
	return *m_CurrentSound;
}
