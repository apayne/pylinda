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

#include <stdio.h>
#include <stdlib.h>
#ifndef NO_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "minimal_internal.h"

MinimalValue Minimal_apply(MinimalValue func, MinimalValue args) {
    MinimalLayer new_layer;
    Minimal_SyntaxTree* param;
    int i;
    MinimalValue r;

    if(func->type != M_FUNCTION) { fprintf(stderr, "Error: Minimal_eval didn't get function for function call.\n"); return NULL; }
    new_layer = Minimal_createLayer2(func->layer);
    param = func->parameter_list;
    for(i=0; i < args->size; i++) {
        MinimalValue param_val = Minimal_tupleGet(args, i);
        Minimal_addReference(param_val);
        Minimal_addName(&(new_layer->map), param->var_name, param_val);

        param = param->next_var;
    }
    r = Minimal_evaluate(func->code, new_layer);
    Minimal_delReference(new_layer);
    return r;
}

MinimalValue Minimal_evaluate(Minimal_SyntaxTree* tree, MinimalLayer layer) {
    switch(tree->type) {
    case ST_BLANK:
        return Minimal_nil();
    case ST_IDENTIFIER:
        {
        MinimalValue r = Minimal_getName(layer, tree->string);
        if(r == NULL) { fprintf(stderr, "Error: Unknown variable name %s.\n", tree->string); };
        return r;
        }
    case ST_INTEGER:
        return Minimal_int(tree->integer);
    case ST_SEQENTIAL_DEFS:
        {
        int i;
        MinimalValue v;
        for(i = 0; i < tree->length-1; i++) {
            v = Minimal_evaluate(tree->branches[i], layer);
            if(v != NULL) { Minimal_delReference(v); }
        }
        return Minimal_evaluate(tree->branches[i+1], layer);
        }
    case ST_TYPE_SPEC:
        Minimal_Layer_addTree(layer, tree);
        return Minimal_getName(layer, tree->type_name);
    case ST_FUNCTION_DEF:
        Minimal_Layer_addTree(layer, tree);
        return Minimal_getName(layer, tree->type_name);
    case ST_FUNCTION_CALL:
        {
        MinimalLayer new_layer;
        Minimal_SyntaxTree* param;
        Minimal_SyntaxTree* arg;
        MinimalValue r;

        MinimalValue function = Minimal_evaluate(tree->function, layer);
        if(function == NULL) { return NULL; }
        if(function->type != M_FUNCTION) { fprintf(stderr, "Error: Didn't get function for function call.\n"); return NULL; }
        new_layer = Minimal_createLayer2(function->layer);
        if(new_layer == NULL) { fprintf(stderr, "Error: Got NULL when creating new layer.\n"); return NULL; }
        param = function->parameter_list;
        if(param == NULL) { fprintf(stderr, "Error: Parameter list for %s is NULL.\n", function->func_name); return NULL; }
        arg = tree->arguments;
        while(arg != NULL && arg->type != ST_BLANK) {
            MinimalValue param_val = Minimal_evaluate(arg->argument, layer);
            if(param == NULL) { fprintf(stderr, "Error: Param is NULL.\n"); return NULL; }
            if(param_val == NULL) { fprintf(stderr, "Error: Got NULL when evalutating parameter.\n"); return NULL; }
            Minimal_addName(&(new_layer->map), param->var_name, param_val);

            param = param->next_var; arg = arg->next_arg;
        }
        r = Minimal_evaluate(function->code, new_layer);
        Minimal_delReference(new_layer);
        Minimal_delReference(function);
        return r;
        }
    case ST_OPERATOR:
        {
        MinimalValue r;
        MinimalValue op1 = Minimal_evaluate(tree->op1, layer);
        MinimalValue op2 = Minimal_evaluate(tree->op2, layer);
        if(strcmp(tree->_operator, "+") == 0) {
            r = Minimal_Value_add(op1, op2);
        } else if(strcmp(tree->_operator, "-") == 0) {
            r = Minimal_Value_sub(op1, op2);
        } else if(strcmp(tree->_operator, "*") == 0) {
            r = Minimal_Value_mul(op1, op2);
        } else if(strcmp(tree->_operator, "/") == 0) {
            r = Minimal_Value_div(op1, op2);
        }
        Minimal_delReference(op1);
        Minimal_delReference(op2);
        return r;
        }
    case ST_TUPLE:
        {
        int i;
        MinimalValue r = Minimal_tuple(0);
        for(i=0; i<tree->size; i++) {
            Minimal_tupleAdd(r, Minimal_evaluate(tree->tuple[i], layer));
        }
        return r;
        }
    default:
        fprintf(stderr, "Error: Unknown tree node type in Minimal_evaluate (%i)\n", tree->type);
        return NULL;
    }
}
