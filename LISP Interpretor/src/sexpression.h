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
using namespace std;

class SExp
{
    
public:
    //Accessor Methods
    SExp* getCAR()          {return car_;                                           }
    SExp* getCDR()          {return cdr_;                                           }
    SExp* getParent()       {return parent_;                                        }
    int getIntegerID()      {return intVal_;                                        }
    string getStringID()    {return stringVal_;                                     }
    bool isAtom()           {return isAtom_;                                        }
    bool isString()         {return isString_;                                      }
    bool isNull()           {return !(std::strcmp(stringVal_.c_str(),"NIL"));       }
    bool isCAR()            {return isCAR_;                                         }
    //Mutator Methods
    void setStringID(std::string stringVal__) { stringVal_ = stringVal__;       }
    void setIntegerID(int intVal__)           { intVal_ = intVal__;             }
    void setIsAtom(bool isAtom__)             { isAtom_ = isAtom__;             }
    void setIsString(bool isString__)         { isString_ = isString__;         }
    void setCAR(SExp* car__)                  { car_ = car__;                   }
    void setCDR(SExp* cdr__)                  { cdr_ = cdr__;                   }
    void setParent(SExp* parent__)            { parent_ = parent__;             }
    void setIsCAR(bool isCar__)               { isCAR_ =isCar__;                }
    //Functions that will be defined in headers
    SExp();
    SExp(int val);
    SExp(string str);
    SExp(SExp* cr,SExp* cd);
    void toString();
    
private:
    SExp* car_;
    SExp* cdr_;
    SExp* parent_;
    int intVal_;
    string stringVal_;
    bool isAtom_;
    bool isString_;
    bool isCAR_;
};

#endif /* defined(__LISP_Interpretor__sexpression__) */
