//
//  main.cpp
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 2/22/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//
#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <sstream>
#include <stack>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "sexpression.h"

bool isNumber(const std::string& s);
int convertToNumber(const std::string& s);
bool getSExpressionTree(std::string text,SExp** S,bool isVerbose);


enum tokenType {TR_LP,TR_RP,TR_DOT,TR_ATOM,TERM_SYM};

typedef struct {
    std::string lexval;
    tokenType type;
}tokenEntry;

int main(int argc, const char* argv[])
{
    std::cout << "LISP Interpretor Project 6341 \n(C) Rajaditya Mukherjee \nPlease type in the S-Expression and press <enter>\n";
    std::cout << "Enter 'quit' to Quit.\n";
    std::cout << "Enter 'verbon'('verboff') to turn ON(OFF) Verbose Mode";
    //std::cout << ">";
    std::string text;
    bool terminate = false;
    bool isVerbose = false;
    do {
        std::cout << "\n$";
        std::getline(std::cin,text);
        if(!(std::strcmp(text.c_str(),"quit")))
           terminate = true;
        else if (!(std::strcmp(text.c_str(),"verboff")))
        {
            std::cout << "[INFO] Verbose mode is OFF.";
            isVerbose = false;
        }
        else if (!(std::strcmp(text.c_str(),"verbon")))
        {
            std::cout << "[INFO] Verbose mode is ON. Expect lots of information about parsing.";
            isVerbose = true;
        }
        else
        {
            SExp* finalSExp = new SExp();
            bool parsingSuccess = getSExpressionTree(text,&finalSExp,isVerbose);
            std::cout << ">";
            if(parsingSuccess)
                finalSExp->toString();
            else
                std::cout << "[ERROR] Parsing Failed.";
        }
    } while (!terminate);
    //std::string text = "(DEFUN NOTSOSILLY (A B) (COND ((EQ A 0) (PLUS B 1)) ((EQ B 0) (NOTSOSILLY (MINUS2 A 1) 1)) (T (NOTSOSILLY (MINUS2 A 1) (NOTSOSILLY A (MINUS2 B 1)))) ))";
    //std::string text = " (DEFUN MINUS2 (A B) (MINUS A B))";
    //std::string text = "(DEFUN SILLY (A B) (PLUS A B))";
    //std::string text = "(SILLY 5 6)";
    //std::string text = "(PLUS 5 (3))";
    //std::string text = "(CAR (QUOTE (A . B)))";
    //std::string text = "(CONS 4 (QUOTE (A . B)))";
    //std::string text = " (CONS 4 (A . B))";
    //std::string text = " (SILLY (CAR (QUOTE (5 . 6))) (CDR (QUOTE (5 . 6))) )";
    //std::string text = " (defun match (p s)(cond((handle-both-null p s))((handle-normal-recursion p s))((atom (first p)) nil)((handle-? p s))((handle-* p s))((handle-restrict-pred p s))(t nil) ) )"
    std::cout << "[INFO]LISP Interpretor exited successfully.";
    return 0;
    
}

bool getSExpressionTree(std::string text,SExp** S,bool isVerbose)
{
    
    //Tokenize the strings
    boost::char_separator<char> sep(" ","().",boost::drop_empty_tokens);
    boost::tokenizer<boost::char_separator<char>> tokens(text, sep);
    
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
    
    //Map for atomic expressions
    std::map<std::string,SExp*> atomicExpMap;
    
    //Lets add the basic maps for CONS, CAR, CDR....
    //Map for NIL
    SExp* _nil = new SExp("NIL");
    atomicExpMap.insert(std::make_pair("NIL", _nil));
    //Map for CAR
    SExp* _car = new SExp("CAR");
    atomicExpMap.insert(std::make_pair("CAR", _car));
    //Map for CDR
    SExp* _cdr = new SExp("CDR");
    atomicExpMap.insert(std::make_pair("CDR", _cdr));
    //@TODO : Insert more basic maps (if)
    //@TODO : Possibly shift them to some function
    
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
                            E->setIntegerID(convertToNumber(curTok.lexval));
                            E->setIsAtom(true);
                            E->setIsString(false);
                        }
                        else
                        {
                            if(!(std::strcmp("NIL",curTok.lexval.c_str())))
                            {
                                SExp* parent = E->getParent();
                                if(E->isCAR())
                                    parent->setCAR((atomicExpMap.find("NIL")->second));
                                else
                                    parent->setCDR((atomicExpMap.find("NIL")->second));
                            }
                            else if (!(std::strcmp("CAR",curTok.lexval.c_str())))
                            {
                                SExp* parent = E->getParent();
                                if(E->isCAR())
                                    parent->setCAR((atomicExpMap.find("CAR")->second));
                                else
                                    parent->setCDR((atomicExpMap.find("CAR")->second));
                            }
                            else if (!(std::strcmp("CDR",curTok.lexval.c_str())))
                            {
                                SExp* parent = E->getParent();
                                if(E->isCAR())
                                    parent->setCAR((atomicExpMap.find("CDR")->second));
                                else
                                    parent->setCDR((atomicExpMap.find("CDR")->second));
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
                                    if(E->isCAR())
                                        parent->setCAR((atomicExpMap.find(curTok.lexval)->second));
                                    else
                                        parent->setCDR((atomicExpMap.find(curTok.lexval)->second));
                                }
                                
                            }
                                
                        }
                        if(isVerbose)
                            std::cout << "[INFO] Atom processed is " << curTok.lexval << "\n";
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
                            if(X->isCAR())
                                parent->setCAR((atomicExpMap.find("NIL")->second));
                            else
                                parent->setCDR((atomicExpMap.find("NIL")->second));
                        }
                        break;
                    case TR_ATOM :
                        if(isVerbose)
                            std::cout << "[INFO]Apply rule X-> E Y  \n";
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
                            std::cout << "[INFO]Apply rule Y-> . E ) \n";
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
                              std::cout << "[INFO]Apply rule R -> E R   \n";
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


bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int convertToNumber(const std::string& s)
{
    std::istringstream ss(s);
    int result;
    ss >> result ;
    return result;
}
