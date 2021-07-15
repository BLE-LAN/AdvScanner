
#pragma once

#include <string>

class Logger 
{
public:
	void Log(unsigned long line, const std::string& message);

	std::string currentDateTime();
};
