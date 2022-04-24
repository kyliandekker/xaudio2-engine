#pragma once

#include <uaudio/Defines.h>

namespace uaudio
{
#pragma pack(push, 1)

	/*
	 * WHAT IS THIS FILE?
	 * This is a wave chunk header. Every chunk in a wave file has an ID and size.
	 * These are used in the WaveFormat class, where it holds every loaded chunk
	 * in an array.
	 *
		* The chunk id is always 4 bytes long and needs to be unique. Capitalization matters.
		* The chunk size is different for every chunk and determines how many bytes are read after this chunk header.
	 */
	struct WaveChunkData
	{
		unsigned char chunk_id[CHUNK_ID_SIZE] = {};
		uint32_t chunkSize = 0;
	};
#pragma pack(pop)
}