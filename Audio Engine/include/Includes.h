#pragma once

#include "utils/Logger.h"

namespace uaudio
{
	enum class BUFFERSIZE
	{
		BUFFERSIZE_256 = 256,
		BUFFERSIZE_384 = 384,
		BUFFERSIZE_512 = 512,
		BUFFERSIZE_1024 = 1024,
		BUFFERSIZE_2048 = 2048,
	};

	enum class TIMEUNIT
	{
		TIMEUNIT_MS,
		TIMEUNIT_S,
		TIMEUNIT_POS
	};

	#include <cassert>

	/// <summary>
	/// Check for unintended behaviour. If a_Check is false, the program will stop and post a message.
	/// </summary>
	/// <param name="a_Check">If false, the program will print the message and assert.</param>
	/// <param name="a_Message">The message that will be printed.</param>
	/// <param name="a_Args">The arguments for the message.</param>
	template <typename... Args>
	inline void ASSERT(bool a_Check, const char *a_Message, Args... a_Args)
	{
		if (!a_Check)
		{
			logger::log_error(a_Message, a_Args...);
			assert(false);
		}
	}

	#define WAVE_FORMAT_PCM 1
	#define WAVE_CHANNELS_MONO 1
	#define WAVE_CHANNELS_STEREO 2
}

#define UAUDIO_DEFAULT_ALLOCATOR std::allocator

#define UAUDIO_DEFAULT_ALLOC malloc
#define UAUDIO_DEFAULT_FREE free

#define UAUDIO_DEFAULT_VOLUME 1.0f
#define UAUDIO_DEFAULT_PANNING 0.0f
#define UAUDIO_DEFAULT_BUFFERSIZE uaudio::BUFFERSIZE::BUFFERSIZE_2048

#include "Hash.h"

#define UAUDIO_DEFAULT_HASH uaudio::Hash
#define UAUDIO_DEFAULT_HASH_FUNCTION uaudio::GetHash