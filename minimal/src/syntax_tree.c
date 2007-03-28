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

MinimalValue Minimal_SyntaxTree_createBlank(char* id) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_BLANK;
    tree->syntax_tree->type_id = NULL;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createNil(char* id) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_NIL;
    tree->syntax_tree->type_id = NULL;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createID(char* id) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_IDENTIFIER;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->string = (char*)malloc(strlen(id)+1);
    strcpy(tree->syntax_tree->string, id);
    return tree;
}

MinimalValue Minimal_SyntaxTree_createInteger(int i) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_INTEGER;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->integer = i;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createOperator(MinimalValue op, MinimalValue expr1, MinimalValue expr2) {
    if(op->type != M_SYNTAX_TREE || op->syntax_tree->type != ST_IDENTIFIER) {
        fprintf(stderr, "Invalid type of operator to Minimal_SyntaxTree_createOperator\n");
        return NULL;
    }
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_OPERATOR;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->string = (char*)malloc(strlen(op->syntax_tree->string)+1);
    strcpy(tree->syntax_tree->_operator, op->syntax_tree->string);
    Minimal_delReference(op);
    tree->syntax_tree->op1 = expr1;
    tree->syntax_tree->op2 = expr2;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createTuple(int size) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_TUPLE;
    tree->syntax_tree->type_id = NULL;
    if(size == 0) {
        tree->syntax_tree->tuple = NULL;
    } else {
        int i;
        tree->syntax_tree->tuple = (MinimalValue*)malloc(sizeof(void*)*size);
        for(i=0; i<size; i++) {
            tree->syntax_tree->tuple[i] = NULL;
        }
    }
    tree->syntax_tree->size = size;
    return tree;
}

void Minimal_SyntaxTree_setTuple(MinimalValue tup, int i, MinimalValue v) {
    if(tup->syntax_tree->tuple[i] != NULL) {
        Minimal_delReference(tup->syntax_tree->tuple[i]);
    }
    tup->syntax_tree->tuple[i] = v;
}

