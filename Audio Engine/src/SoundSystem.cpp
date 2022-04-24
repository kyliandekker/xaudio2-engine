#include <uaudio/SoundSystem.h>

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
	UAUDIO_DEFAULT_HASH SoundSystem::LoadSound(const char *a_Path, const char *a_Name, WaveConfig &a_WaveConfig)
	{
		UAUDIO_DEFAULT_HASH hash = UAUDIO_DEFAULT_HASH_FUNCTION(a_Name);

		WaveFile wave_file = WaveFile(a_Path, a_WaveConfig);
		if (!DoesSoundExist(hash))
			m_Sounds.insert(std::make_pair(hash, wave_file));

		return hash;
	}

	/// <summary>
	/// Unloads a sound.
	/// </summary>
	/// <param name="a_Hash">The sound hash.</param>
	void SoundSystem::UnloadSound(const UAUDIO_DEFAULT_HASH a_Hash)
	{
		m_Sounds.erase(a_Hash);
	}

	/// <summary>
	/// Finds the sound.
	/// </summary>
	/// <param name="a_Hash">The sound hash.</param>
	/// <returns>Returns the sound, returns nullptr if it did not exist.</returns>
	WaveFile *SoundSystem::FindSound(const UAUDIO_DEFAULT_HASH a_Hash)
	{
		if (DoesSoundExist(a_Hash))
			return &m_Sounds[a_Hash];

		logger::ASSERT(false, "Hash %i not found.", a_Hash);
		logger::log_warning("<SoundSystem> Could not find requested sound with hash number %i. Using default sound instead.", a_Hash);

		return nullptr;
	}

	/// <summary>
	/// Checks whether a sound exists or not.
	/// </summary>
	/// <param name="a_Hash">The sound hash.</param>
	/// <returns>Returns whether the sound exists or not.</returns>
	bool SoundSystem::DoesSoundExist(const UAUDIO_DEFAULT_HASH a_Hash) const
	{
		return m_Sounds.find(a_Hash) != m_Sounds.end();
	}

	/// <summary>
	/// Returns the amount of sounds that are currently loaded.
	/// </summary>
	/// <returns>Returns the amount of sounds that are currently loaded.</returns>
	uint32_t SoundSystem::SoundSize() const
	{
		return static_cast<uint32_t>(m_Sounds.size());
	}

	/// <summary>
	/// Collects all the sounds and returns them.
	/// </summary>
	/// <returns>Returns all the sounds in a vector.</returns>
	std::vector<WaveFile *, UAUDIO_DEFAULT_ALLOCATOR<WaveFile *>> SoundSystem::GetSounds()
	{
		std::vector<WaveFile *, UAUDIO_DEFAULT_ALLOCATOR<WaveFile *>> sounds;
		for (auto &[hash, sound] : m_Sounds)
			sounds.push_back(&sound);

		return sounds;
	}

	/// <summary>
	/// Collects all the sound hashes and returns them.
	/// </summary>
	/// <returns>Returns all the sound hashes in a vector.</returns>
	std::vector<UAUDIO_DEFAULT_HASH, UAUDIO_DEFAULT_ALLOCATOR<UAUDIO_DEFAULT_HASH>> SoundSystem::GetSoundHashes()
	{
		std::vector<UAUDIO_DEFAULT_HASH, UAUDIO_DEFAULT_ALLOCATOR<UAUDIO_DEFAULT_HASH>> sounds;
		for (auto &[hash, sound] : m_Sounds)
			sounds.push_back(hash);

		return sounds;
	}
}
