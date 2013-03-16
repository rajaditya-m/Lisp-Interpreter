//
//  sanitychecks.h
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 3/16/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#ifndef LISP_Interpretor_sanitychecks_h
#define LISP_Interpretor_sanitychecks_h

#include "sexpression.h"
#include "enumstructdecs.h"

errCode checkFunctionName(SExp* fName)
{
    if(fName->isAtom())
    {
        if(fName->isString())
        {
            std::vector<std::string> freshVec(reservedWords,reservedWords+19);
            if(std::find(freshVec.begin(), freshVec.end(), fName->getStringID().c_str())!=freshVec.end())
                return T_RESERVED;
            else
                return T_OK;
        }
        else
            return T_NUM;
    }
    else
        return T_LIST;
}

errCode checkFunctionParams(SExp* fName)
{
    if(fName->isAtom())
        return T_ATOM;
    else
        return T_OK;
}

#endif
