
#include <strings_util.h>
#include <paramkit.h>

#include <string>
#include <iostream>
#include <vector>

#include "Watcher.h"

int main(int argc, char* argv[]) 
{
    // Arguments
    unsigned int scantime = 0;
    std::string outputFile = "ble.txt";

    // Start the scanner
    Watcher::Run(scantime, outputFile);
}