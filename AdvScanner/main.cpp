

#include <string>
#include <iostream>
#include <vector>

/*#include "Watcher.h"

int main(int argc, char* argv[]) 
{
    // Arguments
    unsigned int scantime = 0;
    std::string outputFile = "ble.txt";

    // Start the scanner
    Watcher::Run(scantime, outputFile);
}*/

#include <iostream>
#include <strings_util.h>
#include <paramkit.h>
#include "Watcher.h"

#define PARAM_MY_DEC "scantime"
#define PARAM_MY_ASTRING "fileoutput"

#define MAX_BUF 50

//---

using namespace paramkit;

typedef struct {
    char filepath[MAX_BUF];
    unsigned int scantime;
} t_params_struct;

class DemoParams : public Params
{
public:
    DemoParams()
        : Params()
    {
        this->addParam(new IntParam(PARAM_MY_DEC, true));
        this->setInfo(PARAM_MY_DEC, "Seconds to run the scanner");

        this->addParam(new StringParam(PARAM_MY_ASTRING, true));
        this->setInfo(PARAM_MY_ASTRING, "Scan output file path");
    }

    bool fillStruct(t_params_struct& paramsStruct)
    {
        IntParam* myDec = dynamic_cast<IntParam*>(this->getParam(PARAM_MY_DEC));
        if (myDec) paramsStruct.scantime = myDec->value;

        StringParam* myStr = dynamic_cast<StringParam*>(this->getParam(PARAM_MY_ASTRING));
        if (myStr) {
            myStr->copyToCStr(paramsStruct.filepath, _countof(paramsStruct.filepath));
        }
        return true;
    }
};

int main(int argc, char* argv[])
{
    DemoParams params;
    if (argc < 2) {
        paramkit::print_in_color(CYAN, "Welcome to AdvScanner!\n\n");
        params.info(false);
        return 0;
    }
    if (!params.parse(argc, argv)) {
        return 0;
    }
    paramkit::print_in_color(MAGENTA, "\nFilled params:\n");
    params.print();

    t_params_struct p;
    params.fillStruct(p);

    if (!params.hasRequiredFilled()) {
        std::cout << "[!] Some of the required parameters are not filled!\n";
    }
    else {
        paramkit::print_in_color(DARK_GREEN, "\n[+] All the required parameters filled!\n\n");

        Watcher::Run(p.scantime*1000, p.filepath);
    }
    return 0;
}