//
//  satoms.cpp
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 2/22/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#include "sexpression.h"

SExp::SExp()
{
    car_= cdr_= NULL;
    stringVal_ = NULL;
    isAtom_ = false;
    isString_ = false;
    isNull_ = true;
}

SExp::SExp(int val)
{
    car_= cdr_= NULL;
    stringVal_ = NULL;
    intVal_ = val;
    isAtom_ = true;
    isString_ = false;
    isNull_ = false;
}

SExp::SExp(char* str)
{
    car_= cdr_= NULL;
    stringVal_ = str;
    isAtom_ = true;
    isString_ = true;
    isNull_ = false;
}

SExp::SExp(SExp* cr,SExp* cd)
{
    car_= cr;
    cdr_= cd;
    stringVal_ = NULL;
    isAtom_ = false;
    isString_ = false;
    isNull_ = false;
}


