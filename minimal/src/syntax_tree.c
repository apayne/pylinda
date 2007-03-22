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

Minimal_SyntaxTree Minimal_SyntaxTree_createID(char* id) {
    Minimal_SyntaxTree tree;
    tree.type = ST_IDENTIFIER;
    tree.type_id = NULL;
    tree.string = (char*)malloc(strlen(id)+1);
    strcpy(tree.string, id);
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createInteger(int i) {
    Minimal_SyntaxTree tree;
    tree.type = ST_INTEGER;
    tree.type_id = NULL;
    tree.integer = i;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createOperator(char* op) {
    Minimal_SyntaxTree tree;
    tree.type = ST_OPERATOR;
    tree.type_id = NULL;
    tree.string = (char*)malloc(strlen(op)+1);
    strcpy(tree._operator, op);
    tree.op1 = NULL;
    tree.op2 = NULL;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createTuple(int size) {
    Minimal_SyntaxTree tree;
    tree.type = ST_TUPLE;
    tree.type_id = NULL;
    if(size == 0) {
        tree.tuple = NULL;
    } else {
        int i;
        tree.tuple = (Minimal_SyntaxTree**)malloc(sizeof(void*)*size);
        for(i=0; i<size; i++) {
            tree.tuple[i] = NULL;
        }
    }
    tree.size = size;
    return tree;
}


Minimal_SyntaxTree Minimal_SyntaxTree_createIfExpr(int size) {
    Minimal_SyntaxTree tree;
    tree.type = ST_IFEXPR;
    tree.type_id = NULL;
    tree._if = NULL;
    tree._then = NULL;
    tree._else = NULL;
    return tree;
}

void Minimal_SyntaxTree_addToTuple(Minimal_SyntaxTree* tuple, Minimal_SyntaxTree* tree) {
    if(tuple->size == 0) {
        tuple->tuple = (Minimal_SyntaxTree**)malloc(sizeof(void*));
        tuple->tuple[0] = tree;
    } else {
        Minimal_SyntaxTree** newt = (Minimal_SyntaxTree**)malloc(sizeof(void*)*(tuple->size+1));
        memcpy(newt, tuple->tuple, sizeof(void*)*(tuple->size));
        newt[tuple->size] = Minimal_SyntaxTree_copy(tree);
        free(tuple->tuple);
        tuple->tuple = newt;
    }
    tuple->size = tuple->size + 1;
    return;
}

Minimal_SyntaxTree* Minimal_SyntaxTree_copy(Minimal_SyntaxTree* tree) {
    Minimal_SyntaxTree* ntree = NULL;
    if(tree == NULL) { return NULL; }
    ntree = (Minimal_SyntaxTree*)malloc(sizeof(struct Minimal_SyntaxTree_t));
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
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        ntree->type = ST_SEQENTIAL_DEFS;
        ntree->length = tree->length;
        ntree->branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*)*ntree->length);
        for(i = 0; i < tree->length; i++) {
            ntree->branches[i] = Minimal_SyntaxTree_copy(tree->branches[i]);
        }
        return ntree;
        }
    case ST_TYPE_SPEC:
        ntree->type = ST_TYPE_SPEC;
        ntree->type_name = (char*)malloc(strlen(tree->type_name)+1);
        strcpy(ntree->type_name, tree->type_name);
        ntree->type_def = Minimal_SyntaxTree_copy(tree->type_def);
        return ntree;
    case ST_TYPE_FUNCTION:
        ntree->type = ST_TYPE_FUNCTION;
        ntree->length = 2;
        ntree->branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*)*tree->length);
        ntree->branches[0] = Minimal_SyntaxTree_copy(tree->branches[0]);
        ntree->branches[1] = Minimal_SyntaxTree_copy(tree->branches[1]);
        return ntree;
    case ST_FUNCTION_DEF:
        ntree->type = ST_FUNCTION_DEF;
        ntree->func_name = (char*)malloc(strlen(tree->func_name)+1);
        strcpy(ntree->func_name, tree->func_name);
        ntree->parameter_list = Minimal_SyntaxTree_copy(tree->parameter_list);
        ntree->body = Minimal_SyntaxTree_copy(tree->body);
        return ntree;
    case ST_PARAMETER_LIST:
        ntree->type = ST_PARAMETER_LIST;
        ntree->var_name = (char*)malloc(strlen(tree->var_name)+1);
        strcpy(ntree->var_name, tree->var_name);
        ntree->next_var = Minimal_SyntaxTree_copy(tree->next_var);
        return ntree;
    case ST_FUNCTION_CALL:
        ntree->type = ST_FUNCTION_CALL;
        ntree->function = Minimal_SyntaxTree_copy(tree->function);
        ntree->arguments = Minimal_SyntaxTree_copy(tree->arguments);
        return ntree;
    case ST_ARGUMENT_LIST:
        ntree->type = ST_ARGUMENT_LIST;
        ntree->argument = Minimal_SyntaxTree_copy(tree->argument);
        ntree->next_arg = Minimal_SyntaxTree_copy(tree->next_arg);
        return ntree;
    case ST_OPERATOR:
        ntree->type = ST_OPERATOR;
        ntree->_operator = (char*)malloc(strlen(tree->_operator)+1);
        strcpy(ntree->_operator, tree->_operator);
        ntree->op1 = Minimal_SyntaxTree_copy(tree->op1);
        ntree->op2 = Minimal_SyntaxTree_copy(tree->op2);
        return ntree;
    case ST_PRODUCT_TYPE:
        {
        int i;
        ntree->type = ST_PRODUCT_TYPE;
        ntree->length = tree->length;
        ntree->branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*)*ntree->length);
        for(i = 0; i < ntree->length; i++) {
            ntree->branches[i] = Minimal_SyntaxTree_copy(tree->branches[i]);
        }
        return ntree;
        }
    case ST_SUM_TYPE:
        {
        int i;
        ntree->type = ST_SUM_TYPE;
        ntree->length = tree->length;
        ntree->branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*)*ntree->length);
        for(i = 0; i < ntree->length; i++) {
            ntree->branches[i] = Minimal_SyntaxTree_copy(tree->branches[i]);
        }
        return ntree;
        }
    case ST_TUPLE:
        {
        int i;
        ntree->type = ST_TUPLE;
        ntree->size = tree->size;
        ntree->tuple = (Minimal_SyntaxTree**)malloc(sizeof(void*)*tree->size);
        for(i=0; i<tree->size; i++) {
            ntree->tuple[i] = Minimal_SyntaxTree_copy(tree->tuple[i]);
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
        ntree->_if = Minimal_SyntaxTree_copy(tree->_if);
        ntree->_then = Minimal_SyntaxTree_copy(tree->_then);
        ntree->_then = Minimal_SyntaxTree_copy(tree->_then);
        return ntree;
    }
    return NULL;
}