MinimalValue Minimal_SyntaxTree_createIfExpr(MinimalValue test, MinimalValue expr1, MinimalValue expr2) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_IFEXPR;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->_if = test;
    tree->syntax_tree->_then = expr1;
    tree->syntax_tree->_else = expr2;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createSequence() {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_SEQENTIAL_DEFS;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->length = 0;
    tree->syntax_tree->branches = NULL;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createSequence2(MinimalValue s1, MinimalValue s2) {
    if(s1->syntax_tree->type == ST_SEQENTIAL_DEFS && s2->syntax_tree->type == ST_SEQENTIAL_DEFS) {
        int i;
        for(i = 0; i < s2->syntax_tree->length; i++) {
            Minimal_addReference(s2->syntax_tree->branches[i]);
            Minimal_SyntaxTree_addToSequence(s1, s2->syntax_tree->branches[i]);
        }
        return s1;
    } else if(s1->syntax_tree->type == ST_SEQENTIAL_DEFS) {
        Minimal_SyntaxTree_addToSequence(s1, s2);
        return s1;
    } else if(s2->syntax_tree->type == ST_SEQENTIAL_DEFS) {
        Minimal_SyntaxTree_prependToSequence(s2, s1);
        return s2;
    } else {
        MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
        tree->type = M_SYNTAX_TREE;
        tree->typeobj = NULL;
        tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
        tree->syntax_tree->type = ST_SEQENTIAL_DEFS;
        tree->syntax_tree->type_id = NULL;
        tree->syntax_tree->length = 2;
        tree->syntax_tree->branches = malloc(sizeof(void*)*2);
        tree->syntax_tree->branches[0] = s1;
        tree->syntax_tree->branches[1] = s2;
        return tree;
    }
}

MinimalValue Minimal_SyntaxTree_addToSequence(MinimalValue type, MinimalValue v) {
    MinimalValue* old = type->syntax_tree->branches;
    if(old == NULL) {
        type->syntax_tree->branches = malloc(sizeof(void*));
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length = 1;
    } else {
        type->syntax_tree->branches = malloc(sizeof(void*)*(type->syntax_tree->length+1));
        memcpy(type->syntax_tree->branches, old, sizeof(void*)*type->syntax_tree->length);
        type->syntax_tree->branches[type->syntax_tree->length] = v;
        type->syntax_tree->length++;
    }
    return type;
}

MinimalValue Minimal_SyntaxTree_prependToSequence(MinimalValue type, MinimalValue v) {
    MinimalValue* old = type->syntax_tree->branches;
    if(old == NULL) {
        type->syntax_tree->branches = malloc(sizeof(void*));
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length = 1;
    } else {
        type->syntax_tree->branches = malloc(sizeof(void*)*(type->syntax_tree->length+1));
        memcpy(&(type->syntax_tree->branches[1]), old, sizeof(void*)*type->syntax_tree->length);
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length++;
    }
    return type;
}

MinimalValue Minimal_SyntaxTree_createTypeSpec(MinimalValue id, MinimalValue type_spec) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_TYPE_SPEC;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->type_name = malloc(strlen(id->syntax_tree->string) + 1);
    strcpy(tree->syntax_tree->type_name, id->syntax_tree->string);
    Minimal_delReference(id);
    tree->syntax_tree->type_def = type_spec;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createProductType() {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_PRODUCT_TYPE;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->length = 0;
    tree->syntax_tree->branches = NULL;
    return tree;
}

MinimalValue Minimal_SyntaxTree_addToProductType(MinimalValue type, MinimalValue v) {
    MinimalValue* old = type->syntax_tree->branches;
    if(old == NULL) {
        type->syntax_tree->branches = malloc(sizeof(void*));
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length = 1;
    } else {
        type->syntax_tree->branches = malloc(sizeof(void*)*(type->syntax_tree->length+1));
        memcpy(type->syntax_tree->branches, old, sizeof(void*)*type->syntax_tree->length);
        type->syntax_tree->branches[type->syntax_tree->length] = v;
        type->syntax_tree->length++;
    }
    return type;
}

MinimalValue Minimal_SyntaxTree_prependToProductType(MinimalValue type, MinimalValue v) {
    MinimalValue* old = type->syntax_tree->branches;
    if(old == NULL) {
        type->syntax_tree->branches = malloc(sizeof(void*));
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length = 1;
    } else {
        type->syntax_tree->branches = malloc(sizeof(void*)*(type->syntax_tree->length+1));
        memcpy(&(type->syntax_tree->branches[1]), old, sizeof(void*)*type->syntax_tree->length);
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length++;
    }
    return type;
}

MinimalValue Minimal_SyntaxTree_createSumType() {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_SUM_TYPE;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->length = 0;
    tree->syntax_tree->branches = NULL;
    return tree;
}

MinimalValue Minimal_SyntaxTree_addToSumType(MinimalValue type, MinimalValue v) {
    MinimalValue* old = type->syntax_tree->branches;
    if(old == NULL) {
        type->syntax_tree->branches = malloc(sizeof(void*));
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length = 1;
    } else {
        type->syntax_tree->branches = malloc(sizeof(void*)*(type->syntax_tree->length+1));
        memcpy(type->syntax_tree->branches, old, sizeof(void*)*type->syntax_tree->length);
        type->syntax_tree->branches[type->syntax_tree->length] = v;
        type->syntax_tree->length++;
    }
    return type;
}

MinimalValue Minimal_SyntaxTree_prependToSumType(MinimalValue type, MinimalValue v) {
    MinimalValue* old = type->syntax_tree->branches;
    if(old == NULL) {
        type->syntax_tree->branches = malloc(sizeof(void*));
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length = 1;
    } else {
        type->syntax_tree->branches = malloc(sizeof(void*)*(type->syntax_tree->length+1));
        memcpy(&(type->syntax_tree->branches[1]), old, sizeof(void*)*type->syntax_tree->length);
        type->syntax_tree->branches[0] = v;
        type->syntax_tree->length++;
    }
    return type;
}

MinimalValue Minimal_SyntaxTree_createFunction(MinimalValue arg, MinimalValue value) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_TYPE_FUNCTION;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->size = 2;
    tree->syntax_tree->branches = malloc(sizeof(void*)*2);
    tree->syntax_tree->branches[0] = arg;
    tree->syntax_tree->branches[1] = value;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createFunctionCall(MinimalValue func, MinimalValue args) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_FUNCTION_CALL;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->function = func;
    tree->syntax_tree->arguments = args;
    return tree;
}

