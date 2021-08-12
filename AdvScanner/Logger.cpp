
#include <iostream>

#include "Logger.hpp"

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
