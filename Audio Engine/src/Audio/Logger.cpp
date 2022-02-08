#include <iostream>

#include "Audio/Logger.h"

namespace
{
	logger::Verbosity logVerbosity = logger::Verbosity::Info;
}

namespace logger
{

	namespace log
	{
		void LogToConsole(const char* message)
		{
			printf("%s", message);
		}
	}

	void SetLogVerbosity(::logger::Verbosity verbosity)
	{
		logVerbosity = verbosity;
	}

	void LogInfo(const char* const message) { LogMessage(message, Verbosity::Info, "[Info]:"); }
	void LogWarning(const char* const message) { LogMessage(message, Verbosity::Warning, "[Warning]:"); }
	void LogError(const char* const message) { LogMessage(message, Verbosity::Error, "[Error]:"); }

	void LogMessage(const char* const message, Verbosity verbosity, const char* const flag)
	{
		if (static_cast<int>(verbosity) <= static_cast<int>(logVerbosity))
		{
			if (flag == nullptr)
				printf("%s\n", message);
			else
				printf("%s %s\n", flag, message);
		}
	}

	void LogClear() { system("cls"); }

}