void Minimal_SyntaxTree_clear(Minimal_SyntaxTree* tree) {
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
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_SyntaxTree_free(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_TYPE_SPEC:
        free(tree->type_name);
        Minimal_SyntaxTree_free(tree->type_def);
        break;
    case ST_TYPE_FUNCTION:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_SyntaxTree_free(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_FUNCTION_DEF:
        free(tree->func_name);
        Minimal_SyntaxTree_free(tree->parameter_list);
        Minimal_SyntaxTree_free(tree->body);
        break;
    case ST_PARAMETER_LIST:
        free(tree->var_name);
        Minimal_SyntaxTree_free(tree->next_var);
        break;
    case ST_FUNCTION_CALL:
        Minimal_SyntaxTree_free(tree->function);
        Minimal_SyntaxTree_free(tree->arguments);
        break;
    case ST_ARGUMENT_LIST:
        Minimal_SyntaxTree_free(tree->argument);
        Minimal_SyntaxTree_free(tree->next_arg);
        break;
    case ST_OPERATOR:
        free(tree->_operator);
        Minimal_SyntaxTree_free(tree->op1);
        Minimal_SyntaxTree_free(tree->op2);
        break;
    case ST_PRODUCT_TYPE:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_SyntaxTree_free(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_SUM_TYPE:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_SyntaxTree_free(tree->branches[i]);
        }
        free(tree->branches);
        break;
        }
    case ST_TUPLE:
        {
        int i;
        for(i=0; i<tree->size; i++) {
            Minimal_SyntaxTree_free(tree->tuple[i]);
        }
        free(tree->tuple);
        break;
        }
    case ST_POINTER:
        free(tree->ptr);
        break;
    case ST_IFEXPR:
        free(tree->_if);
        free(tree->_then);
        free(tree->_else);
        break;
    }
}

int Minimal_SyntaxTree_cmp(Minimal_SyntaxTree* t1, Minimal_SyntaxTree* t2) {
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
    }
    return 0;
}

void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree) {
    if(tree == NULL) { return; }

    Minimal_SyntaxTree_clear(tree);

    free(tree);
}
