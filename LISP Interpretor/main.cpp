//
//  main.cpp
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 2/22/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#include <iostream>
#include <cstring>

int main(int argc, const char * argv[])
{

    char str[] ="- This, a sample string.";
    char * pch;
    std::cout << "Splitting string" << str << " into tokens:\n";
    pch = strtok (str," ,.-");
    while (pch != NULL)
    {
        std::cout << pch << "\n" ;
        pch = strtok (NULL, " ,.-");
    }
    return 0;
}

