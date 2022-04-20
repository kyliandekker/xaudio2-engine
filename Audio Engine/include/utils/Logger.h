#pragma once

#include <cassert>
#include <stdio.h>

#include "UserInclude.h"

constexpr uint32_t MAX_BUFFER_LOGGER = 256;

namespace uaudio::logger
{
	/// <summary>
	/// Logs a message to console with a verbosity flag.
	/// </summary>
	/// <param name="a_Flag">Verbosity flag.</param>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename... Args>
	void log_to_console(const char *a_Flag, const char *a_Format, Args... a_Args)
	{
		char buffer[MAX_BUFFER_LOGGER] = {};

		snprintf(buffer, MAX_BUFFER_LOGGER, a_Format, a_Args...);

		printf("%s: %s\n", a_Flag, buffer);
	}

#ifdef UAUDIO_NO_COLOR_LOGGING
	constexpr auto COLOR_YELLOW = "";
	constexpr auto COLOR_WHITE = "";
	constexpr auto COLOR_GREEN = "";
	constexpr auto COLOR_RED = "";
	constexpr auto COLOR_CYAN = "";
	constexpr auto COLOR_PURPLE = "";
	constexpr auto COLOR_BLUE = "";
#else
	constexpr auto COLOR_YELLOW = "\033[0;33m";
	constexpr auto COLOR_WHITE = "\033[0;37m";
	constexpr auto COLOR_GREEN = "\033[0;32m";
	constexpr auto COLOR_RED = "\033[31m";
	constexpr auto COLOR_CYAN = "\033[0;36m";
	constexpr auto COLOR_PURPLE = "\033[0;35m";
	constexpr auto COLOR_BLUE = "\033[0;34m";
#endif

	inline void print_yellow()
	{
		printf("%s", COLOR_YELLOW);
	}

	inline void print_white()
	{
		printf("%s", COLOR_WHITE);
	}

	inline void print_green()
	{
		printf("%s", COLOR_GREEN);
	}

	inline void print_red()
	{
		printf("%s", COLOR_RED);
	}

	inline void print_cyan()
	{
		printf("%s", COLOR_CYAN);
	}

	inline void print_purple()
	{
		printf("%s", COLOR_PURPLE);
	}

	/// <summary>
	/// Logs an info message to the console.
	/// </summary>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename... Args>
	void log_info(const char *a_Format, Args... a_Args)
	{
		char buffer[MAX_BUFFER_LOGGER] = {};
		snprintf(buffer, MAX_BUFFER_LOGGER, "%s[%s]%s", COLOR_WHITE, "INFO", COLOR_WHITE);

		log_to_console(buffer, a_Format, a_Args...);
	}

	/// <summary>
	/// Logs an error message to the console.
	/// </summary>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename... Args>
	void log_error(const char *a_Format, Args... a_Args)
	{
		char buffer[MAX_BUFFER_LOGGER] = {};
		snprintf(buffer, MAX_BUFFER_LOGGER, "%s[%s]%s", COLOR_RED, "ERROR", COLOR_WHITE);

		log_to_console(buffer, a_Format, a_Args...);
	}

	/// <summary>
	/// Logs an error message to the console.
	/// </summary>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename... Args>
	void log_warning(const char *a_Format, Args... a_Args)
	{
		char buffer[MAX_BUFFER_LOGGER] = {};
		snprintf(buffer, MAX_BUFFER_LOGGER, "%s[%s]%s", COLOR_YELLOW, "WARNING", COLOR_WHITE);

		log_to_console(buffer, a_Format, a_Args...);
	}

	/// <summary>
	/// Logs a success message to the console.
	/// </summary>
	/// <param name="a_Format">The message that will get printed.</param>
	/// <param name="a_Args">Optional arguments that will replace formatting.</param>
	template <typename... Args>
	void log_success(const char *a_Format, Args... a_Args)
	{
		char buffer[MAX_BUFFER_LOGGER] = {};
		snprintf(buffer, MAX_BUFFER_LOGGER, "%s[%s]%s", COLOR_GREEN, "SUCCESS", COLOR_WHITE);

		log_to_console(buffer, a_Format, a_Args...);
	}

	/// <summary>
	/// Check for unintended behaviour. If a_Check is false, the program will stop and post a message.
	/// </summary>
	/// <param name="a_Check">If false, the program will print the message and assert.</param>
	/// <param name="a_Message">The message that will be printed.</param>
	/// <param name="a_Args">The arguments for the message.</param>
	template <typename... Args>
	inline void ASSERT(bool a_Check, const char* a_Message, Args... a_Args)
	{
		if (!a_Check)
		{
			log_error(a_Message, a_Args...);
			assert(false);
		}
	}
}