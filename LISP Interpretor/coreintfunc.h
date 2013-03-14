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
            SExp* fName = exp->getCDR()->getCAR();
            SExp* pList = exp->getCDR()->getCDR()->getCAR();
            SExp* fBody = exp->getCDR()->getCDR()->getCDR()->getCAR();
            
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

bool searchInList(SExp* exp,SExp* aList)
{
    if(exp->isNull() || aList->isNull())
    {
        return false;
    }
    else if(aList->getCAR()->getCAR()->eqByName(exp->getStringID().c_str()))
    {
        return true;
    }
    else return searchInList(exp, aList->getCDR());
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
        return NULL;
    }
    else
    {
        bool successOfEval;
        SExp* res1 = evalFunc(&successOfEval, beList->getCAR()->getCAR(), aList, &dList,aMap);
        if(successOfEval)
        {
            if(!(res1->eqByName("NIL")))
            {
                bool successOf2ndEval;
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
    else
    {
        bool success1,success2;
        SExp* res1 = evalFunc(&success1,list->getCAR() , aList, &dList,aMap);
        SExp* res2 = evlistFunc(&success2, list->getCDR(), aList, dList,aMap);
        bool final = success1 & success2;
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
        else
        {
            bool sanityCheck = searchInList(funcName, dList);
            if(sanityCheck)
            {
                SExp* dataFromDList = getValFromList(funcName, dList);
                //@TODO:Put some sanity check here for number of parameters matching
                SExp* newAList = addPairs(dataFromDList->getCAR(),body,aList);
                return evalFunc(status, dataFromDList->getCDR(),newAList, &dList,aMap);
            }
            else
            {
                *status = false;
                std::cout << "[ERROR] Function that you are trying to apply is not defined yet.\n";
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
