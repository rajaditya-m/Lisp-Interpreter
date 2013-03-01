//
//  satoms.cpp
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 2/22/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#include <iostream>
#include "sexpression.h"

SExp::SExp()
{
    car_= cdr_= NULL;
    isAtom_ = false;
    isString_ = false;
}

SExp::SExp(int val)
{
    car_= cdr_= NULL;
    intVal_ = val;
    isAtom_ = true;
    isString_ = false;
}

SExp::SExp(string str)
{
    car_= cdr_= NULL;
    stringVal_ = str;
    isAtom_ = true;
    isString_ = true;
}

SExp::SExp(SExp* cr,SExp* cd)
{
    car_= cr;
    cdr_= cd;
    isAtom_ = false;
    isString_ = false;
}

void SExp::toString()
{
    if(isAtom_)
    {
        if(isString_)
            std::cout << stringVal_ ;
        else
            std::cout << intVal_ ;
        return ;
    }
    std::cout << "(" ;
    if(car_)
        car_->toString();
    std::cout << ".";
    if(cdr_)
        cdr_->toString();
    std::cout << ")";
}


