﻿#pragma once
#include <string>

namespace uaudio
{
	typedef unsigned int Hash;

	/// <summary>
	/// Returns a hash from a given string.
	/// </summary>
	/// <param name="a_String">The string which will be counted.</param>
	/// <returns>Sum of each character inside the string.</returns>
	constexpr inline Hash GetHash(const char* a_String)
	{
		unsigned int value = 0;
		const char* next = a_String; next++;
		while (*a_String != '\0')
			value += static_cast<unsigned int>(*a_String++) * static_cast<unsigned int>(*next++);
		return value;
	}

	inline Hash GetHash(const std::string& a_String)
	{
		return GetHash(a_String.c_str());
	}
}