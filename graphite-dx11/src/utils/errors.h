#pragma once

namespace logging
{
	enum class Level
	{
		Info,
		Warning,
		Error,
		Fatal
	};

	void Write(Level level, const char* context, const char* message);
}

namespace window
{
    void LogLastError(const char* context);
	void LogInfo(const char* context, const char* message);
}
