#pragma once

#include <array>

#include <uaudio/AudioSystem.h>
#include <uaudio/SoundSystem.h>
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

	std::array<const char*, 4> m_BitsPerSampleTextOptions = {
		"I don't really care",
		"16-bit",
		"24-bit",
		"32-bit",
	};

	std::array<uint16_t, 4> m_BitsPerSampleOptions = {
		0,
		uaudio::WAVE_BITS_PER_SAMPLE_16,
		uaudio::WAVE_BITS_PER_SAMPLE_24,
		uaudio::WAVE_BITS_PER_SAMPLE_32,
	};

	std::array<const char*, 3> m_ChannelsTextOptions = {
		"I don't really care",
		"Mono",
		"Stereo"
	};

	std::array<const char*, 4> m_LoopPointTextOptions = {
		"None",
		"Load Start Point",
		"Load End Point",
		"Load Start & End Point"
	};

	uaudio::WaveConfig m_WaveConfig;

	std::vector<chunk_select> m_ChunkIds;
	chunk_select m_SelectedChunk = { "", false, true };

	uint32_t m_SelectedBitsPerSample = 0;
};
