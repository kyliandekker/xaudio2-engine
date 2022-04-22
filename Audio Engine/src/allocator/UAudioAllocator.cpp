#include <uaudio/allocator/UAudioAllocator.h>
#include <uaudio/Includes.h>

#include <cstdlib>

void* Alloc(uint64_t sizeInBytes)
{
	return UAUDIO_DEFAULT_ALLOC(sizeInBytes);
}

void Free(void* ptr)
{
	UAUDIO_DEFAULT_FREE(ptr);
}