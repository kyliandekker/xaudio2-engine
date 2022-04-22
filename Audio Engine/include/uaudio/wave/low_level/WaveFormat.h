﻿#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <uaudio/wave/low_level/WaveReader.h>

#include <uaudio/Includes.h>

#include <uaudio/utils/Utils.h>

#include <uaudio/Defines.h>

namespace uaudio
{
#pragma pack(push, 1)
	struct Chunk_Data
	{
		unsigned char chunk_id[CHUNK_ID_SIZE] = {};
		uint32_t chunkSize = 0;
	};
#pragma pack(pop)

	class WaveFormat
	{
	public:
		WaveFormat() = default;
		WaveFormat(const WaveFormat &rhs);

		~WaveFormat();

		WaveFormat &operator=(const WaveFormat &rhs);

		std::string m_FilePath;

	private:
		std::vector<Chunk_Data *, UAUDIO_DEFAULT_ALLOCATOR<Chunk_Data *>> m_Chunks;

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

		void AddChunk(Chunk_Data* a_ChunkData)
		{
			m_Chunks.push_back(a_ChunkData);
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
					return T(reinterpret_cast<T *>(utils::add(m_Chunk, sizeof(Chunk_Data))));

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