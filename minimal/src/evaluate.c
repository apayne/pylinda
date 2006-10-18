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
#include <readline/readline.h>
#include <readline/history.h>

#include "minimal_internal.h"

MinimalValue Minimal_evaluate(Minimal_SyntaxTree* tree, MinimalLayer layer) {
    switch(tree->type) {
    case ST_BLANK:
        return Minimal_nil();
    case ST_INTEGER:
        return Minimal_int(tree->integer);
    case ST_SEQENTIAL_DEFS:
        Minimal_evaluate(tree->branch1, layer);
        return Minimal_evaluate(tree->branch2, layer);
    case ST_TYPE_SPEC:
    case ST_TYPE_FUNCTION:
    case ST_FUNCTION_DEF:
    case ST_PARAMETER_LIST:
    case ST_FUNCTION_CALL:
        {
        MinimalValue function = Minimal_evaluate(tree->function, layer);
        if(function->type != FUNCTION) { fprintf(stderr, "Error, didn't get function for function call.\n"); return Minimal_nil() }
        MinimalLayer new_layer = Minimal_createLayer2(function->layer);
        Minimal_SyntaxTree* param = function->parameter_list;
        Minimal_SyntaxTree* arg = tree->arguments;
        while(arg != NULL && arg->type != ST_BLANK) {
            Minimal_addName(new_layer, param->var_name, Minimal_evaluate(arg->argument, layer));

            param = param->next_var; arg = arg->next_arg;
        }
        MinimalValue r = Minimal_evaluate(function->tree, new_layer);
        Minimal_delReference(new_layer);
        return r;
    case ST_ARGUMENT_LIST:
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_evaluate (%i)\n", tree->type);
        return Minimal_nil();
    }
}