MinimalValue Minimal_SyntaxTree_createFunctionDef(MinimalValue name, MinimalValue args, MinimalValue expr) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_FUNCTION_DEF;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->func_name = malloc(sizeof(name->syntax_tree->string)+1);
    strcpy(tree->syntax_tree->func_name, name->syntax_tree->string);
    Minimal_delReference(name);
    tree->syntax_tree->type_spec = NULL;
    tree->syntax_tree->parameter_list = args;
    tree->syntax_tree->body = expr;
    return tree;
}

void Minimal_SyntaxTree_addToTuple(MinimalValue tuple, MinimalValue tree) {
    if(tuple->type != M_SYNTAX_TREE || tuple->syntax_tree->type != ST_TUPLE) {
        fprintf(stderr, "Invalid type of operator to Minimal_SyntaxTree_createOperator\n");
        return;
    }
    if(tuple->syntax_tree->size == 0) {
        tuple->syntax_tree->tuple = (MinimalValue*)malloc(sizeof(void*));
        tuple->syntax_tree->tuple[0] = tree;
    } else {
        MinimalValue* newt = (MinimalValue*)malloc(sizeof(void*)*(tuple->syntax_tree->size+1));
        memcpy(newt, tuple->syntax_tree->tuple, sizeof(void*)*(tuple->syntax_tree->size));
        newt[tuple->syntax_tree->size] = tree;
        free(tuple->syntax_tree->tuple);
        tuple->syntax_tree->tuple = newt;
    }
    tuple->syntax_tree->size = tuple->syntax_tree->size + 1;
    return;
}

MinimalValue Minimal_SyntaxTree_createArgumentList1(MinimalValue s1) {
    if(s1->syntax_tree->type == ST_ARGUMENT_LIST) {
        return s1;
    } else {
        MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
        tree->type = M_SYNTAX_TREE;
        tree->typeobj = NULL;
        tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
        tree->syntax_tree->type = ST_ARGUMENT_LIST;
        tree->syntax_tree->type_id = NULL;
        tree->syntax_tree->argument = s1;
        tree->syntax_tree->next_arg = NULL;
        return tree;
    }
}

MinimalValue Minimal_SyntaxTree_createArgumentList2(MinimalValue s1, MinimalValue s2) {

    if(s1->syntax_tree->type == ST_ARGUMENT_LIST && s2->syntax_tree->type == ST_ARGUMENT_LIST) {
        MinimalValue ptr = s1;
        while(ptr->syntax_tree->next_arg != NULL) { ptr = ptr->syntax_tree->next_arg; }
        ptr->syntax_tree->next_arg = s2;
        return s1;
    } else if(s1->syntax_tree->type == ST_ARGUMENT_LIST) {
        MinimalValue ptr = s1;
        while(ptr->syntax_tree->next_arg != NULL) { ptr = ptr->syntax_tree->next_arg; }
        ptr->syntax_tree->next_arg = Minimal_SyntaxTree_createArgumentList1(s2);
        return s1;
    } else if(s2->syntax_tree->type == ST_ARGUMENT_LIST) {
        MinimalValue ptr = s2;
        while(ptr->syntax_tree->next_arg != NULL) { ptr = ptr->syntax_tree->next_arg; }
        ptr->syntax_tree->next_arg = Minimal_SyntaxTree_createArgumentList1(s1);
        return s2;
    } else {
        return Minimal_SyntaxTree_createParameterList2(Minimal_SyntaxTree_createArgumentList1(s1), s2);
    }
}
MinimalValue Minimal_SyntaxTree_createParameterList1(MinimalValue s1) {
    if(s1->syntax_tree->type == ST_PARAMETER_LIST) {
        return s1;
    } else {
        MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
        tree->type = M_SYNTAX_TREE;
        tree->typeobj = NULL;
        tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
        tree->syntax_tree->type = ST_PARAMETER_LIST;
        tree->syntax_tree->type_id = NULL;
        tree->syntax_tree->var_name = malloc(strlen(s1->syntax_tree->string)+1);
        strcpy(tree->syntax_tree->var_name, s1->syntax_tree->string);
        Minimal_delReference(s1);
        tree->syntax_tree->next_var = NULL;
        return tree;
    }
}

