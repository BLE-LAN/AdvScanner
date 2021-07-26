

#include <iostream>

#include "Watcher.h"

#include <windows.h>

struct Menu_Option
{
    char choice;
    std::string text;
    void (*procesing_function)();
};

void Process_Selection_One() 
{
    Watcher::Run(1000 * 10);
}

int main()
{
    Menu_Option main_menu[] =
    {
      {'1', "1. Non stop", &Process_Selection_One},
    };

    size_t quantity_selections = sizeof(main_menu) / sizeof(main_menu[0]);

    std::string menu_title =
        "###############################\n"
        "##         ADVtoJSON         ##\n"
        "###############################\n"
        ;

    std::cout
        << menu_title << "\n"
        << "0. Quit \n";

    for (size_t i = 0; i < quantity_selections; ++i)
    {
        std::cout << main_menu[i].text << "\n";
    }

    std::cout << "\nEnter selection: ";

    unsigned int choice = 1;

    while (true)
    {
        //std::cin >> choice;

        if (choice <= quantity_selections && choice >= 0) { break; }

        // cursor up and delete the line
        std::cout << "\x1b[1A" << "\x1b[2K";
        // cursor at the beginning of line
        std::cout << '\r';
        std::cout << "Enter a valid selection: ";
    }

    system("CLS");

    void (*p_function)() = main_menu[choice - 1].procesing_function;
    (p_function)();
}