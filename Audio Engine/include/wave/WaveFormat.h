﻿#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace uaudio
{
	constexpr auto CHUNK_ID_SIZE = 4;

	inline void *Add(void *ptr, size_t size)
	{
		return reinterpret_cast<unsigned char *>(ptr) + size;
	}
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

		std::vector<Chunk_Data*> m_Chunks;

		uint32_t GetChunkSize(const char *a_ChunkID) const
		{
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char*>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					return m_Chunk->chunkSize;

			return 0;
		}

		template <class T>
		T GetChunkFromData(const char *a_ChunkID) const
		{
			for (auto* m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char*>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					return T(reinterpret_cast<T*>(Add(m_Chunk, sizeof(Chunk_Data))));

			return T(nullptr);
		}

		bool HasChunk(const char *a_ChunkID) const
		{
			for (auto* m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char*>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					return true;

			return false;
		}
	};
}