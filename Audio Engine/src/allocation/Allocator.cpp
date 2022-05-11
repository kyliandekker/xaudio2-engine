#include <uaudio/allocation/Allocator.h>
#include <uaudio/Includes.h>

namespace uaudio
{
	void* Alloc(size_t sizeInBytes)
	{
		return UAUDIO_DEFAULT_ALLOC(sizeInBytes);
	}

	void* Realloc(void* ptr, size_t sizeInBytes)
	{
		return UAUDIO_DEFAULT_REALLOC(ptr, sizeInBytes);
	}

	void Free(void* ptr)
	{
		UAUDIO_DEFAULT_FREE(ptr);
	}
}
