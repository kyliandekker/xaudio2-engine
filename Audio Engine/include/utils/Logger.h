#pragma once

#include <stdio.h>

#define MAX_BUFFER_LOGGER 256

namespace uaudio::logger
{
	/// <summary>
	/// Logs a message to console with a verbosity flag.
	/// </summary>
	/// <param name="a_Flag">Verbosity flag.</param>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename ...Args>
	void log_to_console(const char* a_Flag, const char* a_Format, Args ...a_Args)
	{
		char buffer[256] = {};

		snprintf(buffer, MAX_BUFFER_LOGGER, a_Format, a_Args...);

		printf("%s: %s\n", a_Flag, buffer);
	}

	/// <summary>
	/// Logs an info message to the console.
	/// </summary>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename ...Args>
	void log_info(const char* a_Format, Args ...a_Args)
	{
		log_to_console("[Info]", a_Format, a_Args...);
	}

	/// <summary>
	/// Logs an error message to the console.
	/// </summary>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename ...Args>
	void log_error(const char* a_Format, Args ...a_Args)
	{
		log_to_console("[Error]", a_Format, a_Args...);
	}
}
