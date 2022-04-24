#pragma once

#include <map>

#include <uaudio/Includes.h>

#include <uaudio/wave/high_level/WaveConfig.h>
#include <uaudio/wave/high_level/WaveFile.h>

namespace uaudio
{
	class SoundSystem
	{
	public:
		UAUDIO_DEFAULT_HASH LoadSound(const char *a_Path, const char *a_Name, WaveConfig &a_WaveConfig);
		void UnloadSound(const UAUDIO_DEFAULT_HASH hash);
		WaveFile *FindSound(const UAUDIO_DEFAULT_HASH a_Hash);
		bool DoesSoundExist(const UAUDIO_DEFAULT_HASH a_Hash) const;

		uint32_t SoundSize() const;
		std::vector<WaveFile *, UAUDIO_DEFAULT_ALLOCATOR<WaveFile *>> GetSounds();
		std::vector<UAUDIO_DEFAULT_HASH, UAUDIO_DEFAULT_ALLOCATOR<UAUDIO_DEFAULT_HASH>> GetSoundHashes();

	private:
		std::map<UAUDIO_DEFAULT_HASH, WaveFile> m_Sounds;
	};
}