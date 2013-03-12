//
//  main.cpp
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 2/22/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//
#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <iomanip>
#include <sstream>
#include <stack>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "sexpression.h"
#include "corefunc.h"

bool isNumber(const std::string& s);
int convertToNumber(const std::string& s);
bool getSExpressionTree(std::string text,SExp** S,bool isVerbose);
int matchingBrace(const std::string &s, int* startCount);
void populateTableWithPrimitive(std::map<std::string,SExp*> & aMap);


enum tokenType {TR_LP,TR_RP,TR_DOT,TR_ATOM,TERM_SYM};

typedef struct {
    std::string lexval;
    tokenType type;
}tokenEntry;

int main(int argc, const char* argv[])
{
	std::cout << "*******************************************************************************************\n";
    std::cout << "**                           LISP Interpreter Project 6341                               **\n";
	std::cout << "**                  (C) Rajaditya Mukherjee {mukherjee.62@osu.edu}                       **\n";
	std::cout << "** Please type in the S-Expression and press <enter>                                     **\n";
	std::cout << "** If expression is not complete, interpreter will wait for additional lines of input    **\n";
    std::cout << "** Enter 'quit' to Quit.                                                                 **\n";
    std::cout << "** Enter 'verbon'('verboff') to turn ON(OFF) Verbose Mode                                **\n";
	std::cout << "*******************************************************************************************\n";
    //std::cout << ">";
    std::string text,inputByLine;
    bool terminate = false;
    bool isVerbose = false;
    do {
        std::cout << "\n$";
        std::getline(std::cin,inputByLine);
        if(!(std::strcmp(inputByLine.c_str(),"quit")))
            terminate = true;
        else if (!(std::strcmp(inputByLine.c_str(),"verboff")))
        {
            std::cout << "[INFO] Verbose mode is OFF.";
            isVerbose = false;
        }
        else if (!(std::strcmp(inputByLine.c_str(),"verbon")))
        {
            std::cout << "[INFO] Verbose mode is ON. Expect lots of information about parsing.";
            isVerbose = true;
        }
		else if(inputByLine.empty())
		{
			std::cout <<"[INFO] No Input detected. Parser will not do anything.\n";
			continue;
		}
        else
        {
            // Get the first token :
            char buffer[20];
			int copied = sprintf(buffer,"%.19s",inputByLine.c_str());
            char* firstToken = strtok(buffer," \t");
			if (!firstToken)
			{
				std::cout <<"[INFO] No Input detected. Parser will not do anything.\n";
				continue;
			}
            // Check if first token is a left brace
            if(firstToken[0]!='(')
            {
                std::cout << "[INFO] Taking only first token as valid S-Expression.\n";
                std::cout << "[INFO] Input (if any) after " << firstToken << " will be ignored\n";
                text = firstToken;
            }
            else
            {
                int runningCount = 0;
                text = "";
                bool multilineIO = true;
                bool firstTime = true;
                std::string relevantPortion;
                std::string inputByLine2;
                while(multilineIO)
                {
                    int retValue;
                    if(!firstTime)
                        std::getline(std::cin,inputByLine2);
                    if(firstTime)
                        retValue = matchingBrace(inputByLine,&runningCount);
                    else
                        retValue = matchingBrace(inputByLine2,&runningCount );
                    if(retValue != -1)
                    {
                        multilineIO = false;
                        if(!firstTime)
                            relevantPortion = inputByLine2.substr(0,retValue+1);
                        else
                            relevantPortion = inputByLine.substr(0,retValue+1);
                        if(!firstTime)
                        {
                            std::cout << "[INFO] Found a completed Multiline S Expression.\n";
                            if(retValue != inputByLine2.length()-1)
                                std::cout << "[INFO] Additional portion " << inputByLine2.substr(retValue+1) << " will be ignored.\n";
                        }
						else
						{
							if(retValue != inputByLine.length()-1)
                                std::cout << "[INFO] Additional portion " << inputByLine.substr(retValue+1) << " will be ignored.\n";
						}
						text += " ";
                        text += relevantPortion;
                    }
                    else
                    {
						text += " ";
                        if(firstTime)
                            text += inputByLine;
                        else
                            text += inputByLine2;
                    }
                    if(firstTime)
                        firstTime = false;
                }
            }
            SExp* finalSExp = new SExp();
            bool parsingSuccess = getSExpressionTree(text,&finalSExp,isVerbose);
            std::cout << ">";
            if(parsingSuccess)
                finalSExp->toString();
            else
                std::cout << "[ERROR] Parsing Failed. More information can be found in Verbose Mode.";
        }
    } while (!terminate);
    std::cout << "[INFO] LISP Interpreter exited successfully.";
    return 0;
    
}

