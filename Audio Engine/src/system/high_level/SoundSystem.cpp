#include <uaudio/system/high_level/SoundSystem.h>
#include <uaudio/wave/low_level/WaveReader.h>

#include <utility>

#include <uaudio/utils/Logger.h>

namespace uaudio
{
	/// <summary>
	/// Loads a sound.
	/// </summary>
	/// <param name="a_Path">The path to the file.</param>
	/// <param name="a_Name">The name which will be hashed.</param>
	/// <param name="a_WaveConfig">The config to load the file with.</param>
	/// <returns>Returns a hash to the sound.</returns>
	UAUDIO_RESULT SoundSystem::LoadSound(UAUDIO_DEFAULT_HASH& a_Hash, const char* a_Path, const char* a_Name, const WaveConfig& a_WaveConfig)
	{
		UAUDIO_DEFAULT_HASH hash = UAUDIO_DEFAULT_HASH_FUNCTION(a_Name);

		WaveFormat wave_format;

		bool exists = false;
		DoesSoundExist(exists, a_Hash);
		WaveReader::LoadSound(a_Path, wave_format, a_WaveConfig);
		if (!exists)
		{
			m_Sounds.insert(std::make_pair(hash, wave_format));
			logger::log_success(R"(<SoundSystem> Loaded sound successfully (%s"%s"%s with hash %s"%i"%s).)", logger::COLOR_YELLOW, a_Path, logger::COLOR_WHITE, logger::COLOR_YELLOW, hash, logger::COLOR_WHITE);
			a_Hash = hash;
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		logger::log_success("<SoundSystem> Hash already existed. Returning existing hash (%s\"%i\"%s).", logger::COLOR_YELLOW, hash, logger::COLOR_WHITE);
		a_Hash = hash;
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Unloads a sound.
	/// </summary>
	/// <param name="a_Hash">The sound hash.</param>
	UAUDIO_RESULT SoundSystem::UnloadSound(const UAUDIO_DEFAULT_HASH a_Hash)
	{
		bool exists = false;
		DoesSoundExist(exists, a_Hash);
		if (exists)
		{
			m_Sounds.erase(a_Hash);
			logger::log_success("<SoundSystem> Unloaded sound successfully (%s\"%i\"%s).", logger::COLOR_YELLOW, a_Hash, logger::COLOR_WHITE);
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		logger::log_info("<SoundSystem> Failed unloading sound because it did not exist (%s\"%i\"%s).", logger::COLOR_YELLOW, a_Hash, logger::COLOR_WHITE);
		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_FOUND;
	}

	/// <summary>
	/// Finds the sound.
	/// </summary>
	/// <param name="a_Hash">The sound hash.</param>
	/// <returns>Returns the sound, returns nullptr if it did not exist.</returns>
	UAUDIO_RESULT SoundSystem::FindSound(WaveFormat*& a_WaveFormat, const UAUDIO_DEFAULT_HASH a_Hash)
	{
		bool exists = false;
		DoesSoundExist(exists, a_Hash);
		if (exists)
		{
			a_WaveFormat = &m_Sounds[a_Hash];
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		logger::ASSERT(false, "Hash %i not found.", a_Hash);
		a_WaveFormat = nullptr;
		return UAUDIO_RESULT::UAUDIO_ERR_SOUND_NOT_FOUND;
	}

	/// <summary>
	/// Checks whether a sound exists or not.
	/// </summary>
	/// <param name="a_Hash">The sound hash.</param>
	/// <param name="a_Exists"></param>
	/// <returns>Returns whether the sound exists or not.</returns>
	UAUDIO_RESULT SoundSystem::DoesSoundExist(bool& a_Exists, const UAUDIO_DEFAULT_HASH a_Hash) const
	{
		a_Exists = m_Sounds.find(a_Hash) != m_Sounds.end();
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Returns the amount of sounds that are currently loaded.
	/// </summary>
	/// <returns>Returns the amount of sounds that are currently loaded.</returns>
	UAUDIO_RESULT SoundSystem::SoundSize(uint32_t& a_Size) const
	{
		a_Size = static_cast<uint32_t>(m_Sounds.size());
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Collects all the sounds and returns them.
	/// </summary>
	/// <returns>Returns all the sounds in a vector.</returns>
	UAUDIO_RESULT SoundSystem::GetSounds(utils::uaudio_vector<const WaveFormat*>& a_Sounds)
	{
		for (auto &[hash, sound] : m_Sounds)
			a_Sounds.push_back(&sound);

		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Collects all the sound hashes and returns them.
	/// </summary>
	/// <returns>Returns all the sound hashes in a vector.</returns>
	UAUDIO_RESULT SoundSystem::GetSoundHashes(utils::uaudio_vector<UAUDIO_DEFAULT_HASH>& a_SoundHashes)
	{
		for (auto &[hash, sound] : m_Sounds)
			a_SoundHashes.push_back(hash);

		return UAUDIO_RESULT::UAUDIO_OK;
	}
}
