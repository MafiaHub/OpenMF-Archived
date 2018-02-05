#ifndef TESTS_GENERAL_H
#define TESTS_GENERAL_H

#include <iostream>
#include <string>

unsigned int numErrors = 0;

#define ass(cond) ass(cond,#cond)

/*
  General stuff for testing.
*/

void printHeader(std::string title, char separator='=')
{
    std::cout << std::endl << title << std::endl;
    
    for (int i = 0; i < (int) title.size(); ++i)
        std::cout << separator;

    std::cout << std::endl << std::endl;
}

void printSubHeader(std::string title)
{
    printHeader(title,'-');
}

unsigned int getNumErrors()
{
    return numErrors;
}

void message(std::string msg)
{
    std::cout << "\t" << msg << std::endl;
}

bool (ass)(bool condition, std::string message)
{
    std::cout << (condition ? "[OK]" : "[ERROR]") << "\t" << message << std::endl;

    if (!condition)
        numErrors++;

    return condition;
}

#endif
