#include "allocator/UAudioAllocator.h"
#include "Includes.h"

#include <cstdlib>

void* Alloc(uint64_t sizeInBytes)
{
	return malloc(sizeInBytes);
}

void Free(void* ptr)
{
	free(ptr);
}