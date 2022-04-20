﻿#include <SoundSystem.h>

#include <utility>

#include "utils/Logger.h"

namespace uaudio
{
	UAUDIO_DEFAULT_HASH SoundSystem::LoadSound(const char *a_Path, const char *a_Name, std::vector<const char*> a_Chunks)
	{
		UAUDIO_DEFAULT_HASH hash = UAUDIO_DEFAULT_HASH_FUNCTION(a_Name);

		if (!DoesSoundExist(hash))
			m_Sounds.insert(std::make_pair(hash, WaveFile(a_Path, std::move(a_Chunks))));

		return hash;
	}

	void SoundSystem::UnloadSound(UAUDIO_DEFAULT_HASH hash)
	{
		m_Sounds.erase(hash);
	}

	WaveFile *SoundSystem::FindSound(UAUDIO_DEFAULT_HASH a_Hash)
	{
		if (DoesSoundExist(a_Hash))
			return &m_Sounds[a_Hash];

		logger::ASSERT(false, "Hash %i not found.", a_Hash);
		logger::log_warning("<ResourceManager> Could not find requested sound with hash number %i. Using default sound instead.", a_Hash);

		return nullptr;
	}

	bool SoundSystem::DoesSoundExist(UAUDIO_DEFAULT_HASH a_Hash) const
	{
		return m_Sounds.find(a_Hash) != m_Sounds.end();
	}

	uint32_t SoundSystem::SoundSize() const
	{
		return static_cast<uint32_t>(m_Sounds.size());
	}

	std::vector<WaveFile *, UAUDIO_DEFAULT_ALLOCATOR<WaveFile *>> SoundSystem::GetSounds()
	{
		std::vector<WaveFile *, UAUDIO_DEFAULT_ALLOCATOR<WaveFile *>> sounds;
		for (auto &sound_file : m_Sounds)
			sounds.push_back(&sound_file.second);

		return sounds;
	}

	std::vector<UAUDIO_DEFAULT_HASH, UAUDIO_DEFAULT_ALLOCATOR<UAUDIO_DEFAULT_HASH>> SoundSystem::GetSoundHashes()
	{
		std::vector<UAUDIO_DEFAULT_HASH, UAUDIO_DEFAULT_ALLOCATOR<UAUDIO_DEFAULT_HASH>> sounds;
		for (auto &sound_file : m_Sounds)
			sounds.push_back(sound_file.first);

		return sounds;
	}
}
