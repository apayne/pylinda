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
    tree.string = (char*)malloc(strlen(id)+1);
    strcpy(tree.string, id);
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createInteger(int i) {
    Minimal_SyntaxTree tree;
    tree.type = ST_INTEGER;
    tree.integer = i;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createOperator(char* op) {
    Minimal_SyntaxTree tree;
    tree.type = ST_OPERATOR;
    tree.string = (char*)malloc(strlen(op)+1);
    strcpy(tree._operator, op);
    tree.op1 = NULL;
    tree.op2 = NULL;
    return tree;
}

Minimal_SyntaxTree* Minimal_SyntaxTree_copy(Minimal_SyntaxTree* tree) {
    Minimal_SyntaxTree* ntree;
    if(tree == NULL) { return NULL; }
    ntree = (Minimal_SyntaxTree*)malloc(sizeof(struct Minimal_SyntaxTree_t));
    switch(tree->type) {
    case ST_BLANK:
        ntree->type = ST_BLANK;
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
        ntree->type = ST_SEQENTIAL_DEFS;
        ntree->branch1 = Minimal_SyntaxTree_copy(tree->branch1);
        ntree->branch2 = Minimal_SyntaxTree_copy(tree->branch2);
        return ntree;
    case ST_TYPE_SPEC:
        ntree->type = ST_TYPE_SPEC;
        ntree->type_name = (char*)malloc(strlen(tree->type_name)+1);
        strcpy(ntree->type_name, tree->type_name);
        ntree->type_def = Minimal_SyntaxTree_copy(tree->type_def);
        return ntree;
    case ST_TYPE_FUNCTION:
        ntree->type = ST_TYPE_FUNCTION;
        ntree->branch1 = Minimal_SyntaxTree_copy(tree->branch1);
        ntree->branch2 = Minimal_SyntaxTree_copy(tree->branch2);
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
        ntree->type = ST_PRODUCT_TYPE;
        ntree->branch1 = Minimal_SyntaxTree_copy(tree->branch1);
        ntree->branch2 = Minimal_SyntaxTree_copy(tree->branch2);
        return ntree;
    case ST_SUM_TYPE:
        ntree->type = ST_SUM_TYPE;
        ntree->branch1 = Minimal_SyntaxTree_copy(tree->branch1);
        ntree->branch2 = Minimal_SyntaxTree_copy(tree->branch2);
        return ntree;
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_SyntaxTree_copy (%i)\n", tree->type);
        free(ntree);
        return NULL;
    }
}

void Minimal_SyntaxTree_clear(Minimal_SyntaxTree* tree) {
    switch(tree->type) {
    case ST_BLANK:
        break;
    case ST_IDENTIFIER:
        free(tree->string);
        break;
    case ST_INTEGER:
        break;
    case ST_SEQENTIAL_DEFS:
        Minimal_SyntaxTree_free(tree->branch1);
        Minimal_SyntaxTree_free(tree->branch2);
        break;
    case ST_TYPE_SPEC:
        free(tree->type_name);
        Minimal_SyntaxTree_free(tree->type_def);
        break;
    case ST_TYPE_FUNCTION:
        Minimal_SyntaxTree_free(tree->branch1);
        Minimal_SyntaxTree_free(tree->branch2);
        break;
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
        Minimal_SyntaxTree_free(tree->branch1);
        Minimal_SyntaxTree_free(tree->branch2);
        break;
    case ST_SUM_TYPE:
        Minimal_SyntaxTree_free(tree->branch1);
        Minimal_SyntaxTree_free(tree->branch2);
        break;
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_SyntaxTree_clear (%i)\n", tree->type);
        break;
    }
}

void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree) {
    if(tree == NULL) { return; }

    Minimal_SyntaxTree_clear(tree);

    free(tree);
}
