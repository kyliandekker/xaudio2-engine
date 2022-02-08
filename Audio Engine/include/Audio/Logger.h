#pragma once

namespace logger
{
	template <typename ...Args>
	void log_to_console(const char* flag, const char* format, Args ...args)
	{
		char buffer[256];

		snprintf(buffer, 256, format, args...);

		printf("%s: %s\n", flag, buffer);
	}

	template <typename ...Args>
	void log_info(const char* format, Args ...args)
	{
		log_to_console("[Info]", format, args...);
	}

	template <typename ...Args>
	void log_error(const char* format, Args ...args)
	{
		log_to_console("[Error]", format, args...);
	}
}