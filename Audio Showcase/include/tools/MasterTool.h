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
	MasterTool(uaudio::AudioSystem& a_AudioSystem, uaudio::SoundSystem& a_SoundSystem);
	void Render() override;
private:
	void OpenFile();

	uaudio::AudioSystem& m_AudioSystem;
	uaudio::SoundSystem& m_SoundSystem;

	std::array<const char*, 7> m_BufferSizeTextOptions = {
		"256",
		"384",
		"512",
		"1024",
		"2048",
		"4096",
		"8192",
	};

	uint32_t m_BufferSizeSelection = -1;

	std::array<uaudio::BUFFERSIZE, 7> m_BufferSizeOptions = {
		uaudio::BUFFERSIZE::BUFFERSIZE_256,
		uaudio::BUFFERSIZE::BUFFERSIZE_384,
		uaudio::BUFFERSIZE::BUFFERSIZE_512,
		uaudio::BUFFERSIZE::BUFFERSIZE_1024,
		uaudio::BUFFERSIZE::BUFFERSIZE_2048,
		uaudio::BUFFERSIZE::BUFFERSIZE_4096,
		uaudio::BUFFERSIZE::BUFFERSIZE_8192,
	};

	std::vector<chunk_select> m_ChunkIds;
};
