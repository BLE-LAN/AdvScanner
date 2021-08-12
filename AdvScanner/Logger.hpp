
#pragma once

#include <string>

#define logError(source, message) Logger::Log(source, 1, message)
#define logInfo(source, message) Logger::Log(source, 0, message)

namespace Logger 
{
	void Log(std::string source, int logType, std::string message);
};
