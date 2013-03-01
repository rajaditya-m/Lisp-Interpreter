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
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lockfree/stack.hpp>
#include "sexpression.h"

bool isNumber(const std::string& s);
int convertToNumber(const std::string& s);

enum tokenType {TR_LP,TR_RP,TR_DOT,TR_ATOM,TERM_SYM};

typedef struct {
    std::string lexval;
    tokenType type;
}tokenEntry;

int main(int argc, const char* argv[])
{
    //@TODO::Get input from the console
    std::string text = "(DEFUN NOTSOSILLY (A B) (COND ((EQ A 0) (PLUS B 1)) ((EQ B 0) (NOTSOSILLY (MINUS2 A 1) 1)) (T (NOTSOSILLY (MINUS2 A 1) (NOTSOSILLY A (MINUS2 B 1)))) ))";
    //std::string text = " (DEFUN MINUS2 (A B) (MINUS A B))";
    //std::string text = "(DEFUN SILLY (A B) (PLUS A B))";
    //std::string text = "(SILLY 5 6)";
    //std::string text = "(PLUS 5 (3))";
    
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
    
    std::cout << "[INFO] Tokenization sucessfully completed\n";

    //Debug routine for Tokenizer
    //@TODO : Remove later
    /*BOOST_FOREACH(tokenEntry it, tokenList)
    {
        std::cout << it.lexval << " Type :" << it.type << "\n";
    }*/
    
    //Map for atomic expressions
    std::map<std::string,SExp*> atomicExpMap;
    
    //Lets add the basic maps for CONS, CAR, CDR....
    //Map for NIL
    SExp* _nil = new SExp("NIL");
    atomicExpMap.insert(std::make_pair("NIL", _nil));
    //Map for CAR
    SExp* _car = new SExp("CAR");
    atomicExpMap.insert(std::make_pair("CAR", _car));
    //@TODO : Insert more basic maps required
    //@TODO : Possibly shift them to some function
    
    //Parser for S Expression
    boost::lockfree::stack<char> *parsingStack = new boost::lockfree::stack<char>(100);
    boost::lockfree::stack<SExp*> *addRStack = new boost::lockfree::stack<SExp*>(100);
    boost::lockfree::stack<SExp*> *addYStack = new boost::lockfree::stack<SExp*>(100);
    boost::lockfree::stack<SExp*> *addXStack = new boost::lockfree::stack<SExp*>(100);
    boost::lockfree::stack<SExp*> *addEStack = new boost::lockfree::stack<SExp*>(100);
    parsingStack->push('$');
    parsingStack->push('S');
    SExp *S = new SExp();
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
            parsingStack->pop(symStackTop);
            parsingStack->push(symStackTop);
            // std::cout << "Current stack top : " << symStackTop << "\n";
            // std::cout << "Current symbol being processed : " << curTok.lexval << "\n";
            switch (symStackTop)
            {
                    
                case 'S':
                    switch (curTok.type)
                    {
                        case TR_LP :
                            std::cout << "Apply rule S->E \n";
                            //Parsing Stuff
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('E');
                            //Pointer Manupulation for tree
                            addEStack->push(S);
                            //E = S;
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule S->E \n";
                            //Parsing Stuff
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push('E');
                            //Pointer Manupulation for tree
                            addEStack->push(S);
                            //E = S;
                            break;
                        default :
                            std::cout << "Error in Parser found";
                            errorF = true;
                            break;
                    }
                    break;
                case 'E':
                    switch (curTok.type)
                    {
                        case TR_LP :
                            //Parsing Stuff
                            std::cout << "Apply rule E->( X \n";
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('X');
                            parsingStack->push('(');
                            //Pointer Manupulation for Tree
                            addEStack->pop(E);
                            addXStack->push(E);
                            //X = E;
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule E->atom \n";
                            //Parsing Stuff
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push('a');
                            //Pointer Manupulation for tree
                            addEStack->pop(E);
                            if(isNumber(curTok.lexval))
                            {
                                E->setIntegerID(convertToNumber(curTok.lexval));
                                E->setIsAtom(true);
                                E->setIsString(false);
                            }
                            else
                            {
                                //@TODO : Do parent manupulation for strings like NIL CAR CDR
                                E->setStringID(curTok.lexval);
                                E->setIsAtom(true);
                                E->setIsString(true);
                            }
                            //@TODO : Remove
                            std::cout << "Atom processed is" << curTok.lexval << "\n";
                            break;
                        default :
                            std::cout << "Error in Parser found";
                            errorF = true;
                            break;
                    }
                    break;
                case 'X':
                    switch (curTok.type)
                    {
                        case TR_LP :
                            std::cout << "Apply rule X->E Y  \n";
                            //Parsing Stuff
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('Y');
                            parsingStack->push('E');
                            //Pointer Manupulation for Tree
                            addXStack->pop(X);
                            E1 = new SExp();
                            Y1 = new SExp();
                            X->setCAR(E1);
                            X->setCDR(Y1);
                            E1->setParent(X);
                            Y1->setParent(X);
                            //E = E1;
                            //Y = Y1;
                            addEStack->push(E1);
                            addYStack->push(Y1);
                            break;
                        case TR_RP :
                            std::cout << "Apply rule X-> )  \n";
                            //Parsing Stuff
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push(')');
                            //Pointer Manupulation for tree
                            //@TODO Do repetation manupulation
                            addXStack->pop(X);
                            X->setStringID("NIL");
                            X->setIsAtom(true);
                            X->setIsString(true);
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule X-> E Y  \n";
                            //Parsing Stuff
                            char t3;
                            parsingStack->pop(t3);
                            parsingStack->push('Y');
                            parsingStack->push('E');
                            //Pointer Manupulation for Tree
                            addXStack->pop(X);
                            E1 = new SExp();
                            Y1 = new SExp();
                            X->setCAR(E1);
                            X->setCDR(Y1);
                            E1->setParent(X);
                            Y1->setParent(X);
                            //E = E1;
                            //Y = Y1;
                            addEStack->push(E1);
                            addYStack->push(Y1);
                            break;
                        default : std::cout << "Error in Parser found";
                            errorF = true;
                            break;
                    }
                    break;
                case 'Y':
                    switch (curTok.type)
                    {
                        case TR_LP :
                            std::cout << "Apply rule Y ->R )  \n";
                            //Parsing Stuff
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push(')');
                            parsingStack->push('R');
                            //Pointer Manupulation for Tree
                            //Check if stack is empty or not
                            //if(!(addYStack->empty()))
                            //{
                            addYStack->pop(Y);
                            //}
                            addRStack->push(Y);
                            //R = Y;
                            break;
                        case TR_RP :
                            std::cout << "Apply rule Y-> R )  \n";
                            //Parsing Stuff
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push(')');
                            parsingStack->push('R');
                            //Pointer Manupulation for Tree
                            //Check if stack is empty or not
                            //if(!(addYStack->empty()))
                            //{
                            addYStack->pop(Y);
                            //}
                            addRStack->push(Y);
                            //R = Y;
                            break;
                        case TR_DOT :
                            std::cout << "Apply rule Y-> . E ) \n";
                            //Parsing Stuff
                            char t3;
                            parsingStack->pop(t3);
                            parsingStack->push(')');
                            parsingStack->push('E');
                            parsingStack->push('.');
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule Y -> R )  \n";
                            //Parsing Stuff
                            char t4;
                            parsingStack->pop(t4);
                            parsingStack->push(')');
                            parsingStack->push('R');
                            //Pointer Manupulation for Tree
                            //if(!(addYStack->empty()))
                            //{
                            addYStack->pop(Y);
                            //}
                            addRStack->push(Y);
                            //R = Y;
                            break;
                        default :
                            std::cout << "Error in Parser found";
                            errorF = true;
                            break;
                    }
                    break;
                case 'R':
                    switch (curTok.type)
                    {
                        case TR_LP :
                            std::cout << "Apply rule R -> E R  \n";
                            //Parsing Stuff
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('R');
                            parsingStack->push('E');
                            //Pointer Manupulation for Tree
                            E2 = new SExp();
                            R1 = new SExp();
                            //Check if stack is empty or not
                            //if(!(addRStack->empty()))
                            //{
                            addRStack->pop(R);
                            //}
                            R->setCAR(E2);
                            R->setCDR(R1);
                            R1->setParent(R);
                            E2->setParent(R);
                            //E = E2;
                            //R = R1;
                            addEStack->push(E2);
                            addRStack->push(R1);
                            break;
                        case TR_RP :
                            std::cout << "Apply rule R -> epsa   \n";
                            //Parsing Stuff
                            char t2;
                            parsingStack->pop(t2);
                            //Pointer Manupulation for Tree
                            //@TODO : Repetation Checks for NIL CAR ETC
                            addRStack->pop(R);
                            R->setStringID("NIL");
                            R->setIsAtom(true);
                            R->setIsString(true);
                            std::cout << "Imp Debug :: " << S->getCAR()->getIntegerID() << "\n";
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule R -> E R   \n";
                            //Parsing Stuff
                            char t3;
                            parsingStack->pop(t3);
                            parsingStack->push('R');
                            parsingStack->push('E');
                            //Pointer Manupulation for Tree
                            E2 = new SExp();
                            R1 = new SExp();
                            //Check if stack is empty or not
                            //if(!(addRStack->empty()))
                            //{
                            addRStack->pop(R);
                            //}
                            R->setCAR(E2);
                            R->setCDR(R1);
                            R1->setParent(R);
                            E2->setParent(R);
                            //E = E2;
                            //R = R1;
                            addEStack->push(E2);
                            addRStack->push(R1);
                            break;
                        default :
                            std::cout << "Error in Parser found \n";
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
                        parsingStack->pop(t1);
                    }
                    break;
                case '(' :
                    if (curTok.type != TR_LP)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        parsingStack->pop(t1);
                    }
                    break;
                case '.' :
                    if (curTok.type != TR_DOT)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        parsingStack->pop(t1);
                    }
                    break;
                case 'a' :
                    if (curTok.type != TR_ATOM)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        parsingStack->pop(t1);
                    }
                    break;
                case '$' :
                    if(curTok.type != TERM_SYM)
                        errorF = true;
                    else
                        finishedP =true;
                    break;
                    
                default:
                    errorF = true;
                    break;
            }
        }
    }
    if(errorF)
        std::cout << "[ERROR] Parsing Failed.\n";
    else
    {
        std::cout << "[INFO] Parsing sucessfully completed\n";
        //std::cout << S->getCAR()->getIntegerID();
        S->toString();
    }
    //char test;
    //parsingStack->pop(test);
    //std::cout << test;
    return 0;
    
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
