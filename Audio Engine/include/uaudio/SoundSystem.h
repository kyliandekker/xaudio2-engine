#pragma once

#include <map>

#include <uaudio/Includes.h>

#include <uaudio/wave/high_level/WaveConfig.h>

#include <uaudio/wave/low_level/WaveFormat.h>

namespace uaudio
{
	class SoundSystem
	{
	public:
		UAUDIO_DEFAULT_HASH LoadSound(const char *a_Path, const char *a_Name, const WaveConfig &a_WaveConfig);
		void UnloadSound(const UAUDIO_DEFAULT_HASH hash);
		WaveFormat*FindSound(const UAUDIO_DEFAULT_HASH a_Hash);
		bool DoesSoundExist(const UAUDIO_DEFAULT_HASH a_Hash) const;

		uint32_t SoundSize() const;
		std::vector<WaveFormat*, UAUDIO_DEFAULT_ALLOCATOR<WaveFormat*>> GetSounds();
		std::vector<UAUDIO_DEFAULT_HASH, UAUDIO_DEFAULT_ALLOCATOR<UAUDIO_DEFAULT_HASH>> GetSoundHashes();

	private:
		std::map<UAUDIO_DEFAULT_HASH, WaveFormat> m_Sounds;
	};
}