MinimalValue Minimal_SyntaxTree_createParameterList2(MinimalValue s1, MinimalValue s2) {
    if(s1->syntax_tree->type == ST_PARAMETER_LIST && s2->syntax_tree->type == ST_PARAMETER_LIST) {
        MinimalValue ptr = s1;
        while(ptr->syntax_tree->next_var != NULL) { ptr = ptr->syntax_tree->next_var; }
        ptr->syntax_tree->next_var = s2;
        return s1;
    } else if(s1->syntax_tree->type == ST_PARAMETER_LIST) {
        MinimalValue ptr = s1;
        while(ptr->syntax_tree->next_var != NULL) { ptr = ptr->syntax_tree->next_var; }
        ptr->syntax_tree->next_var = Minimal_SyntaxTree_createParameterList1(s2);
        return s1;
    } else if(s2->syntax_tree->type == ST_PARAMETER_LIST) {
        MinimalValue ptr = s2;
        while(ptr->syntax_tree->next_var != NULL) { ptr = ptr->syntax_tree->next_var; }
        ptr->syntax_tree->next_var = Minimal_SyntaxTree_createParameterList1(s1);
        return s2;
    } else {
        return Minimal_SyntaxTree_createParameterList2(Minimal_SyntaxTree_createParameterList1(s1), s2);
    }
}

MinimalValue Minimal_SyntaxTree_createPointer(MinimalValue v1) {
    MinimalValue tree = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    tree->type = M_SYNTAX_TREE;
    tree->typeobj = NULL;
    tree->syntax_tree = Minimal_newReference(MINIMAL_SYNTAXTREE, Minimal_SyntaxTree*, struct Minimal_SyntaxTree_t);
    tree->syntax_tree->type = ST_TYPE_FUNCTION;
    tree->syntax_tree->type_id = NULL;
    tree->syntax_tree->ptr = malloc(strlen(v1->syntax_tree->string) + 1);
    strcpy(tree->syntax_tree->ptr, v1->syntax_tree->string);
    Minimal_delReference(v1);
    return tree;
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
        ntree->branches = (MinimalValue*)malloc(sizeof(void*)*ntree->length);
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
        ntree->branches = (MinimalValue*)malloc(sizeof(void*)*tree->length);
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
        ntree->branches = (MinimalValue*)malloc(sizeof(void*)*ntree->length);
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
        ntree->branches = (MinimalValue*)malloc(sizeof(void*)*ntree->length);
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
        ntree->tuple = (MinimalValue*)malloc(sizeof(void*)*tree->size);
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
    }
    return NULL;
}


