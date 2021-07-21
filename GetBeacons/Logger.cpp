
#include "Logger.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <windows.h>

void Logger::Log(unsigned long line, const std::string& message)
{
    std::string output = "[" + currentDateTime() + "][L:" + std::to_string(line) + "] -> " + message;
    std::cout << output << "\n";
}

std::string Logger::currentDateTime() 
{
    struct tm newtime;
    time_t now = time(0);
    localtime_s(&newtime, &now);

    std::ostringstream oss;
    oss << std::put_time(&newtime, "%d-%m-%Y %H-%M-%S");
    
    return oss.str();
}