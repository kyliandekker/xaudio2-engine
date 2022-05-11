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
		UAUDIO_RESULT LoadSound(UAUDIO_DEFAULT_HASH& a_Hash, const char *a_Path, const char *a_Name, const WaveConfig &a_WaveConfig);
		UAUDIO_RESULT UnloadSound(const UAUDIO_DEFAULT_HASH a_Hash);
		UAUDIO_RESULT FindSound(WaveFormat*& a_WaveFormat, const UAUDIO_DEFAULT_HASH a_Hash);
		UAUDIO_RESULT DoesSoundExist(bool& a_Exists, const UAUDIO_DEFAULT_HASH a_Hash) const;

		UAUDIO_RESULT SoundSize(uint32_t& a_Size) const;
		UAUDIO_RESULT GetSounds(utils::uaudio_vector<const WaveFormat*>& a_Sounds);
		UAUDIO_RESULT GetSoundHashes(utils::uaudio_vector<UAUDIO_DEFAULT_HASH>& a_SoundHashes);
	private:
		std::map<UAUDIO_DEFAULT_HASH, WaveFormat> m_Sounds;
	};
	extern SoundSystem SoundSys;
}