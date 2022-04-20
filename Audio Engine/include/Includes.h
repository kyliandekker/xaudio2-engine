#pragma once

#include "Hash.h"
#include <cstdlib>

namespace uaudio
{
	inline int compare(unsigned char* a, unsigned char* b, int size)
	{
		while (size-- > 0)
		{
			if (*a != *b)
				return (*a < *b) ? -1 : 1;
			a++; b++;
		}
		return 0;
	}

	inline int compare(unsigned char* a, const char* b, int size)
	{
		while (size-- > 0)
		{
			if (*a != *b)
				return (*a < *b) ? -1 : 1;
			a++; b++;
		}
		return 0;
	}

	enum class BUFFERSIZE
	{
		BUFFERSIZE_256 = 256,
		BUFFERSIZE_384 = 384,
		BUFFERSIZE_512 = 512,
		BUFFERSIZE_1024 = 1024,
		BUFFERSIZE_2048 = 2048,
		BUFFERSIZE_4096 = 4096,
		BUFFERSIZE_8192 = 8192,
	};

	enum class TIMEUNIT
	{
		TIMEUNIT_MS,
		TIMEUNIT_S,
		TIMEUNIT_POS
	};

#define UAUDIO_DEFAULT_ALLOCATOR std::allocator

#define UAUDIO_DEFAULT_ALLOC malloc
#define UAUDIO_DEFAULT_FREE free

constexpr float UAUDIO_DEFAULT_VOLUME = 1.0f;
constexpr float UAUDIO_DEFAULT_PANNING = 0.0f;
constexpr uaudio::BUFFERSIZE UAUDIO_DEFAULT_BUFFERSIZE = uaudio::BUFFERSIZE::BUFFERSIZE_8192;

#define UAUDIO_DEFAULT_HASH uaudio::Hash
#define UAUDIO_DEFAULT_HASH_FUNCTION uaudio::GetHash

}

#include "UserInclude.h"