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

typedef struct {
    std::string lexval;
    tokenType type;
}tokenEntry;

typedef std::map<std::string,SExp*> SExpMap;


#endif
