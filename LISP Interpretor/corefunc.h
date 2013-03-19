//
//  corefunc.h
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 3/12/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#ifndef LISP_Interpretor_corefunc_h
#define LISP_Interpretor_corefunc_h

#include <vector>
#include <stack>
#include <map>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "enumstructdecs.h"
#include "stringfunc.h"

void populateTableWithPrimitive(SExpMap & aMap)
{
	//Map for T
    SExp* _t = new SExp("T");
    aMap.insert(std::make_pair("T", _t));
    //Map for NIL
    SExp* _nil = new SExp("NIL");
    aMap.insert(std::make_pair("NIL", _nil));
    //Map for CAR
    SExp* _car = new SExp("CAR");
    aMap.insert(std::make_pair("CAR", _car));
    //Map for CDR
    SExp* _cdr = new SExp("CDR");
    aMap.insert(std::make_pair("CDR", _cdr));
	//Map for CONS
    SExp* _cons = new SExp("CONS");
    aMap.insert(std::make_pair("CONS", _cons));
	//Map for ATOM
    SExp* _atom = new SExp("ATOM");
    aMap.insert(std::make_pair("ATOM", _atom));
	//Map for EQ
    SExp* _eq = new SExp("EQ");
    aMap.insert(std::make_pair("EQ", _eq));
	//Map for NULL
    SExp* _null = new SExp("NULL");
    aMap.insert(std::make_pair("NULL", _null));
	//Map for INT
    SExp* _int = new SExp("INT");
    aMap.insert(std::make_pair("INT", _int));
	//Map for PLUS
    SExp* _plus = new SExp("PLUS");
    aMap.insert(std::make_pair("PLUS", _plus));
	//Map for MINUS
    SExp* _minus = new SExp("MINUS");
    aMap.insert(std::make_pair("MINUS", _minus));
	//Map for TIMES
    SExp* _times = new SExp("TIMES");
    aMap.insert(std::make_pair("TIMES", _times));
	//Map for QUOTIENT
    SExp* _quo = new SExp("QUOTIENT");
    aMap.insert(std::make_pair("QUOTIENT", _quo));
	//Map for REMAINDER
    SExp* _rem = new SExp("REMAINDER");
    aMap.insert(std::make_pair("REMAINDER", _rem));
	//Map for LESS
    SExp* _less = new SExp("LESS");
    aMap.insert(std::make_pair("LESS", _less));
	//Map for GREATER
    SExp* _grtr = new SExp("GREATER");
    aMap.insert(std::make_pair("GREATER", _grtr));
	//Map for COND
    SExp* _cond = new SExp("COND");
    aMap.insert(std::make_pair("COND", _cond));
	//Map for QUOTE
    SExp* _quote = new SExp("QUOTE");
    aMap.insert(std::make_pair("QUOTE", _quote));
	//Map for DEFUN
    SExp* _defun = new SExp("DEFUN");
    aMap.insert(std::make_pair("DEFUN", _defun));
}

