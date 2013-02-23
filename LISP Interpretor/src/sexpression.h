//
//  sexpression.h
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 2/22/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#ifndef __LISP_Interpretor__sexpression__
#define __LISP_Interpretor__sexpression__

#include <iostream>
#include <cstring>

class SExp
{
    
public:
    SExp* getCAR()          {return car_;           }
    SExp* getCDR()          {return cdr_;           }
    int getIntegerID()      {return intVal_;        }
    char* getStringID()     {return stringVal_;     }
    bool isAtom()           {return isAtom_;        }
    bool isString()         {return isString_;      }
    bool isNull()           {return isNull_;        }
    //Functions that will be defined in headers
    SExp();
    SExp(int val);
    SExp(char* str);
    SExp(SExp* cr,SExp* cd);
    
private:
    SExp* car_;
    SExp* cdr_;
    int intVal_;
    char* stringVal_;
    bool isAtom_;
    bool isString_;
    bool isNull_;
};

#endif /* defined(__LISP_Interpretor__sexpression__) */
