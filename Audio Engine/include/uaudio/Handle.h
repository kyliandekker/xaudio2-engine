#pragma once

#include <cstdint>

namespace uaudio
{
	constexpr int32_t SOUND_NULL_HANDLE = -1;

	struct ChannelHandle
	{
		ChannelHandle() = default;
		ChannelHandle(const int32_t rhs) { m_Handle = rhs; }
		ChannelHandle(const ChannelHandle& rhs) { m_Handle = rhs; }
		~ChannelHandle() = default;

		ChannelHandle& operator=(const ChannelHandle& rhs) { m_Handle = rhs; return *this; }

		operator int32_t() const
		{
			return m_Handle;
		}

		ChannelHandle& operator=(const int32_t a_Rhs)
		{
			m_Handle = a_Rhs;
			return *this;
		}

		/// <summary>
		/// Retrieves the validity of the handle.
		/// </summary>
		/// <returns>Returns whether the handle is valid.</returns>
		bool IsValid() const
		{
			return m_Handle != SOUND_NULL_HANDLE;
		}

	protected:
		int32_t m_Handle = SOUND_NULL_HANDLE;
	};
}