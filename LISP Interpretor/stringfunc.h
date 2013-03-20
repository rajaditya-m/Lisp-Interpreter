//
//  stringfunc.h
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 3/12/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#ifndef LISP_Interpretor_stringfunc_h
#define LISP_Interpretor_stringfunc_h

#include <string>
#include <sstream>

bool isNumber(const std::string & s)
{
    if(s.empty() || ((!std::isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false ;
    char * p ;
    std::strtol(s.c_str(), &p, 10) ;
    return (*p == 0) ;
}

int convertToNumber(const std::string& s)
{
    std::istringstream ss(s);
    int result;
    ss >> result ;
    return result;
}

int matchingBrace(const std::string & s,int *startCount)
{
    unsigned long len = s.length();
    unsigned int i ;
    for ( i = 0;i<len ; i++)
    {
        char x = s.at(i);
		if(x ==' ' || x=='\t')
			continue;
        if (x == '(')
            (*startCount)++;
        else if(x == ')')
            (*startCount)--;
        if((*startCount) == 0)
            return i;
    }
    return -1;
}

strErrCode checkSanityOfToken(const std::string & s)
{
    long len = s.length();
    for(int i=0;i<len;i++)
    {
        char x = s.at(i);
        if((x>=65 && x<=90) || (x>=48 && x<=57) || x=='-' || x=='+')
            continue;
        else if(x>=97 && x<=122)
            return S_LC;
        else
            return S_SPC;
    }
    return S_OK;
}

#endif
