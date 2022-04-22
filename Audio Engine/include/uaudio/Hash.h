#pragma once

namespace uaudio
{
	typedef unsigned int Hash;

	/// <summary>
	/// Returns a hash from a given string.
	/// </summary>
	/// <param name="a_String">The string which will be counted.</param>
	/// <returns>Sum of each character inside the string.</returns>
	inline Hash GetHash(const char *a_String)
	{
		unsigned int value = 0;
		const char *next = a_String;
		next++;
		while (*a_String != '\0')
			value += static_cast<unsigned int>(*a_String++) * static_cast<unsigned int>(*next++);
		return value;
	}

	/// <summary>
	/// Returns a hash from a given string.
	/// </summary>
	/// <param name="a_String">The string which will be counted.</param>
	/// <returns>Sum of each character inside the string.</returns>
	inline Hash GetHash(unsigned char *a_String)
	{
		unsigned int value = 0;
		unsigned char *next = a_String;
		next++;
		while (*a_String != '\0')
			value += static_cast<unsigned int>(*a_String++) * static_cast<unsigned int>(*next++);
		return value;
	}
}