/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of Minimal (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    Minimal is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    Minimal is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Minimal; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minimal_internal.h"

Minimal_SyntaxTree Minimal_SyntaxTree_createBlank(char* id) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_BLANK;
    tree->type_id = NULL;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createNil(char* id) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_NIL;
    tree->type_id = NULL;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createID(char* id) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_IDENTIFIER;
    tree->type_id = NULL;
    tree->string = (char*)malloc(strlen(id)+1);
    strcpy(tree->string, id);
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createInteger(int i) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_INTEGER;
    tree->type_id = NULL;
    tree->integer = i;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createString(char* s) {
    printf("create string. %s\n", s);
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_STRING;
    tree->type_id = NULL;
    tree->string = malloc(strlen(s)+1);
    strcpy(tree->string, s);
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createOperator(Minimal_SyntaxTree op, Minimal_SyntaxTree expr1, Minimal_SyntaxTree expr2) {
    Minimal_SyntaxTree tree;
    if(op->type != ST_IDENTIFIER) {
        fprintf(stderr, "Invalid type of operator to Minimal_SyntaxTree_createOperator %i\n", op->type);
        return NULL;
    }
    tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_OPERATOR;
    tree->type_id = NULL;
    tree->string = (char*)malloc(strlen(op->string)+1);
    strcpy(tree->_operator, op->string);
    Minimal_delReference(op);
    tree->op1 = expr1;
    tree->op2 = expr2;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createBracket(Minimal_SyntaxTree b) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_BRACKET;
    tree->type_id = NULL;
    tree->type_def = b;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createIndex(Minimal_SyntaxTree expr, Minimal_SyntaxTree index) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_INDEX;
    tree->type_id = NULL;
    tree->expr = expr;
    tree->index = index;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createTuple(int size) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_TUPLE;
    tree->type_id = NULL;
    if(size == 0) {
        tree->tuple = NULL;
    } else {
        int i;
        tree->tuple = (Minimal_SyntaxTree*)malloc(sizeof(void*)*size);
        for(i=0; i<size; i++) {
            tree->tuple[i] = NULL;
        }
    }
    tree->size = size;
    return tree;
}

void Minimal_SyntaxTree_setTuple(Minimal_SyntaxTree tup, int i, Minimal_SyntaxTree v) {
    if(tup->tuple[i] != NULL) {
        Minimal_delReference(tup->tuple[i]);
    }
    tup->tuple[i] = v;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createIfExpr(Minimal_SyntaxTree test, Minimal_SyntaxTree expr1, Minimal_SyntaxTree expr2) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_IFEXPR;
    tree->type_id = NULL;
    tree->_if = test;
    tree->_then = expr1;
    tree->_else = expr2;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createSequence() {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_SEQENTIAL_DEFS;
    tree->type_id = NULL;
    tree->length = 0;
    tree->branches = NULL;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createSequence2(Minimal_SyntaxTree s1, Minimal_SyntaxTree s2) {
    if(s1->type == ST_SEQENTIAL_DEFS && s2->type == ST_SEQENTIAL_DEFS) {
        int i;
        for(i = 0; i < s2->length; i++) {
            Minimal_addReference(s2->branches[i]);
            Minimal_SyntaxTree_addToSequence(s1, s2->branches[i]);
        }
        return s1;
    } else if(s1->type == ST_SEQENTIAL_DEFS) {
        Minimal_SyntaxTree_addToSequence(s1, s2);
        return s1;
    } else if(s2->type == ST_SEQENTIAL_DEFS) {
        Minimal_SyntaxTree_prependToSequence(s2, s1);
        return s2;
    } else {
        Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
        tree->type = ST_SEQENTIAL_DEFS;
        tree->type_id = NULL;
        tree->length = 2;
        tree->branches = malloc(sizeof(void*)*2);
        tree->branches[0] = s1;
        tree->branches[1] = s2;
        return tree;
    }
}

Minimal_SyntaxTree Minimal_SyntaxTree_addToSequence(Minimal_SyntaxTree type, Minimal_SyntaxTree v) {
    Minimal_SyntaxTree* old = type->branches;
    if(old == NULL) {
        type->branches = malloc(sizeof(void*));
        type->branches[0] = v;
        type->length = 1;
    } else {
        type->branches = malloc(sizeof(void*)*(type->length+1));
        memcpy(type->branches, old, sizeof(void*)*type->length);
        type->branches[type->length] = v;
        type->length++;
    }
    return type;
}

Minimal_SyntaxTree Minimal_SyntaxTree_prependToSequence(Minimal_SyntaxTree type, Minimal_SyntaxTree v) {
    Minimal_SyntaxTree* old = type->branches;
    if(old == NULL) {
        type->branches = malloc(sizeof(void*));
        type->branches[0] = v;
        type->length = 1;
    } else {
        type->branches = malloc(sizeof(void*)*(type->length+1));
        memcpy(&(type->branches[1]), old, sizeof(void*)*type->length);
        type->branches[0] = v;
        type->length++;
    }
    return type;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createTypeSpec(Minimal_SyntaxTree id, Minimal_SyntaxTree type_spec) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_TYPE_SPEC;
    tree->type_id = NULL;
    tree->type_name = malloc(strlen(id->string) + 1);
    strcpy(tree->type_name, id->string);
    Minimal_delReference(id);
    tree->type_def = type_spec;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createProductType() {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_PRODUCT_TYPE;
    tree->type_id = NULL;
    tree->length = 0;
    tree->branches = NULL;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_addToProductType(Minimal_SyntaxTree type, Minimal_SyntaxTree v) {
    Minimal_SyntaxTree* old = type->branches;
    if(old == NULL) {
        type->branches = malloc(sizeof(void*));
        type->branches[0] = v;
        type->length = 1;
    } else {
        type->branches = malloc(sizeof(void*)*(type->length+1));
        memcpy(type->branches, old, sizeof(void*)*type->length);
        type->branches[type->length] = v;
        type->length++;
        free(old);
    }
    return type;
}

Minimal_SyntaxTree Minimal_SyntaxTree_prependToProductType(Minimal_SyntaxTree type, Minimal_SyntaxTree v) {
    Minimal_SyntaxTree* old = type->branches;
    if(old == NULL) {
        type->branches = malloc(sizeof(void*));
        type->branches[0] = v;
        type->length = 1;
    } else {
        type->branches = malloc(sizeof(void*)*(type->length+1));
        memcpy(&(type->branches[1]), old, sizeof(void*)*type->length);
        type->branches[0] = v;
        type->length++;
        free(old);
    }
    return type;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createSumType() {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_SUM_TYPE;
    tree->type_id = NULL;
    tree->length = 0;
    tree->branches = NULL;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_addToSumType(Minimal_SyntaxTree type, Minimal_SyntaxTree v) {
    Minimal_SyntaxTree* old = type->branches;
    if(old == NULL) {
        type->branches = malloc(sizeof(void*));
        type->branches[0] = v;
        type->length = 1;
    } else {
        type->branches = malloc(sizeof(void*)*(type->length+1));
        memcpy(type->branches, old, sizeof(void*)*type->length);
        type->branches[type->length] = v;
        type->length++;
        free(old);
    }
    return type;
}

Minimal_SyntaxTree Minimal_SyntaxTree_prependToSumType(Minimal_SyntaxTree type, Minimal_SyntaxTree v) {
    Minimal_SyntaxTree* old = type->branches;
    if(old == NULL) {
        type->branches = malloc(sizeof(void*));
        type->branches[0] = v;
        type->length = 1;
    } else {
        type->branches = malloc(sizeof(void*)*(type->length+1));
        memcpy(&(type->branches[1]), old, sizeof(void*)*type->length);
        type->branches[0] = v;
        type->length++;
    }
    return type;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createFunction(Minimal_SyntaxTree arg, Minimal_SyntaxTree value) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_TYPE_FUNCTION;
    tree->type_id = NULL;
    tree->size = 2;
    tree->branches = malloc(sizeof(void*)*2);
    tree->branches[0] = arg;
    tree->branches[1] = value;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createFunctionCall(Minimal_SyntaxTree func, Minimal_SyntaxTree args) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_FUNCTION_CALL;
    tree->type_id = NULL;
    tree->function = func;
    tree->arguments = args;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createLet(Minimal_SyntaxTree var, Minimal_SyntaxTree expr, Minimal_SyntaxTree code) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_LET;
    tree->var = var;
    tree->letexpr = expr;
    tree->code = code;

    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createFunctionDef(Minimal_SyntaxTree name, Minimal_SyntaxTree args, Minimal_SyntaxTree expr) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_FUNCTION_DEF;
    tree->type_id = NULL;
    tree->func_name = malloc(strlen(name->string)+1);
    strcpy(tree->func_name, name->string);
    Minimal_delReference(name);
    tree->type_spec = NULL;
    tree->parameter_list = args;
    tree->body = expr;
    return tree;
}

void Minimal_SyntaxTree_addToTuple(Minimal_SyntaxTree tuple, Minimal_SyntaxTree tree) {
    if(tuple->type != M_SYNTAX_TREE && tuple->type != ST_TUPLE) {
        fprintf(stderr, "Invalid type to Minimal_SyntaxTree_addToTuple %i\n", tuple->type);
        return;
    }
    if(tuple->size == 0) {
        tuple->tuple = (Minimal_SyntaxTree*)malloc(sizeof(void*));
        tuple->tuple[0] = tree;
    } else {
        Minimal_SyntaxTree* newt = (Minimal_SyntaxTree*)malloc(sizeof(void*)*(tuple->size+1));
        memcpy(newt, tuple->tuple, sizeof(void*)*(tuple->size));
        newt[tuple->size] = tree;
        free(tuple->tuple);
        tuple->tuple = newt;
    }
    tuple->size = tuple->size + 1;
    return;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createArgumentList1(Minimal_SyntaxTree s1) {
    if(s1->type == ST_ARGUMENT_LIST) {
        return s1;
    } else {
        Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
        tree->type = ST_ARGUMENT_LIST;
        tree->type_id = NULL;
        tree->argument = s1;
        tree->next_arg = NULL;
        return tree;
    }
}

Minimal_SyntaxTree Minimal_SyntaxTree_createArgumentList2(Minimal_SyntaxTree s1, Minimal_SyntaxTree s2) {
    if(s1->type == ST_ARGUMENT_LIST && s2->type == ST_ARGUMENT_LIST) {
        Minimal_SyntaxTree ptr = s1;
        while(ptr->next_arg != NULL) { ptr = ptr->next_arg; }
        ptr->next_arg = s2;
        return s1;
    } else if(s1->type == ST_ARGUMENT_LIST) {
        Minimal_SyntaxTree ptr = s1;
        while(ptr->next_arg != NULL) { ptr = ptr->next_arg; }
        ptr->next_arg = Minimal_SyntaxTree_createArgumentList1(s2);
        return s1;
    } else if(s2->type == ST_ARGUMENT_LIST) {
        Minimal_SyntaxTree ptr = Minimal_SyntaxTree_createArgumentList1(s1);
        return Minimal_SyntaxTree_createArgumentList2(ptr, s2);
    } else {
        return Minimal_SyntaxTree_createArgumentList2(Minimal_SyntaxTree_createArgumentList1(s1), s2);
    }
}
Minimal_SyntaxTree Minimal_SyntaxTree_createParameterList1(Minimal_SyntaxTree s1) {
    if(s1->type == ST_PARAMETER_LIST) {
        return s1;
    } else {
        Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
        tree->type = ST_PARAMETER_LIST;
        tree->type_id = NULL;
        tree->var_name = malloc(strlen(s1->string)+1);
        strcpy(tree->var_name, s1->string);
        /*Minimal_delReference(s1);*/
        tree->next_var = NULL;
        return tree;
    }
}

Minimal_SyntaxTree Minimal_SyntaxTree_createParameterList2(Minimal_SyntaxTree s1, Minimal_SyntaxTree s2) {
    if(s1->type == ST_PARAMETER_LIST && s2->type == ST_PARAMETER_LIST) {
        Minimal_SyntaxTree ptr = s1;
        while(ptr->next_var != NULL) { ptr = ptr->next_var; }
        ptr->next_var = s2;
        return s1;
    } else if(s1->type == ST_PARAMETER_LIST) {
        Minimal_SyntaxTree ptr = s1;
        while(ptr->next_var != NULL) { ptr = ptr->next_var; }
        ptr->next_var = Minimal_SyntaxTree_createParameterList1(s2);
        return s1;
    } else if(s2->type == ST_PARAMETER_LIST) {
        Minimal_SyntaxTree ptr = s2;
        while(ptr->next_var != NULL) { ptr = ptr->next_var; }
        ptr->next_var = Minimal_SyntaxTree_createParameterList1(s1);
        return s2;
    } else {
        return Minimal_SyntaxTree_createParameterList2(Minimal_SyntaxTree_createParameterList1(s1), s2);
    }
}

Minimal_SyntaxTree Minimal_SyntaxTree_createPointer(Minimal_SyntaxTree v1) {
    Minimal_SyntaxTree tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree, struct Minimal_SyntaxTree_t);
    tree->type = ST_POINTER;
    tree->type_id = NULL;
    tree->ptr = malloc(strlen(v1->string) + 1);
    strcpy(tree->ptr, v1->string);
    Minimal_delReference(v1);
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_copy(Minimal_SyntaxTree tree) {
    Minimal_SyntaxTree ntree = NULL;
    if(tree == NULL) { return NULL; }
    ntree = (Minimal_SyntaxTree)malloc(sizeof(struct Minimal_SyntaxTree_t));
    if(tree->type_id == NULL) {
        ntree->type_id = NULL;
    } else {
        ntree->type_id = malloc(strlen(tree->type_id) + 1);
        strcpy(ntree->type_id, tree->type_id);
    }
    switch(tree->type) {
    case ST_BLANK:
        ntree->type = ST_BLANK;
        return ntree;
    case ST_NIL:
        ntree->type = ST_NIL;
        return ntree;
    case ST_IDENTIFIER:
        ntree->type = ST_IDENTIFIER;
        ntree->string = (char*)malloc(strlen(tree->string)+1);
        strcpy(ntree->string, tree->string);
        return ntree;
    case ST_INTEGER:
        ntree->type = ST_INTEGER;
        ntree->integer = tree->integer;
        return ntree;
    case ST_STRING:
        ntree->type = ST_STRING;
        ntree->string = malloc(strlen(tree->string)+1);
        strcpy(ntree->string, tree->string);
        return ntree;
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        ntree->type = ST_SEQENTIAL_DEFS;
        ntree->length = tree->length;
        ntree->branches = (Minimal_SyntaxTree*)malloc(sizeof(void*)*ntree->length);
        for(i = 0; i < tree->length; i++) {
            Minimal_addReference(tree->branches[i]);
            ntree->branches[i] = tree->branches[i];
        }
        return ntree;
        }
    case ST_TYPE_SPEC:
        ntree->type = ST_TYPE_SPEC;
        ntree->type_name = (char*)malloc(strlen(tree->type_name)+1);
        strcpy(ntree->type_name, tree->type_name);
        Minimal_addReference(tree->type_def);
        ntree->type_def = tree->type_def;
        return ntree;
    case ST_TYPE_FUNCTION:
        ntree->type = ST_TYPE_FUNCTION;
        ntree->length = 2;
        ntree->branches = (Minimal_SyntaxTree*)malloc(sizeof(void*)*tree->length);
        Minimal_addReference(tree->branches[0]);
        Minimal_addReference(tree->branches[1]);
        ntree->branches[0] = tree->branches[0];
        ntree->branches[1] = tree->branches[1];
        return ntree;
    case ST_FUNCTION_DEF:
        ntree->type = ST_FUNCTION_DEF;
        ntree->func_name = (char*)malloc(strlen(tree->func_name)+1);
        strcpy(ntree->func_name, tree->func_name);
        Minimal_addReference(tree->parameter_list);
        ntree->parameter_list = tree->parameter_list;
        Minimal_addReference(tree->body);
        ntree->body = tree->body;
        return ntree;
    case ST_PARAMETER_LIST:
        ntree->type = ST_PARAMETER_LIST;
        ntree->var_name = (char*)malloc(strlen(tree->var_name)+1);
        strcpy(ntree->var_name, tree->var_name);
        Minimal_addReference(tree->next_var);
        ntree->next_var = tree->next_var;
        return ntree;
    case ST_FUNCTION_CALL:
        ntree->type = ST_FUNCTION_CALL;
        Minimal_addReference(tree->function);
        ntree->function = tree->function;
        Minimal_addReference(tree->arguments);
        ntree->arguments = tree->arguments;
        return ntree;
    case ST_LET:
        ntree->type = ST_LET;
        Minimal_addReference(tree->var);
        ntree->var = tree->var;
        Minimal_addReference(tree->letexpr);
        ntree->letexpr = tree->letexpr;
        Minimal_addReference(tree->code);
        ntree->code = tree->code;
        return ntree;
    case ST_ARGUMENT_LIST:
        ntree->type = ST_ARGUMENT_LIST;
        Minimal_addReference(tree->argument);
        ntree->argument = tree->argument;
        Minimal_addReference(tree->next_arg);
        ntree->next_arg = tree->next_arg;
        return ntree;
    case ST_OPERATOR:
        ntree->type = ST_OPERATOR;
        ntree->_operator = (char*)malloc(strlen(tree->_operator)+1);
        strcpy(ntree->_operator, tree->_operator);
        Minimal_addReference(tree->op1);
        ntree->op1 = tree->op1;
        Minimal_addReference(tree->op2);
        ntree->op2 = tree->op2;
        return ntree;
    case ST_PRODUCT_TYPE:
        {
        int i;
        ntree->type = ST_PRODUCT_TYPE;
        ntree->length = tree->length;
        ntree->branches = (Minimal_SyntaxTree*)malloc(sizeof(void*)*ntree->length);
        for(i = 0; i < ntree->length; i++) {
            Minimal_addReference(tree->branches[i]);
            ntree->branches[i] = tree->branches[i];
        }
        return ntree;
        }
    case ST_SUM_TYPE:
        {
        int i;
        ntree->type = ST_SUM_TYPE;
        ntree->length = tree->length;
        ntree->branches = (Minimal_SyntaxTree*)malloc(sizeof(void*)*ntree->length);
        for(i = 0; i < ntree->length; i++) {
            Minimal_addReference(tree->branches[i]);
            ntree->branches[i] = tree->branches[i];
        }
        return ntree;
        }
    case ST_TUPLE:
        {
        int i;
        ntree->type = ST_TUPLE;
        ntree->size = tree->size;
        ntree->tuple = (Minimal_SyntaxTree*)malloc(sizeof(void*)*tree->size);
        for(i=0; i<tree->size; i++) {
            Minimal_addReference(tree->tuple[i]);
            ntree->tuple[i] = tree->tuple[i];
        }
        return ntree;
        }
    case ST_POINTER:
        ntree->type = ST_POINTER;
        ntree->ptr = (char*)malloc(strlen(tree->ptr)+1);
        strcpy(ntree->ptr, tree->ptr);
        return ntree;
    case ST_IFEXPR:
        ntree->type = ST_IFEXPR;
        Minimal_addReference(tree->_if);
        ntree->_if = tree->_if;
        Minimal_addReference(tree->tuple);
        ntree->_then = tree->_then;
        Minimal_addReference(tree->_else);
        ntree->_else = tree->_else;
        return ntree;
    case ST_BRACKET:
        ntree->type = ST_BRACKET;
        Minimal_addReference(tree->type_def);
        ntree->type_def = tree->type_def;
    case ST_INDEX:
        ntree->type = ST_BRACKET;
        Minimal_addReference(tree->expr);
        ntree->expr = tree->expr;
        ntree->index = tree->index;
    }
    return NULL;
}


void Minimal_SyntaxTree_getReferences(struct CyclicGarbageList* list, Minimal_SyntaxTree ptr) {
    switch(ptr->type) {
    case ST_BLANK:
    case ST_NIL:
    case ST_IDENTIFIER:
    case ST_INTEGER:
    case ST_STRING:
        break;
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        for(i=0; i<ptr->size; i++) {
            Minimal_addToCyclicGarbageList(list, ptr->branches[i]);
        }
        break;
        }
    case ST_TYPE_SPEC:
        Minimal_addToCyclicGarbageList(list, ptr->type_def);
        break;
    case ST_TYPE_FUNCTION:
        {
        int i;
        for(i=0; i<ptr->size; i++) {
            Minimal_addToCyclicGarbageList(list, ptr->branches[i]);
        }
        break;
        }
    case ST_FUNCTION_DEF:
        if(ptr->type_spec != NULL) {
            Minimal_addToCyclicGarbageList(list, ptr->type_spec);
        }
        Minimal_addToCyclicGarbageList(list, ptr->parameter_list);
        Minimal_addToCyclicGarbageList(list, ptr->body);
        break;
    case ST_PARAMETER_LIST:
        Minimal_addToCyclicGarbageList(list, ptr->next_var);
        break;
    case ST_FUNCTION_CALL:
        Minimal_addToCyclicGarbageList(list, ptr->function);
        Minimal_addToCyclicGarbageList(list, ptr->arguments);
        break;
    case ST_LET:
        Minimal_addToCyclicGarbageList(list, ptr->var);
        Minimal_addToCyclicGarbageList(list, ptr->letexpr);
        Minimal_addToCyclicGarbageList(list, ptr->code);
        break;
    case ST_ARGUMENT_LIST:
        Minimal_addToCyclicGarbageList(list, ptr->argument);
        Minimal_addToCyclicGarbageList(list, ptr->next_arg);
        break;
    case ST_OPERATOR:
        Minimal_addToCyclicGarbageList(list, ptr->op1);
        Minimal_addToCyclicGarbageList(list, ptr->op2);
        break;
    case ST_PRODUCT_TYPE:
        {
        int i;
        for(i=0; i<ptr->size; i++) {
            Minimal_addToCyclicGarbageList(list, ptr->branches[i]);
        }
        break;
        }
    case ST_SUM_TYPE:
        {
        int i;
        for(i=0; i<ptr->size; i++) {
            Minimal_addToCyclicGarbageList(list, ptr->branches[i]);
        }
        break;
        }
    case ST_TUPLE:
        {
        int i;
        for(i=0; i<ptr->size; i++) {
            Minimal_addToCyclicGarbageList(list, ptr->branches[i]);
        }
        break;
        }
    case ST_POINTER:
        break;
    case ST_IFEXPR:
        Minimal_addToCyclicGarbageList(list, ptr->_if);
        Minimal_addToCyclicGarbageList(list, ptr->_then);
        Minimal_addToCyclicGarbageList(list, ptr->_else);
        break;
    case ST_BRACKET:
        Minimal_addToCyclicGarbageList(list, ptr->type_def);
        break;
    case ST_INDEX:
        Minimal_addToCyclicGarbageList(list, ptr->expr);
        Minimal_addToCyclicGarbageList(list, ptr->index);
        break;
    }
}

int Minimal_SyntaxTree_cmp(Minimal_SyntaxTree t1, Minimal_SyntaxTree t2) {
    if(t1 == NULL && t2 == NULL) {
        return 0;
    } else if(t1 == NULL) {
        return -1;
    } else if(t2 == NULL) {
        return 1;
    } else if(t1->type < t2->type) {
        return -1;
    } else if(t1->type > t2->type) {
        return 1;
    }

    switch(t1->type) {
    case ST_BLANK:
        return 0;
    case ST_NIL:
        return 0;
    case ST_IDENTIFIER:
        return strcmp(t1->string, t2->string);
    case ST_INTEGER:
        if(t1->integer < t2->integer) {
            return -1;
        } else if(t1->integer == t2->integer) {
            return 0;
        } else if(t1->integer > t2->integer) {
            return 1;
        }
    case ST_STRING:
        return strcmp(t1->string, t2->string);
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        int c;
        if(t1->length < t2->length) {
            return -1;
        } else if(t1->length > t2->length) {
            return 1;
        }
        for(i = 0; i < t1->length; i++) {
            c = Minimal_SyntaxTree_cmp(t1->branches[i], t2->branches[i]);
            if(c != 0) {
                return c;
            }
        }
        return 0;
        }
    case ST_TYPE_SPEC:
        if(strcmp(t1->type_name, t2->type_name) != 0) {
            return strcmp(t1->type_name, t2->type_name);
        } else {
            return Minimal_SyntaxTree_cmp(t1->type_def, t2->type_def);
        }
    case ST_TYPE_FUNCTION:
        {
        int i;
        int c;
        if(t1->length < t2->length) {
            return -1;
        } else if(t1->length > t2->length) {
            return 1;
        }
        for(i = 0; i < t1->length; i++) {
            c = Minimal_SyntaxTree_cmp(t1->branches[i], t2->branches[i]);
            if(c != 0) {
                return c;
            }
        }
        return 0;
        }
    case ST_FUNCTION_DEF:
        if(strcmp(t1->func_name, t2->func_name) != 0) {
            return strcmp(t1->func_name, t2->func_name);
        } else if(Minimal_SyntaxTree_cmp(t1->parameter_list, t2->parameter_list)) {
            return Minimal_SyntaxTree_cmp(t1->parameter_list, t2->parameter_list);
        } else {
            return Minimal_SyntaxTree_cmp(t1->body, t2->body);
        }
    case ST_PARAMETER_LIST:
        if(strcmp(t1->var_name, t2->var_name) != 0) {
            return strcmp(t1->var_name, t2->var_name);
        } else {
            if(t1->next_var == NULL && t2->next_var == NULL) {
                return 0;
            } else if(t1->next_var == NULL) {
                return -1;
            } else if(t1->next_var == NULL) {
                return 1;
            } else {
                return Minimal_SyntaxTree_cmp(t1->next_var, t2->next_var);
            }
        }
    case ST_FUNCTION_CALL:
        if(Minimal_SyntaxTree_cmp(t1->function, t2->function)) {
            return Minimal_SyntaxTree_cmp(t1->function, t2->function);
        } else {
            return Minimal_SyntaxTree_cmp(t1->arguments, t2->arguments);
        }
    case ST_LET:
        if(Minimal_SyntaxTree_cmp(t1->var, t2->var)) {
            if(Minimal_SyntaxTree_cmp(t1->letexpr, t2->letexpr)) {
                return Minimal_SyntaxTree_cmp(t1->code, t2->code);
            } else {
                return Minimal_SyntaxTree_cmp(t1->letexpr, t2->letexpr);
            }
        } else {
            return Minimal_SyntaxTree_cmp(t1->var, t2->var);
        }
    case ST_ARGUMENT_LIST:
        if(Minimal_SyntaxTree_cmp(t1->argument, t2->argument) != 0) {
            return Minimal_SyntaxTree_cmp(t1->argument, t2->argument);
        } else {
            if(t1->next_arg == NULL && t2->next_arg == NULL) {
                return 0;
            } else if(t1->next_arg == NULL) {
                return -1;
            } else if(t1->next_arg == NULL) {
                return 1;
            } else {
                return Minimal_SyntaxTree_cmp(t1->next_arg, t2->next_arg);
            }
        }
    case ST_OPERATOR:
        if(strcmp(t1->_operator, t2->_operator) != 0) {
            return strcmp(t1->_operator, t2->_operator);
        } else if(Minimal_SyntaxTree_cmp(t1->op1, t2->op1)) {
            return Minimal_SyntaxTree_cmp(t1->op1, t2->op1);
        } else {
            return Minimal_SyntaxTree_cmp(t1->op2, t2->op2);
        }
    case ST_PRODUCT_TYPE:
        {
        int i;
        int c;
        if(t1->length < t2->length) {
            return -1;
        } else if(t1->length > t2->length) {
            return 1;
        }
        for(i = 0; i < t1->length; i++) {
            c = Minimal_SyntaxTree_cmp(t1->branches[i], t2->branches[i]);
            if(c != 0) {
                return c;
            }
        }
        return 0;
        }
    case ST_SUM_TYPE:
        {
        int i;
        int c;
        if(t1->length < t2->length) {
            return -1;
        } else if(t1->length > t2->length) {
            return 1;
        }
        for(i = 0; i < t1->length; i++) {
            c = Minimal_SyntaxTree_cmp(t1->branches[i], t2->branches[i]);
            if(c != 0) {
                return c;
            }
        }
        return 0;
        }
    case ST_TUPLE:
        if(t1->size < t2->size) {
            return -1;
        } else if(t1->size > t2->size) {
            return 1;
        } else {
            int i;
            for(i=0; i<t1->size; i++) {
                if(Minimal_SyntaxTree_cmp(t1->tuple[i], t2->tuple[i])) {
                    return Minimal_SyntaxTree_cmp(t1->tuple[i], t2->tuple[i]);
                }
            }
            return 0;
        }
    case ST_POINTER:
        return strcmp(t1->ptr, t2->ptr);
    case ST_IFEXPR:
        if(Minimal_SyntaxTree_cmp(t1->_if, t2->_if)) {
            return Minimal_SyntaxTree_cmp(t1->_if, t2->_if);
        } else if(Minimal_SyntaxTree_cmp(t1->_then, t2->_then)) {
            return Minimal_SyntaxTree_cmp(t1->_then, t2->_then);
        } else {
            return Minimal_SyntaxTree_cmp(t1->_else, t2->_else);
        }
    case ST_BRACKET:
        return Minimal_SyntaxTree_cmp(t1->type_def, t2->type_def);
    case ST_INDEX:
        if(Minimal_SyntaxTree_cmp(t1->expr, t2->expr)) {
           return Minimal_SyntaxTree_cmp(t1->expr, t2->expr);
        } else {
            return Minimal_SyntaxTree_cmp(t1->index, t2->index);
        }
    }
    return 0;
}

void Minimal_SyntaxTree_free(Minimal_SyntaxTree tree) {
    if(tree == NULL) { return; }

    if(tree->type_id != NULL) {
        free(tree->type_id);
    }
    switch(tree->type) {
    case ST_BLANK:
    case ST_NIL:
        break;
    case ST_IDENTIFIER:
        free(tree->string);
        break;
    case ST_INTEGER:
        break;
    case ST_STRING:
        free(tree->string);
        break;
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_delReference(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_TYPE_SPEC:
        free(tree->type_name);
        Minimal_delReference(tree->type_def);
        break;
    case ST_TYPE_FUNCTION:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_delReference(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_FUNCTION_DEF:
        free(tree->func_name);
        if(tree->type_spec != NULL) {
            Minimal_delReference(tree->type_spec);
        }
        Minimal_delReference(tree->parameter_list);
        Minimal_delReference(tree->body);
        break;
    case ST_PARAMETER_LIST:
        free(tree->var_name);
        if(tree->next_arg != NULL) {
            Minimal_delReference(tree->next_var);
        }
        break;
    case ST_FUNCTION_CALL:
        Minimal_delReference(tree->function);
        Minimal_delReference(tree->arguments);
        break;
    case ST_LET:
        Minimal_delReference(tree->var);
        Minimal_delReference(tree->letexpr);
        Minimal_delReference(tree->code);
        break;
    case ST_ARGUMENT_LIST:
        Minimal_delReference(tree->argument);
        if(tree->next_arg != NULL) {
            Minimal_delReference(tree->next_arg);
        }
        break;
    case ST_OPERATOR:
        free(tree->_operator);
        Minimal_delReference(tree->op1);
        Minimal_delReference(tree->op2);
        break;
    case ST_PRODUCT_TYPE:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_delReference(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_SUM_TYPE:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_delReference(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_TUPLE:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_delReference(tree->tuple[i]);
        }
        free(tree->tuple);
        break;
        }
    case ST_POINTER:
        Minimal_delReference(tree->ptr);
        break;
    case ST_IFEXPR:
        Minimal_delReference(tree->_if);
        Minimal_delReference(tree->_then);
        Minimal_delReference(tree->_else);
        break;
    case ST_BRACKET:
        Minimal_delReference(tree->type_def);
        break;
    case ST_INDEX:
        Minimal_delReference(tree->expr);
        Minimal_delReference(tree->index);
        break;
    }
}
