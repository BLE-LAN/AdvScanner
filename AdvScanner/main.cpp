
#include <boost/program_options.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "Watcher.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) 
{
    // Arguments
    unsigned int scantime;
    std::string outputFile;

    // Options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("scantime,s", po::value<unsigned int>(&scantime)->default_value(40*1000), "time to find ADV packets in miliseconds")
        ("output-file,o", po::value<std::string>(&outputFile)->default_value("ble.txt"), "output file path")
        ;
    
    // Parse cli arguments
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);
    
    // Check if --help is used
    if (vm.count("help"))
    {
        std::cerr << desc << "\n";
        return -1;
    }

    // Start the scanner
    Watcher::Run(scantime, outputFile);
}