void Minimal_SyntaxTree_getReferences(struct CyclicGarbageList* list, Minimal_SyntaxTree* ptr) {
    switch(ptr->type) {
    case ST_BLANK:
    case ST_NIL:
    case ST_IDENTIFIER:
    case ST_INTEGER:
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
            c = Minimal_SyntaxTree_cmp(t1->branches[i]->syntax_tree, t2->branches[i]->syntax_tree);
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
            return Minimal_SyntaxTree_cmp(t1->type_def->syntax_tree, t2->type_def->syntax_tree);
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
            c = Minimal_SyntaxTree_cmp(t1->branches[i]->syntax_tree, t2->branches[i]->syntax_tree);
            if(c != 0) {
                return c;
            }
        }
        return 0;
        }
    case ST_FUNCTION_DEF:
        if(strcmp(t1->func_name, t2->func_name) != 0) {
            return strcmp(t1->func_name, t2->func_name);
        } else if(Minimal_SyntaxTree_cmp(t1->parameter_list->syntax_tree, t2->parameter_list->syntax_tree)) {
            return Minimal_SyntaxTree_cmp(t1->parameter_list->syntax_tree, t2->parameter_list->syntax_tree);
        } else {
            return Minimal_SyntaxTree_cmp(t1->body->syntax_tree, t2->body->syntax_tree);
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
                return Minimal_SyntaxTree_cmp(t1->next_var->syntax_tree, t2->next_var->syntax_tree);
            }
        }
    case ST_FUNCTION_CALL:
        if(Minimal_SyntaxTree_cmp(t1->function->syntax_tree, t2->function->syntax_tree)) {
            return Minimal_SyntaxTree_cmp(t1->function->syntax_tree, t2->function->syntax_tree);
        } else {
            return Minimal_SyntaxTree_cmp(t1->arguments->syntax_tree, t2->arguments->syntax_tree);
        }
    case ST_ARGUMENT_LIST:
        if(Minimal_SyntaxTree_cmp(t1->argument->syntax_tree, t2->argument->syntax_tree) != 0) {
            return Minimal_SyntaxTree_cmp(t1->argument->syntax_tree, t2->argument->syntax_tree);
        } else {
            if(t1->next_arg == NULL && t2->next_arg == NULL) {
                return 0;
            } else if(t1->next_arg == NULL) {
                return -1;
            } else if(t1->next_arg == NULL) {
                return 1;
            } else {
                return Minimal_SyntaxTree_cmp(t1->next_arg->syntax_tree, t2->next_arg->syntax_tree);
            }
        }
    case ST_OPERATOR:
        if(strcmp(t1->_operator, t2->_operator) != 0) {
            return strcmp(t1->_operator, t2->_operator);
        } else if(Minimal_SyntaxTree_cmp(t1->op1->syntax_tree, t2->op1->syntax_tree)) {
            return Minimal_SyntaxTree_cmp(t1->op1->syntax_tree, t2->op1->syntax_tree);
        } else {
            return Minimal_SyntaxTree_cmp(t1->op2->syntax_tree, t2->op2->syntax_tree);
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
            c = Minimal_SyntaxTree_cmp(t1->branches[i]->syntax_tree, t2->branches[i]->syntax_tree);
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
            c = Minimal_SyntaxTree_cmp(t1->branches[i]->syntax_tree, t2->branches[i]->syntax_tree);
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
                if(Minimal_SyntaxTree_cmp(t1->tuple[i]->syntax_tree, t2->tuple[i]->syntax_tree)) {
                    return Minimal_SyntaxTree_cmp(t1->tuple[i]->syntax_tree, t2->tuple[i]->syntax_tree);
                }
            }
            return 0;
        }
    case ST_POINTER:
        return strcmp(t1->ptr, t2->ptr);
    case ST_IFEXPR:
        if(Minimal_SyntaxTree_cmp(t1->_if->syntax_tree, t2->_if->syntax_tree)) {
            return Minimal_SyntaxTree_cmp(t1->_if->syntax_tree, t2->_if->syntax_tree);
        } else if(Minimal_SyntaxTree_cmp(t1->_then->syntax_tree, t2->_then->syntax_tree)) {
            return Minimal_SyntaxTree_cmp(t1->_then->syntax_tree, t2->_then->syntax_tree);
        } else {
            return Minimal_SyntaxTree_cmp(t1->_else->syntax_tree, t2->_else->syntax_tree);
        }
    }
    return 0;
}

void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree) {
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
        free(tree->ptr);
        break;
    case ST_IFEXPR:
        free(tree->_if);
        free(tree->_then);
        free(tree->_else);
        break;
    }

    free(tree);
}
