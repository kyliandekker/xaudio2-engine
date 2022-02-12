#pragma once

#include <stdint.h>

constexpr int32_t SOUND_NULL_HANDLE = -1;

struct Handle
{
	Handle() = default;
	Handle(const int32_t a_Handle) { m_Handle = a_Handle; }

	operator int32_t() const
	{
		return m_Handle;
	}

	Handle& operator=(const int32_t a_Rhs)
	{
		m_Handle = a_Rhs;
		return *this;
	}

	bool IsValid() const
	{
		return m_Handle != SOUND_NULL_HANDLE;
	}

protected:
	int32_t m_Handle = SOUND_NULL_HANDLE;
};