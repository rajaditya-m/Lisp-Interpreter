//
//  enumdecs.h
//  LISP Interpretor
//
//  Created by Rajaditya Mukherjee on 3/12/13.
//  Copyright (c) 2013 Rajaditya Mukherjee. All rights reserved.
//

#ifndef LISP_Interpretor_enumstructdecs_h
#define LISP_Interpretor_enumstructdecs_h


enum tokenType {TR_LP,TR_RP,TR_DOT,TR_ATOM,TERM_SYM};

enum errCode {T_LIST,T_ATOM,T_NUMLIST,T_NUM,T_RESERVED,T_OK};

enum strErrCode {S_OK,S_LC,S_SPC};

typedef struct {
    std::string lexval;
    tokenType type;
}tokenEntry;

typedef std::map<std::string,SExp*> SExpMap;

const char *reservedWords[] = {"T","NIL","CAR","CDR","CONS","ATOM","EQ","NULL","INT","PLUS","MINUS","TIMES","QUOTIENT","REMAINDER","LESS","GREATER","COND","QUOTE","DEFUN"};


#endif