bool getSExpressionTree(std::string text,SExp** S,bool isVerbose,SExpMap & atomicExpMap)
{
    
    //Tokenize the strings
    boost::char_separator<char> sep(" \t","().",boost::drop_empty_tokens);
    boost::tokenizer<boost::char_separator<char> > tokens(text, sep);
    
    //Classify into proper types
    std::vector<tokenEntry> tokenList;
    BOOST_FOREACH(std::string t, tokens)
    {
        tokenEntry* tk = new tokenEntry();
        tk->lexval = t;
        if (std::strcmp(t.c_str(), "(") == 0)
        {
            tk->type = TR_LP;
        }
        else if (std::strcmp(t.c_str(), ")") == 0)
        {
            tk->type = TR_RP;
        }
        else if (std::strcmp(t.c_str(), ".") == 0)
        {
            tk->type = TR_DOT;
        }
        else
        {
            tk->type = TR_ATOM;
        }
        tokenList.push_back(*tk);
    }
    
    //Dont forget the terminator symbol $
    tokenEntry *terminator = new tokenEntry();
    terminator->lexval = "$";
    terminator->type = TERM_SYM;
    tokenList.push_back(*terminator);
    
    if(isVerbose)
        std::cout << "[INFO] Tokenization sucessfully completed\n";
    /*
    //Map for atomic expressions
    SExpMap atomicExpMap;
    
	//Populate with primitives
	populateTableWithPrimitive(atomicExpMap);
    */
    
    //Parser for S Expression
    std::stack<char> parsingStack;
    std::stack<SExp*> addRStack;
    std::stack<SExp*> addYStack;
    std::stack<SExp*> addXStack;
    std::stack<SExp*> addEStack ;
    parsingStack.push('$');
    parsingStack.push('S');
    SExp *E;
    SExp *X;
    SExp *Y;
    SExp *R;
    SExp* E1;
    SExp* Y1;
    SExp* E2;
    SExp* R1;
    bool errorF = false;
    bool finishedP = false;
    BOOST_FOREACH(tokenEntry curTok, tokenList)
    {
        if(errorF) break;
        char symStackTop;
        //std::cout << symStackTop << "\n";
        bool readyForNxtToken = false;
        while (!(readyForNxtToken || errorF || finishedP))
        {
            symStackTop = parsingStack.top();
            switch (symStackTop)
            {
                    
                case 'S':
                    switch (curTok.type)
                {
                    case TR_LP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule S->E \n";
                        //Parsing Stuff
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('E');
                        //Pointer Manupulation for tree
                        addEStack.push(*S);
                        //E = S;
                        break;
                    case TR_ATOM :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule S->E \n";
                        //Parsing Stuff
                        char t2;
                        t2 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('E');
                        //Pointer Manupulation for tree
                        addEStack.push(*S);
                        //E = S;
                        break;
                    default :
                        if(isVerbose)
                            std::cout << "[ERROR] Error in Parser found. Error Parsing Token - " << curTok.lexval;
                        errorF = true;
                        break;
                }
                    break;
                case 'E':
                    switch (curTok.type)
                {
                    case TR_LP :
                        //Parsing Stuff
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule E->( X \n";
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('X');
                        parsingStack.push('(');
                        //Pointer Manupulation for Tree
                        E = addEStack.top();
                        addEStack.pop();
                        addXStack.push(E);
                        //X = E;
                        break;
                    case TR_ATOM :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule E->atom \n";
                        //Parsing Stuff
                        char t2;
                        t2 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('a');
                        //Pointer Manupulation for tree
                        E = addEStack.top();
                        addEStack.pop();
                        if(isNumber(curTok.lexval))
                        {
							int numVal = convertToNumber(curTok.lexval);
                            E->setIntegerID(numVal);
                            E->setIsAtom(true);
                            E->setIsString(false);
							if(isVerbose)
								std::cout << "[INFO] Numerical Atom processed is " << numVal << "\n";
                        }
                        else
                        {
                            
							if(atomicExpMap.find(curTok.lexval)==atomicExpMap.end())
							{
								E->setStringID(curTok.lexval);
								E->setIsAtom(true);
								E->setIsString(true);
								if(isVerbose)
									std::cout << "[INFO] Inserted new string pair '"<< curTok.lexval <<"' in table. \n";
								atomicExpMap.insert(std::make_pair(curTok.lexval,E));
							}
							else
							{
								SExp* parent = E->getParent();
                                if(!parent)
                                {
                                    *S = atomicExpMap.find(curTok.lexval)->second;
                                    return true;
                                }
                                else
                                {
								if(E->isCAR())
									parent->setCAR((atomicExpMap.find(curTok.lexval)->second));
								else
									parent->setCDR((atomicExpMap.find(curTok.lexval)->second));
                                }
							}
							if(isVerbose)
								std::cout << "[INFO] Atom processed is " << curTok.lexval << "\n";
                        }
                        break;
                    default :
                        if(isVerbose)
                            std::cout << "[ERROR] Error in Parser found. Error Parsing Token - " << curTok.lexval;
                        errorF = true;
                        break;
                }
                    break;
                case 'X':
                    switch (curTok.type)
                {
                    case TR_LP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule X->E Y  \n";
                        //Parsing Stuff
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('Y');
                        parsingStack.push('E');
                        //Pointer Manupulation for Tree
                        X=addXStack.top();
                        addXStack.pop();
                        E1 = new SExp();
                        Y1 = new SExp();
                        X->setCAR(E1);
                        X->setCDR(Y1);
                        E1->setParent(X);
                        E1->setIsCAR(true);
                        Y1->setParent(X);
                        Y1->setIsCAR(false);
                        //E = E1;
                        //Y = Y1;
                        addEStack.push(E1);
                        addYStack.push(Y1);
                        break;
                    case TR_RP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule X-> )  \n";
                        //Parsing Stuff
                        char t2;
                        t2 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push(')');
                        //Pointer Manupulation for tree
                        X=addXStack.top();
                        addXStack.pop();
                    {
                        SExp* parent = X->getParent();
                        if(!parent)
                        {
                            *S = atomicExpMap.find("NIL")->second;
                            return true;
                        }
                        else
                        {
                            if(X->isCAR())
                                parent->setCAR((atomicExpMap.find("NIL")->second));
                            else
                                parent->setCDR((atomicExpMap.find("NIL")->second));
                        }
                    }
                        break;
                    case TR_ATOM :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule X-> E Y  \n";
                        //Parsing Stuff
                        char t3;
                        t3 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('Y');
                        parsingStack.push('E');
                        //Pointer Manupulation for Tree
                        X=addXStack.top();
                        addXStack.pop();
                        E1 = new SExp();
                        Y1 = new SExp();
                        X->setCAR(E1);
                        X->setCDR(Y1);
                        E1->setParent(X);
                        E1->setIsCAR(true);
                        Y1->setParent(X);
                        Y1->setIsCAR(false);
                        addEStack.push(E1);
                        addYStack.push(Y1);
                        break;
                    default :
                        if(isVerbose)
                            std::cout << "[ERROR] Error in Parser found. Error Parsing Token - " << curTok.lexval;
                        errorF = true;
                        break;
                }
                    break;
                case 'Y':
                    switch (curTok.type)
                {
                    case TR_LP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule Y ->R )  \n";
                        //Parsing Stuff
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push(')');
                        parsingStack.push('R');
                        //Pointer Manupulation for Tree
                        Y = addYStack.top();
                        addYStack.pop();
                        addRStack.push(Y);
                        break;
                    case TR_RP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule Y-> R )  \n";
                        //Parsing Stuff
                        char t2;
                        t2 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push(')');
                        parsingStack.push('R');
                        Y = addYStack.top();
                        addYStack.pop();
                        addRStack.push(Y);
                        break;
                    case TR_DOT :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule Y-> . E ) \n";
                        //Parsing Stuff
                        char t3;
                        t3 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push(')');
                        parsingStack.push('E');
                        parsingStack.push('.');
                        //Pointer Manupulations for Tree
                        Y = addYStack.top();
                        addYStack.pop();
                        addEStack.push(Y->getParent()->getCDR());
                        break;
                    case TR_ATOM :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule Y -> R )  \n";
                        //Parsing Stuff
                        char t4;
                        t4 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push(')');
                        parsingStack.push('R');
                        //Pointer Manupulation for Tree
                        Y = addYStack.top();
                        addYStack.pop();
                        addRStack.push(Y);
                        break;
                    default :
                        if(isVerbose)
                            std::cout << "[ERROR] Error in Parser found. Error Parsing Token - " << curTok.lexval;
                        errorF = true;
                        break;
                }
                    break;
                case 'R':
                    switch (curTok.type)
                {
                    case TR_LP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule R -> E R  \n";
                        //Parsing Stuff
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('R');
                        parsingStack.push('E');
                        //Pointer Manupulation for Tree
                        E2 = new SExp();
                        R1 = new SExp();
                        R = addRStack.top();
                        addRStack.pop();
                        //}
                        R->setCAR(E2);
                        R->setCDR(R1);
                        R1->setParent(R);
                        R1->setIsCAR(false);
                        E2->setParent(R);
                        E2->setIsCAR(true);
                        //E = E2;
                        //R = R1;
                        addEStack.push(E2);
                        addRStack.push(R1);
                        break;
                    case TR_RP :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule R -> epsa   \n";
                        //Parsing Stuff
                        char t2;
                        t2 = parsingStack.top();
                        parsingStack.pop();
                        //Pointer Manupulation for Tree
                        R = addRStack.top();
                        addRStack.pop();
                    {
                        SExp* parent = R->getParent();
                        if(R->isCAR())
                            parent->setCAR((atomicExpMap.find("NIL")->second));
                        else
                            parent->setCDR((atomicExpMap.find("NIL")->second));
                    }
                        break;
                    case TR_ATOM :
                        if(isVerbose)
                            std::cout << "[INFO] Apply rule R -> E R   \n";
                        //Parsing Stuff
                        char t3;
                        t3 = parsingStack.top();
                        parsingStack.pop();
                        parsingStack.push('R');
                        parsingStack.push('E');
                        //Pointer Manupulation for Tree
                        E2 = new SExp();
                        R1 = new SExp();
                        R = addRStack.top();
                        addRStack.pop();
                        R->setCAR(E2);
                        R->setCDR(R1);
                        R1->setParent(R);
                        R1->setIsCAR(false);
                        E2->setParent(R);
                        E2->setIsCAR(true);
                        addEStack.push(E2);
                        addRStack.push(R1);
                        break;
                    default :
                        if(isVerbose)
                            std::cout << "[ERROR] Error in Parser found. Error Parsing Token - " << curTok.lexval;
                        errorF = true;
                        break;
                }
                    break;
                case ')' :
                    if (curTok.type != TR_RP)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                    }
                    break;
                case '(' :
                    if (curTok.type != TR_LP)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                    }
                    break;
                case '.' :
                    if (curTok.type != TR_DOT)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                    }
                    break;
                case 'a' :
                    if (curTok.type != TR_ATOM)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        t1 = parsingStack.top();
                        parsingStack.pop();
                    }
                    break;
                case '$' :
                    if(curTok.type != TERM_SYM)
                        errorF = true;
                    else
                        finishedP =true;
                    break;
                    
                default:
                    if(isVerbose)
                        std::cout << "[ERROR] Error in Parser found. Error Parsing Token - " << curTok.lexval;
                    errorF = true;
                    break;
            }
        }
    }
    if(finishedP)
    {
        if(isVerbose)
            std::cout << "[INFO] Parsing sucessfully completed\n";
    }
    return finishedP;
    
}




#endif
