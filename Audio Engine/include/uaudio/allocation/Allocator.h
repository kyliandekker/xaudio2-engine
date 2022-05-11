#pragma once

#include <cstdint>
#include <utility>

namespace uaudio
{
	void* Alloc(size_t sizeInBytes);
	void* Realloc(void* ptr, size_t sizeInBytes);
	void Free(void* ptr);

	template <typename T, typename... Args>
	T* new_object(Args&&... args)
	{
		T* object = static_cast<T*>(Alloc(sizeof(T)));
		new (object) T(std::forward<Args>(args)...);
		return object;
	}

	template <typename T>
	void delete_object(T* object) noexcept
	{
		if (object)
		{
			object->~T();
			Free(object);
		}
	}
	/*
		Implements a subset of Allocator named requirement
	*/
	template <typename T>
	struct UAllocator
	{
		using value_type = T;

		UAllocator() = default;
		~UAllocator() = default;

		template <typename U>
		constexpr UAllocator(const UAllocator<U>&) noexcept {}

		T* allocate(size_t n)
		{
			T* ptr = static_cast<T*>(Alloc(n * sizeof(value_type)));
			return ptr;
		}

		void deallocate(T* ptr, size_t) noexcept
		{
			Free(ptr);
		}
	};

	template <class T, class U>
	bool operator==(const UAllocator<T>&, const UAllocator<U>&) { return true; }
	template <class T, class U>
	bool operator!=(const UAllocator<T>&, const UAllocator<U>&) { return false; }

	template <typename T>
	using UAUDIO_Allocator = UAllocator<T>;
}
