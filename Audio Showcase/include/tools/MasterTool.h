#pragma once

#include <array>

#include "AudioSystem.h"
#include "SoundSystem.h"
#include "tools/BaseTool.h"

struct chunk_select
{
	std::string chunk_id;
	bool selected = false;
	bool removable = true;
};

class MasterTool : public BaseTool
{
public:
	MasterTool(uaudio::AudioSystem &a_AudioSystem, uaudio::SoundSystem &a_SoundSystem);
	void Render() override;

private:
	void OpenFile();

	uaudio::AudioSystem &m_AudioSystem;
	uaudio::SoundSystem &m_SoundSystem;

	std::array<const char *, 7> m_BufferSizeTextOptions = {
		"256",
		"384",
		"512",
		"1024",
		"2048",
		"4096",
		"8192",
	};

	uint32_t m_BufferSizeSelection = 0;

	std::array<uaudio::BUFFERSIZE, 7> m_BufferSizeOptions = {
		uaudio::BUFFERSIZE::BUFFERSIZE_256,
		uaudio::BUFFERSIZE::BUFFERSIZE_384,
		uaudio::BUFFERSIZE::BUFFERSIZE_512,
		uaudio::BUFFERSIZE::BUFFERSIZE_1024,
		uaudio::BUFFERSIZE::BUFFERSIZE_2048,
		uaudio::BUFFERSIZE::BUFFERSIZE_4096,
		uaudio::BUFFERSIZE::BUFFERSIZE_8192,
	};

	std::array<uint16_t, 3> m_BitsPerSampleOptions = {
		uaudio::WAVE_BITS_PER_SAMPLE_16,
		uaudio::WAVE_BITS_PER_SAMPLE_24,
		uaudio::WAVE_BITS_PER_SAMPLE_32,
	};

	std::vector<chunk_select> m_ChunkIds;
	chunk_select m_SelectedChunk = { "", false, true };
	uint16_t m_BitsPerSample = uaudio::UAUDIO_DEFAULT_BITS_PER_SAMPLE;
	uint16_t m_Channels = uaudio::UAUDIO_DEFAULT_CHANNELS;

	uint32_t m_BitsPerSampleSelection = 0;
};
