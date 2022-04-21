#pragma once

// Necessary to override all the default settings.
#include "UserInclude.h"

#if !defined(UAUDIO_DEFAULT_HASH)
#include "Hash.h"

#define UAUDIO_DEFAULT_HASH uaudio::Hash
#define UAUDIO_DEFAULT_HASH_FUNCTION uaudio::GetHash
#endif

#if !defined(UAUDIO_DEFAULT_ALLOCATOR)

#include <cstdlib>

#define UAUDIO_DEFAULT_ALLOCATOR std::allocator

#endif

#if !defined(UAUDIO_DEFAULT_ALLOC)

#include <cstdlib>

#define UAUDIO_DEFAULT_ALLOC malloc

#endif

#if !defined(UAUDIO_DEFAULT_FREE)

#include <cstdlib>

#define UAUDIO_DEFAULT_FREE free

#endif

#if !defined(UAUDIO_DEFAULT_MEMCOPY)

#include <cstdio>
#include <string.h>

#define UAUDIO_DEFAULT_MEMCOPY memcpy

#endif

namespace uaudio
{
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

}

#if !defined(UAUDIO_DEFAULT_BUFFERSIZE)

#define UAUDIO_DEFAULT_BUFFERSIZE uaudio::BUFFERSIZE::BUFFERSIZE_8192

#endif

#define UAUDIO_MAX_PANNING 1.0f
#define UAUDIO_MIN_PANNING (-1.0f)

#define UAUDIO_MAX_VOLUME 1.0f
#define UAUDIO_MIN_VOLUME (-1.0f)

#if !defined(UAUDIO_DEFAULT_VOLUME)

#define UAUDIO_DEFAULT_VOLUME UAUDIO_MAX_VOLUME

#endif

#if !defined(UAUDIO_DEFAULT_PANNING)

#define UAUDIO_DEFAULT_PANNING 0.0f

#endif