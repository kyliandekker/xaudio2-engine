#pragma once

#include <cstdint>
#include <string>

#include <uaudio/wave/low_level/WaveChunkData.h>
#include <uaudio/utils/Utils.h>
#include "uaudio/system/high_level/UaudioResult.h"

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
	struct WaveConfig;
	class WaveReader;

	class WaveFormat
	{
	public:
		WaveFormat() = default;
		WaveFormat(const WaveFormat &rhs);

		~WaveFormat();

		WaveFormat &operator=(const WaveFormat &rhs);

		std::string m_FilePath;
	private:
		utils::uaudio_vector<WaveChunkData *> m_Chunks;
		void SetLoopPositions(const WaveConfig &a_WaveConfig);
		void ConfigConversion(const WaveConfig &a_WaveConfig);
		void BitsPerSampleConvert(const WaveConfig &a_WaveConfig);
		void MonoStereoConvert(const WaveConfig &a_WaveConfig);

		uint32_t m_StartPosition = 0;
		uint32_t m_EndPosition = 0;
		bool m_Looping = false;

		bool m_HasLoaded = false;

		friend class WaveReader;

	public:
		/// <summary>
		/// Whether the wave format has been loaded or not.
		/// </summary>
		/// <returns></returns>
		bool HasLoaded() const
		{
			return m_HasLoaded;
		}

		/// <summary>
		/// Removes all chunks that share the chunk id.
		/// </summary>
		/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
		UAUDIO_RESULT RemoveChunk(const char *a_ChunkID)
		{
			for (size_t i = 0; i < m_Chunks.size(); i++)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunks[i]->chunk_id)[0], CHUNK_ID_SIZE) == 0)
				{
					UAUDIO_DEFAULT_FREE(m_Chunks[i]);
					m_Chunks.erase(m_Chunks.begin() + i);
				}
			return UAUDIO_RESULT::UAUDIO_OK;
		}
		
		/// <summary>
		/// Adds a chunk to the chunk list.
		/// </summary>
		/// <param name="a_ChunkData"></param>
		/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
		/// <param name="a_Chunk">The chunk data.</param>
		template <class T>
		static UAUDIO_RESULT ConstructChunk(WaveChunkData*& a_ChunkData, const char* a_ChunkID, T& a_Chunk)
		{
			a_ChunkData = reinterpret_cast<WaveChunkData*>(UAUDIO_DEFAULT_ALLOC(sizeof(WaveChunkData) + sizeof(T)));
			UAUDIO_DEFAULT_MEMCPY(a_ChunkData->chunk_id, a_ChunkID, CHUNK_ID_SIZE);
			a_ChunkData->chunkSize = sizeof(T);
			T* custom_chunk = reinterpret_cast<T*>(utils::add(a_ChunkData, sizeof(WaveChunkData)));
			UAUDIO_DEFAULT_MEMCPY(custom_chunk, &a_Chunk, sizeof(T));
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		/// <summary>
		/// Adds a chunk to the chunk list.
		/// </summary>
		/// <param name="a_WaveChunkData">The chunk that needs to be added.</param>
		UAUDIO_RESULT AddChunk(WaveChunkData* a_WaveChunkData)
		{
			m_Chunks.push_back(a_WaveChunkData);
			return UAUDIO_RESULT::UAUDIO_OK;
		}

		/// <summary>
		/// Gets the chunk size of the first chunk that shares the chunk id.
		/// </summary>
		/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
		UAUDIO_RESULT GetChunkSize(uint32_t& a_Size, const char *a_ChunkID) const
		{
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
				{
					a_Size = m_Chunk->chunkSize;
					return UAUDIO_RESULT::UAUDIO_OK;
				}

			return UAUDIO_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}

		/// <summary>
		/// Returns the first chunk that shares the chunk id.
		/// </summary>
		/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
		template <class T>
		UAUDIO_RESULT GetChunkFromData(T& a_Type, const char *a_ChunkID) const
		{
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
				{
					a_Type = T(reinterpret_cast<T *>(utils::add(m_Chunk, sizeof(WaveChunkData))));
					return UAUDIO_RESULT::UAUDIO_OK;
				}

			return UAUDIO_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}

		/// <summary>
		/// Returns a chunk based on index.
		/// </summary>
		/// <param name="a_Data"></param>
		/// <param name="a_Index"></param>
		/// <returns></returns>
		UAUDIO_RESULT GetChunkFromData(WaveChunkData*& a_Data, uint32_t a_Index) const
		{
			a_Data = m_Chunks[a_Index];
			return UAUDIO_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}

		/// <summary>
		/// Returns the number of loaded chunks.
		/// </summary>
		/// <param name="a_Size"></param>
		/// <returns></returns>
		UAUDIO_RESULT GetNumberOfChunks(size_t& a_Size) const
		{
			a_Size = m_Chunks.size();
			return UAUDIO_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}

		/// <summary>
		/// Checks whether a chunk that shares the chunk id exists.
		/// </summary>
		/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
		UAUDIO_RESULT HasChunk(bool& a_ChunkFound, const char *a_ChunkID) const
		{
			a_ChunkFound = false;
			for (auto *m_Chunk : m_Chunks)
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char *>(m_Chunk->chunk_id)[0], CHUNK_ID_SIZE) == 0)
				{
					a_ChunkFound = true;
					return UAUDIO_RESULT::UAUDIO_OK;
				}

			return UAUDIO_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}

		UAUDIO_RESULT GetStartPosition(uint32_t &a_StartPosition) const;
		UAUDIO_RESULT GetEndPosition(uint32_t &a_EndPosition) const;

		UAUDIO_RESULT SetStartPosition(uint32_t a_StartPosition);
		UAUDIO_RESULT SetEndPosition(uint32_t a_EndPosition);

		UAUDIO_RESULT IsLooping(bool& a_Looping) const;
		UAUDIO_RESULT SetLooping(bool a_Looping);
	};
}
