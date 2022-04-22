#pragma once

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

	enum class LOOP_POINT_SETTING
	{
		LOOP_POINT_SETTING_NONE,
		LOOP_POINT_SETTING_START,
		LOOP_POINT_SETTING_END,
		LOOP_POINT_SETTING_BOTH,
	};
}

// Necessary to override all the default settings.
#include <uaudio/UserInclude.h>

#if !defined(UAUDIO_DEFAULT_HASH)
#include <uaudio/Hash.h>
#endif

#if !defined(UAUDIO_DEFAULT_ALLOCATOR)
#include <cstdlib>
#endif

#if !defined(UAUDIO_DEFAULT_ALLOC)
#include <cstdlib>
#endif

#if !defined(UAUDIO_DEFAULT_FREE)
#include <cstdlib>
#endif

#if !defined(UAUDIO_DEFAULT_MEMCPY)
#include <cstdio>
#include <string.h>
#endif

namespace uaudio
{
#if !defined(UAUDIO_DEFAULT_HASH)
#define UAUDIO_DEFAULT_HASH Hash
#define UAUDIO_DEFAULT_HASH_FUNCTION GetHash
#endif

#if !defined(UAUDIO_DEFAULT_ALLOCATOR)
#define UAUDIO_DEFAULT_ALLOCATOR std::allocator
#endif

#if !defined(UAUDIO_DEFAULT_ALLOC)
#define UAUDIO_DEFAULT_ALLOC malloc
#endif

#if !defined(UAUDIO_DEFAULT_FREE)
#define UAUDIO_DEFAULT_FREE free
#endif

#if !defined(UAUDIO_DEFAULT_MEMCPY)
#define UAUDIO_DEFAULT_MEMCPY memcpy
#endif

#if !defined(UAUDIO_DEFAULT_BUFFERSIZE)

	#define UAUDIO_DEFAULT_BUFFERSIZE BUFFERSIZE::BUFFERSIZE_8192

#endif

	#define UAUDIO_MAX_PANNING 1.0f
	#define UAUDIO_MIN_PANNING -1.0f

	#define UAUDIO_MAX_VOLUME 1.0f
	#define UAUDIO_MIN_VOLUME -1.0f

#if !defined(UAUDIO_DEFAULT_VOLUME)

	#define UAUDIO_DEFAULT_VOLUME UAUDIO_MAX_VOLUME

#endif

#if !defined(UAUDIO_DEFAULT_PANNING)

	#define UAUDIO_DEFAULT_PANNING 0.0f

#endif
}