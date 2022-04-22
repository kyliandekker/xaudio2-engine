#pragma once

#include <cstdint>

#include <uaudio/Includes.h>

void* Alloc(uint64_t sizeInBytes);
void Free(void* ptr);

namespace uaudio
{
    template <typename T>
    struct UAudioAllocator
    {
        using value_type = T;

        UAudioAllocator() = default;
        ~UAudioAllocator() = default;

        template <typename U>
        constexpr UAudioAllocator(const UAudioAllocator<U> &) noexcept {}

        T *allocate(size_t n)
        {
            T *ptr = static_cast<T *>(UAUDIO_DEFAULT_ALLOC(n));
            return ptr;
        }

        void deallocate(T *ptr, size_t) noexcept
        {
            UAUDIO_DEFAULT_FREE(ptr);
        }
    };
}
