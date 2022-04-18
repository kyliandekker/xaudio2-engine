#pragma once

#include <array>

#include "AudioSystem.h"
#include "SoundSystem.h"
#include "tools/BaseTool.h"

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

	bool
		m_AllChunks = false,
		m_RiffChunk = true,
		m_FmtChunk = true,
		m_DataChunk = true,
		m_AcidChunk = false,
		m_BextChunk = false,
		m_FactChunk = false,
		m_SmplChunk = false,
		m_CueChunk = false,
		m_OtherChunks = false;
};
