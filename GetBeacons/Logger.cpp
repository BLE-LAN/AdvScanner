
#include "Logger.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <windows.h>

/*
    Console ouput logs.
    Format:
        [SOURCE][INFO/ERROR] : text
        info -> green
        error -> red
*/


void setColor()
{

}

void Logger::Log(std::string source, int logType, std::string message)
{    
    if (logType == 0) 
    {
        std::clog << "[" + source + "][INFO] : " + message << std::endl;
    }
    else {
        std::cerr << "[" + source + "][ERROR] : " + message << std::endl;
    }
}

/*std::string Logger::currentDateTime()
{
    struct tm newtime;
    time_t now = time(0);
    localtime_s(&newtime, &now);

    std::ostringstream oss;
    oss << std::put_time(&newtime, "%d-%m-%Y %H-%M-%S");
    
    return oss.str();
}*/