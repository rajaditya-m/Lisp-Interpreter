//
//  coreintfunc.h
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 3/12/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#ifndef LISP_Interpretor_coreintfunc_h
#define LISP_Interpretor_coreintfunc_h

#include "sexpression.h"
#include "corefunc.h"
#include "sanitychecks.h"

SExp* evlistFunc(bool* status,SExp* list,SExp* aList,SExp* dList,const SExpMap &aMap);
SExp* applyFunc(bool* status,SExp* funcName,SExp* body,SExp* aList,SExp* dList,const SExpMap &aMap);
SExp* evconFunc(bool* status,SExp* beList,SExp* aList,SExp* dList,const SExpMap &aMap);
bool searchInList(SExp* exp,SExp* aList);
SExp* getValFromList(SExp* exp,SExp* aList);
SExp* addPairs(SExp* pList, SExp* argVals, SExp* aList);

SExp* evalFunc(bool* status,SExp* exp,SExp* aList,SExp** dList,const SExpMap &aMap)
{
    if(exp->isAtom())
    {
        if(!(exp->isString()))
        {
            *status = true;
            return exp;
        }
        else if (exp->eqByName("T"))
        {
            *status = true;
            return exp;
        }
        else if (exp->eqByName("NIL"))
        {
            *status = true;
            return exp;
        }
        else if (searchInList(exp,aList))
        {
            *status = true;
            return getValFromList(exp,aList);
        }
        else
        {
            std::cout << "[ERROR] Unbound Atom ";
            exp->toString();
            std::cout << " found.\n";
            *status = false;
            return NULL;
        }
    }
    else if(exp->getCAR()->isAtom())
    {
        if(exp->getCAR()->eqByName("QUOTE"))
        {
            *status = true;
            return exp->getCDR()->getCAR();
        }
        else if(exp->getCAR()->eqByName("COND"))
        {
            bool success;
            SExp* res = evconFunc(&success,exp->getCDR(),aList,*dList,aMap);
            *status = success;
            if(success)
                return res;
            else
                return NULL;
        }
        else if(exp->getCAR()->eqByName("DEFUN"))
        {
            SExp *fName, *pList,*fBody;
            if(exp->getCDR()->isAtom())
            {
                *status = false;
                if(exp->getCDR()->isNull())
                    std::cout << "[ERROR] DEFUN Function requires three more arguments : Name, Parameter List and Function Body.\n";
                else
                    std::cout << "[ERROR] DEFUN Function requires specification of arguments as lists, not in DOT notation.\n";
                return NULL;
            }
            else
            {
                fName = exp->getCDR()->getCAR();
                if(exp->getCDR()->getCDR()->isAtom())
                {
                    *status = false;
                    if(exp->getCDR()->getCDR()->isNull())
                        std::cout << "[ERROR] DEFUN Function requires two more arguments : Parameter List and Function Body.\n";
                    else
                        std::cout << "[ERROR] DEFUN Function requires specification of arguments as lists, not in DOT notation.\n";
                    return NULL;
                }
                pList = exp->getCDR()->getCDR()->getCAR();
                if(exp->getCDR()->getCDR()->getCDR()->isAtom())
                {
                    *status = false;
                    if(exp->getCDR()->getCDR()->getCDR()->isNull())
                        std::cout << "[ERROR] DEFUN Function requires one more argument : Function Body.\n";
                    else
                        std::cout << "[ERROR] DEFUN Function requires specification of arguments as lists, not in DOT notation.\n";
                    return NULL;
                }
                fBody = exp->getCDR()->getCDR()->getCDR()->getCAR();
            }
            //Sanity check for function names.
            errCode fNameSanChk = checkFunctionName(fName);
            if(fNameSanChk!=T_OK)
            {
                *status = false;
                if(fNameSanChk==T_LIST)
                    std::cout <<"[ERROR] Function name is a list. Please give an atomic string as name.\n";
                else if(fNameSanChk==T_NUM)
                    std::cout <<"[ERROR] Function name is a number. Please give an string as name.\n";
                else if(fNameSanChk==T_RESERVED)
                    std::cout <<"[ERROR] Function name is a reserved primitive. Please select a new name.\n";
                return NULL;
            }
            //Sanity checks for Parameters
            errCode pLSanChk = checkFunctionParams(pList);
            if(pLSanChk!=T_OK)
            {
                *status = false;
                if(pLSanChk==T_ATOM)
                    std::cout <<"[ERROR] Function parameter list is an atom. Please specify it in the form of a list.\n";
                return NULL;
            }
            *status = true;
            SExp* new1 = new SExp(pList,fBody);
            SExp* entry = new SExp(fName,new1);
            
            SExp* newDList = new SExp(entry,*dList);
            *dList = newDList;
            return fName;
        }
        else
        {
            bool successOfEvlist;
            SExp* res1 = evlistFunc(&successOfEvlist,exp->getCDR(),aList,*dList,aMap);
            if(successOfEvlist)
            {
                bool successofApply;
                SExp* res2 = applyFunc(&successofApply,exp->getCAR(),res1,aList,*dList,aMap);
                *status = successofApply;
                if(successofApply)
                {
                    return res2;
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                *status = false;
                return NULL;
            }
        }
    }
    else
    {
        std::cout << "[ERROR] Expression is in an incorrect form and cannot be parsed.\n";
        *status = false;
        return NULL;
    }
}

SExp* getValFromList(SExp* exp,SExp* aList)
{
    if(aList->getCAR()->getCAR()->eqByName(exp->getStringID().c_str()))
    {
        return aList->getCAR()->getCDR();
    }
    else return getValFromList(exp, aList->getCDR());
}

SExp* evconFunc(bool* status,SExp* beList,SExp* aList,SExp* dList,const SExpMap &aMap)
{
    if(beList->isNull())
    {
        *status = false;
        std::cout << "[ERROR] COND statement must have atleast one conditional clause.\n";
        return NULL;
    }
    else
    {
        bool successOfEval;
        int nosPairs = numberOfElems(beList->getCAR());
        if(nosPairs!=2)
        {
            *status = false;
            std::cout << "[ERROR] COND statement clause is empty or not of the form (EXP EXP).\n";
            return NULL;
        }
        /*if(beList->getCAR()->isAtom())
        {
            *status = false;
            std::cout << "[ERROR] COND statement clause is empty or not of the form (EXP EXP).\n";
            return NULL;
        }*/
        SExp* res1 = evalFunc(&successOfEval, beList->getCAR()->getCAR(), aList, &dList,aMap);
        if(successOfEval)
        {
            if(!(res1->eqByName("NIL")))
            {
                bool successOf2ndEval;
                if(beList->getCAR()->getCDR()->isAtom())
                {
                    *status = false;
                    std::cout << "[ERROR] COND statement clause is not of the form (EXP EXP).\n";
                    return NULL;
                }
                SExp* res2 = evalFunc(&successOf2ndEval,beList->getCAR()->getCDR()->getCAR(),aList,&dList,aMap);
                if(successOf2ndEval)
                {
                    *status = true;
                    return res2;
                }
                else
                {
                    *status = false;
                    return NULL;
                }
            }
            else
            {
                if(beList->getCDR()->isNull())
                {
                    *status = false;
                    std::cout << "[ERROR] COND Statement must have atleast one clause which evaluates to T.\n";
                    return NULL;
                }
                return evconFunc(status,beList->getCDR(), aList, dList,aMap);
            }
        }
        else
        {
            *status = false;
            return NULL;
        }
    }
}

SExp* evlistFunc(bool* status,SExp* list,SExp* aList,SExp* dList,const SExpMap &aMap)
{
    if(list->isNull())
    {
        *status = true;
        return list;
    }
    else if(list->isAtom())
    {
        *status = false;
        return NULL;
    }
    else
    {
        bool success1,success2;
        SExp* res1 = evalFunc(&success1,list->getCAR() , aList, &dList,aMap);
        SExp* res2 = evlistFunc(&success2, list->getCDR(), aList, dList,aMap);
        bool final = success1 & success2;
        if(!success1)
        {
            std::cout << "[ERROR] Error evaluating ";
            list->getCAR()->toString();
            std::cout << " expression.\n";
        }
        *status = final;
        if(final)
        {
            SExp* result = new SExp(res1,res2);
            return result;
        }
        else
            return NULL;
    }
}

SExp* applyFunc(bool* status,SExp* funcName,SExp* body,SExp* aList,SExp* dList,const SExpMap &aMap)
{
    if(funcName->isAtom())
    {
        if(funcName->eqByName("CAR"))
        {
            *status = true;
            return body->getCAR()->getCAR();
        }
        else if (funcName->eqByName("CDR"))
        {
            *status = true;
            return body->getCAR()->getCDR();
        }
        else if(funcName->eqByName("CONS"))
        {
            *status = true;
            SExp* newSExp = new SExp(body->getCAR(),body->getCDR()->getCAR());
            return newSExp;
        }
        else if(funcName->eqByName("ATOM"))
        {
            *status = true;
            bool res = body->getCAR()->isAtom();
            if(res)
            {
                return aMap.find("T")->second;
            }
            else
            {
                return aMap.find("NIL")->second;
            }
        }
        else if(funcName->eqByName("EQ"))
        {
            *status = true;
            SExp* e1 = body->getCAR();
            SExp* e2 = body->getCDR()->getCAR();
            bool res = e1->getPureEquality(e2);
            if(res)
            {
                return aMap.find("T")->second;
            }
            else
            {
                return aMap.find("NIL")->second;
            }
        }
        else if(funcName->eqByName("NULL"))
        {
            *status = true;
            bool res = body->getCAR()->isNull();
            if(res)
            {
                return aMap.find("T")->second;
            }
            else
            {
                return aMap.find("NIL")->second;
            }
        }
        else if(funcName->eqByName("INT"))
        {
            *status = true;
            bool res=false;
            if(body->getCAR()->isAtom())
            {
                if(body->getCAR()->isString())
                    res = false;
                else
                    res = true;
            }
            if(res)
            {
                return aMap.find("T")->second;
            }
            else
            {
                return aMap.find("NIL")->second;
            }
        }
        else if (funcName->eqByName("PLUS"))
        {
            
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    int result = op1->getIntegerID() + op2->getIntegerID();
                    SExp* newsexp = new SExp(result);
                    return newsexp;
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for PLUS primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for PLUS primitive function.\n";
                return NULL;
            }
        }
        else if (funcName->eqByName("MINUS"))
        {
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    int result = op1->getIntegerID() - op2->getIntegerID();
                    SExp* newsexp = new SExp(result);
                    return newsexp;
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for MINUS primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for MINUS primitive function.\n";
                return NULL;
            }
        }
        else if (funcName->eqByName("TIMES"))
        {
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    int result = op1->getIntegerID() * op2->getIntegerID();
                    SExp* newsexp = new SExp(result);
                    return newsexp;
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for TIMES primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for TIMES primitive function.\n";
                return NULL;
            }
        }
        else if (funcName->eqByName("QUOTIENT"))
        {
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    int result = op1->getIntegerID() / op2->getIntegerID();
                    SExp* newsexp = new SExp(result);
                    return newsexp;
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for QUOTIENT primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for QUOTIENT primitive function.\n";
                return NULL;
            }
        }
        else if (funcName->eqByName("REMAINDER"))
        {
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    int result = op1->getIntegerID() % op2->getIntegerID();
                    SExp* newsexp = new SExp(result);
                    return newsexp;
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for REMAINDER primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for REMAINDER primitive function.\n";
                return NULL;
            }
        }
        else if (funcName->eqByName("LESS"))
        {
            
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    bool result = op1->getIntegerID() < op2->getIntegerID();
                    if(result)
                    {
                        return aMap.find("T")->second;
                    }
                    else
                    {
                        return aMap.find("NIL")->second;
                    }
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for LESS primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for LESS primitive function.\n";
                return NULL;
            }
        }
        else if (funcName->eqByName("GREATER"))
        {
            SExp* op1 = body->getCAR();
            SExp* op2 = body->getCDR()->getCAR();
            if(op1->isAtom() && op2->isAtom())
            {
                if(!(op1->isString() || op2->isString()))
                {
                    *status = true;
                    bool result = op1->getIntegerID() > op2->getIntegerID();
                    if(result)
                    {
                        return aMap.find("T")->second;
                    }
                    else
                    {
                        return aMap.find("NIL")->second;
                    }
                }
                else
                {
                    *status = false;
                    std::cout << "[ERROR] Integer operands expected for GREATER primitive function.\n";
                    return NULL;
                }
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Integer operands expected for GREATER primitive function.\n";
                return NULL;
            }
        }
        else
        {
            bool sanityCheck = searchInList(funcName, dList);
            if(sanityCheck)
            {
                SExp* dataFromDList = getValFromList(funcName, dList);
                //Sanity check here for number of parameters matching
                int nosFParams = numberOfElems(dataFromDList->getCAR());
                int nosAParams = numberOfElems(body);
                if(nosAParams==-1)
                {
                    *status = false;
                    std::cout << "[ERROR] Arguments to Function ";
                    funcName->toString();
                    std::cout << "is not in the form of a proper list.\n";
                    return NULL;
                }
                if(nosFParams!=nosAParams)
                {
                    *status = false;
                    if(nosAParams<nosFParams)
                    {
                        std::cout << "[ERROR] Too few arguments to function ";
                        funcName->toString();
                        std::cout << ".\n";
                    }
                    else
                    {
                        std::cout << "[ERROR] Too many arguments to function ";
                        funcName->toString();
                        std::cout << ".\n";
                    }
                    return NULL;
                }
                SExp* newAList = addPairs(dataFromDList->getCAR(),body,aList);
                return evalFunc(status, dataFromDList->getCDR(),newAList, &dList,aMap);
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Function ";
                funcName->toString();
                std::cout << "that you are trying to apply is not defined yet.\n";
                return NULL;
            }
        }
    }
    else
    {
        *status = false;
        std::cout << "[ERROR] A non atomic S Expression has been passed as a function name.\n";
        return NULL;
    }
}

SExp* addPairs(SExp* pList, SExp* argVals, SExp* aList)
{
    if(pList->isNull())
        return aList;
    SExp* newExp = new SExp(pList->getCAR(),argVals->getCAR());
    SExp* newTail = addPairs(pList->getCDR(),argVals->getCDR(),aList);
    SExp* newRes = new SExp(newExp,newTail);
    return newRes;
}


#endif
