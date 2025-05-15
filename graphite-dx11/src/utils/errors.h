#pragma once

#include <Windows.h>

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

namespace d3d
{
	void LogIfFailed(HRESULT hr, const char* context);
}
