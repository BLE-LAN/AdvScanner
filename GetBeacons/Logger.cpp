
#include "Logger.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <windows.h>

#define DEBUG

void Logger::Log(unsigned long line, const std::string& message)
{
#ifdef DEBUG 
    // Log to Ouput window
    std::string output = "[" + currentDateTime() + "] : " + message;
    std::cout << output << "\n";
#else
    // Log to Ouput window
    std::log << "TODO" << "\n";

#endif
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