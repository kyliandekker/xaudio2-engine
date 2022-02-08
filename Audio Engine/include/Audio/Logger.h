#pragma once

namespace logger
{
	namespace log
	{
		// probably going to replace this with the other functions in this header
		void LogToConsole(const char* message);
	}

	enum class Verbosity
	{
		None = 0,
		Error,
		Warning,
		Info
	};

	void SetLogVerbosity(Verbosity verbosity);

	void LogInfo(const char* const message);

	void LogWarning(const char* const message);

	void LogError(const char* const message);

	void LogMessage(const char* const message, Verbosity verbosity, const char* const flag = nullptr);

	void LogClear();
}