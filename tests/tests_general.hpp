#ifndef TESTS_GENERAL_H
#define TESTS_GENERAL_H

#include <iostream>

/*
  General stuff for testing.
*/

void printHeader(std::string title, char separator='-')
{
    std::cout << title << std::endl;
    
    for (int i = 0; i < (int) title.size(); ++i)
        std::cout << separator;

    std::cout << std::endl << std::endl;
}


#endif
