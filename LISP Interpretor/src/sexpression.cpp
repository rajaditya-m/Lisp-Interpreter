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
    isCAR_ = false;
	parent_ = NULL;
}

SExp::SExp(int val)
{
    car_= cdr_= NULL;
    intVal_ = val;
    isAtom_ = true;
    isString_ = false;
    isCAR_ = false;
	parent_ = NULL;
}

SExp::SExp(string str)
{
    car_= cdr_= NULL;
    stringVal_ = str;
    isAtom_ = true;
    isString_ = true;
    isCAR_ = false;
	parent_ = NULL;
}

SExp::SExp(SExp* cr,SExp* cd)
{
    car_= cr;
    cdr_= cd;
    isAtom_ = false;
    isString_ = false;
    isCAR_ = false;
	parent_ = NULL;
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


bool SExp::eqByName(const std::string name)
{
    return !(std::strcmp(stringVal_.c_str(),name.c_str()));
}

bool SExp::getPureEquality(SExp* op)
{
    if(isAtom_ && op->isAtom())
    {
        if(isString_)
        {
            if(op->isString())
            {
                if(!(strcmp(stringVal_.c_str(), op->getStringID().c_str())))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        else
        {
            if(op->isString())
                return false;
            else
            {
                if(intVal_ == op->getIntegerID())
                    return true;
                else
                    return false;
            }
        }
    }
    return false;
}

void SExp::toListFormat()
{
    if(isAtom_)
    {
        if(isString_)
            std::cout << stringVal_ ;
        else
            std::cout << intVal_ ;
        return ;
    }
    else if(isNull())
    {
        return;
    }
    else
    {
        std::cout << "(" ;
        if(car_)
            car_->toListFormat();
        std::cout << " ";
        if(cdr_)
            cdr_->toListFormat();
        std::cout << ")";
    }
}

