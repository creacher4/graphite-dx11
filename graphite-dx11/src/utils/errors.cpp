#include "errors.h"
#include <Windows.h>
#include <string>

void logging::Write(Level level, const char* context, const char* message)
{
	const char* levelStr = "";

	switch (level)
	{
	case Level::Info:
		levelStr = "INFO";
		break;
	case Level::Warning:
		levelStr = "WARNING";
		break;
	case Level::Error:
		levelStr = "ERROR";
		break;
	case Level::Fatal:
		levelStr = "FATAL";
		break;
	}

	std::string output = "[";
	output += levelStr;
	output += "] ";
	output += context;
	output += ": ";
	output += message;
	output += "\n";

	OutputDebugStringA(output.c_str());
}


void window::LogLastError(const char* context)
{
	DWORD error = GetLastError();
	if (error == 0) return;

	LPSTR message = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		error,
		0,
		(LPSTR)&message,
		0,
		nullptr);

	if (message)
	{
		logging::Write(logging::Level::Error, context, message);
		LocalFree(message);
	}
}

void window::LogInfo(const char* context, const char* message)
{
	logging::Write(logging::Level::Info, context, message);
}

