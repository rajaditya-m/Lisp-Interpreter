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
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lockfree/stack.hpp>
#include "sexpression.h"

bool isNumber(const std::string& s);

enum tokenType {TR_LP,TR_RP,TR_DOT,TR_ATOM,TERM_SYM};

typedef struct {
    std::string lexval;
    tokenType type;
}tokenEntry;

int main(int argc, const char* argv[])
{
    //Get input from the console
    std::string text = "(DEFUN NOTSOSILLY (A B) (COND ((EQ A 0) (PLUS B 1)) ((EQ B 0) (NOTSOSILLY (MINUS2 A 1) 1)) (T (NOTSOSILLY (MINUS2 A 1) (NOTSOSILLY A (MINUS2 B 1)))) ))";
    
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
    atomicExpMap.insert(std::make_pair("NIL", _car));
    //@TODO : Insert more basic maps required
    //@TODO : Possibly shift them to some function
    
    //Parser for S Expression
    boost::lockfree::stack<char> *parsingStack = new boost::lockfree::stack<char>(100);
    parsingStack->push('$');
    parsingStack->push('S');
    SExp *S;
    SExp *E;
    SExp *X;
    bool errorF = false;
    BOOST_FOREACH(tokenEntry curTok, tokenList)
    {
        if(errorF) break;
        char symStackTop;
        parsingStack->pop(symStackTop);
        parsingStack->push(symStackTop);
        //std::cout << symStackTop << "\n";
        bool readyForNxtToken = false;
        while (!(readyForNxtToken || errorF))
        {
            switch (symStackTop)
            {
                    
                case 'S':
                    switch (curTok.type)
                    {
                        case TR_LP :
                            std::cout << "Apply rule S->E \n";
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('E');
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule S->E \n";
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push('E');
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
                            std::cout << "Apply rule E->( X \n";
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('X');
                            parsingStack->push('(');
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule E->atom \n"; 
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push('a');
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
                            char t1;
                            parsingStack->pop(t1);
                            parsingStack->push('Y');
                            parsingStack->push('E');
                            break;
                        case TR_RP :
                            std::cout << "Apply rule X-> )  \n";
                            char t2;
                            parsingStack->pop(t2);
                            parsingStack->push(')');
                            break;
                        case TR_ATOM :
                            std::cout << "Apply rule X-> E Y  \n";
                            char t3;
                            parsingStack->pop(t3);
                            parsingStack->push('Y');
                            parsingStack->push('E');
                            break;
                        default : std::cout << "Error in Parser found";
                            errorF = true;
                            break;
                    }
                    break;
                case 'Y':
                    switch (curTok.type)
                    {
                        case TR_LP : std::cout << "Apply rule Y ->R )  \n"; break;
                        case TR_RP : std::cout << "Apply rule Y-> R )  \n"; break;
                        case TR_DOT : std::cout << "Apply rule Y-> . E ) \n"; break;
                        case TR_ATOM : std::cout << "Apply rule Y -> R )  \n"; break;
                        default : std::cout << "Error in Parser found"; errorF = true; break;
                    }
                    break;
                case 'R':
                    switch (curTok.type)
                    {
                        case TR_LP : std::cout << "Apply rule R -> E R  \n"; break;
                        case TR_RP : std::cout << "Apply rule R -> epsa )  \n"; break;
                        case TR_ATOM : std::cout << "Apply rule R -> E R )  \n"; break;
                        default : std::cout << "Error in Parser found"; errorF = true; break;
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
                case '(' :
                    if (curTok.type != TR_LP)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        parsingStack->pop(t1);
                    }
                case '.' :
                    if (curTok.type != TR_DOT)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        parsingStack->pop(t1);
                    }
                case 'a' :
                    if (curTok.type != TR_ATOM)
                        errorF = true;
                    else
                    {
                        readyForNxtToken = true;
                        char t1;
                        parsingStack->pop(t1);
                    }
                case '$' :
                    if(curTok.type != TERM_SYM)
                        errorF = true;
                    break;
                    
                default:
                    break;
            }
        }
    }
    if(errorF)
        std::cout << "[ERROR] Parsing Failed.\n";
    else
        std::cout << "[INFO] Parsing sucessfully completed\n";
    
    char test;
    parsingStack->pop(test);
    std::cout << test;
    return 0;
    
}

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}



/*
#include <iostream>
#include <cstring>
#include <vector>
#include "sexpression.h"

SExp* createTreeFromMixed(std::string str);
void Tokenize(const std::string& str,vector<std::string>& tokens,const std::string& delimiters);
std::string stripOuterBrackets(std::string s);

int main(int argc, const char * argv[])
{

    //string str ="(PLUS (PLUS 2 3) (PLUS 4 5))";
    std::string str ="((QUOTE (2,3)) (PLUS 2 3) (PLUS 4 5))";
    std::cout << stripOuterBrackets(str);
    
    //createTreeFromMixed(str);
    //char * pch;
    //std::cout << "Splitting string" << str << " into tokens:\n";
    // Determine greedily whether its a list or dot notation
    /*pch = strtok (str.c_str(),".");
    while (pch != NULL)
    {
        std::cout << pch << "\n" ;
        pch = strtok (NULL, ".");
    }
    return 0;
}

SExp* createTreeFromMixed(string str)
{
    //Determine greedily whether its a list or dot notation
    unsigned long lenght = str.length();
    unsigned long brackStart = str.find('(',1);
    unsigned long dot = str.find('.',1);
    //If string is not in dot notation its all list notation so we will process accordingly
    if(dot == string::npos)
    {
        if(brackStart > 1)
        {
            cout << str.substr(1,brackStart-2) << "\n";
            cout << str.substr(brackStart,(lenght-brackStart-1)) << "\n";
        }
        else
        {
            unsigned long brackEnd = str.find(')',brackStart);
            cout << str.substr(1,lenght-brackEnd-1) << "\n";
            cout << str.substr(brackEnd+2,(lenght-brackEnd-1)) << "\n";
        }
    }
    else
    {
        
        cout << str.substr(1,dot-1) << "\n";
        cout << str.substr(dot+1,(lenght-dot-2)) << "\n";
    }
    //Otherwise its all bracket notation
    return NULL;
    
}

std::string stripOuterBrackets(std::string s)
{
    //Check if outer brackets exists
    std::string retval("");
    if(s.at(0)=='(')
    {
        if(s.back()!=')')
        {
            std::cout << "[ERROR] Malformed input is given. Please put () between expressions \n";
            return "";
        }
        retval = s.substr(1,(s.length()-2));
    }
    return retval;
}

void Tokenize(const string& str,vector<string>& tokens,const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}*/
