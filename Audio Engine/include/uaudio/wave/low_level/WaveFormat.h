#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <uaudio/wave/low_level/WaveReader.h>

#include <uaudio/wave/low_level/WaveChunkData.h>

#include <uaudio/Includes.h>

#include <uaudio/utils/Utils.h>

namespace uaudio
{
	/*
	 * WHAT IS THIS FILE?
	 * This is the WaveFormat class, where all the chunks are stored along with the file name.
	 * This class also is responsible for the conversion after it has been loaded in WaveReader.h.
	 * It has methods to retrieve specific chunks and can remove chunks as well.
	 *
	 * This class does not hold information such as volume, panning and pitch. It only stored the actual wave file information.
	 */
	class WaveFormat
	{
	public:
		WaveFormat() = default;
		WaveFormat(const WaveFormat &rhs);

		~WaveFormat();

		WaveFormat &operator=(const WaveFormat &rhs);

		char *m_FilePath = nullptr;

	private:
		void SetFileName(const char *a_FilePath);
		std::vector<WaveChunkData *, UAUDIO_DEFAULT_ALLOCATOR<WaveChunkData *>> m_Chunks;
		void ConfigConversion(WaveConfig &a_WaveConfig);
		void BitsPerSampleConvert(WaveConfig &a_WaveConfig);
		void MonoStereoConvert(WaveConfig &a_WaveConfig);

		friend class WaveReader;

	public:
		void RemoveChunk(const char *a_ChunkID)
		{
			for (size_t i = 0; i < m_Chunks.size(); i++)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunks[i]->chunk_id)[0], CHUNK_ID_SIZE) == 0)
				{
					UAUDIO_DEFAULT_FREE(m_Chunks[i]);
					m_Chunks.erase(m_Chunks.begin() + i);
				}
		}

		void AddChunk(WaveChunkData *a_WaveChunkData)
		{
			m_Chunks.push_back(a_WaveChunkData);
		}

		uint32_t GetChunkSize(const char *a_ChunkID) const
		{
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					return m_Chunk->chunkSize;

			return 0;
		}

		template <class T>
		T GetChunkFromData(const char *a_ChunkID) const
		{
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					return T(reinterpret_cast<T *>(utils::add(m_Chunk, sizeof(WaveChunkData))));

			return T(nullptr);
		}

		bool HasChunk(const char *a_ChunkID) const
		{
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					return true;

			return false;
		}
	};
}