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

MinimalValue Minimal_Nil;

MinimalValue Minimal_nil() {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = NIL;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

unsigned char Minimal_isInt(MinimalValue v) {
    return v->type == INTEGER;
}

MinimalValue Minimal_int(int i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = INTEGER;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

int Minimal_getInt(MinimalValue v) {
    return v->integer;
}

unsigned char Minimal_isFloat(MinimalValue v) {
    return v->type == FLOAT;
}

MinimalValue Minimal_float(float f) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = FLOAT;
    v->floating = f;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

float Minimal_getFloat(MinimalValue v) {
    return v->floating;
}

unsigned char Minimal_isString(MinimalValue v) {
    return v->type == STRING;
}

MinimalValue Minimal_string(char* s) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = STRING;
    v->length = strlen(s) + 1;
    v->string = malloc(v->length);
    memcpy(v->string, s, v->length);
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_string2(char* s, unsigned int len) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = STRING;
    v->length = len;
    v->string = malloc(v->length);
    memcpy(v->string, s, v->length);
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

char* Minimal_getString(MinimalValue v) {
    return v->string;
}

unsigned int Minimal_getStringLen(MinimalValue v) {
    return v->length;
}

unsigned char Minimal_isTypeSpec(MinimalValue v) {
    return v->type == TYPE;
}

MinimalValue Minimal_typeSpec(char* type_name, Minimal_SyntaxTree* type_spec) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = TYPE;
    v->type_name = (char*)malloc(strlen(type_name)+1);
    strcpy(v->type_name, type_name);
    v->type_spec = Minimal_SyntaxTree_copy(type_spec);
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_function(char* func_name, Minimal_SyntaxTree* func_type, Minimal_SyntaxTree* parameter_list, Minimal_SyntaxTree* code) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = FUNCTION;
    v->func_name = (char*)malloc(strlen(func_name)+1);
    strcpy(v->func_name, func_name);
    v->func_type = Minimal_SyntaxTree_copy(func_type);
    v->parameter_list = Minimal_SyntaxTree_copy(parameter_list);
    v->code = Minimal_SyntaxTree_copy(code);
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_type(char* typespec) {
    Minimal_SyntaxTree* tree = Minimal_parseTypeSpec(typespec);
    if(tree == NULL) {
        fprintf(stderr, "Error: Tried to parse type spec but got NULL\n");
        return NULL;
    } else if(tree->type != ST_TYPE_SPEC) {
        fprintf(stderr, "Error: Tried to parse type spec but didn't get a type spec back - got %i\n", tree->type);
        return NULL;
    }
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);

    v->type = TYPE;
    v->type_name = (char*)malloc(strlen(tree->type_name)+1); strcpy(v->type_name, tree->type_name);
    v->type_spec = Minimal_SyntaxTree_copy(tree->type_def);
    v->typeobj = NULL;
    v->sum_pos = -1;

    Minimal_SyntaxTree_free(tree);

    return v;
}

MinimalValue Minimal_tuple(int size) {
    int i;
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = TUPLE;
    v->size = size;
    v->values = (MinimalValue*)malloc(sizeof(void*)*size);
    for(i=0; i<size; i++) {
        v->values[i] = NULL;
    }
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

void Minimal_tupleSet(MinimalValue tuple, int size, MinimalValue value) {
    if(size > tuple->size) {
        int i;
        MinimalValue* ptrs = (MinimalValue*)malloc(sizeof(void*)*(size+1));
        memcpy(ptrs, tuple->values, sizeof(void*)*tuple->size);
        for(i=tuple->size; i<=size; i++) {
            ptrs[i] = NULL;
        }
        free(tuple->values);
        tuple->values = ptrs;
    }
    if(tuple->values[size] != NULL) {
        Minimal_delReference(tuple->values[size]);
    }
    tuple->values[size] = value;
}

MinimalValue Minimal_tupleGet(MinimalValue tuple, int pos) {
    if(pos < 0 || pos > tuple->size) {
        return NULL;
    } else {
        return tuple->values[pos];
    }
}

void Minimal_setType(MinimalValue value, MinimalValue type) {
    if(value->typeobj != NULL) {
        Minimal_delReference(value->typeobj);
    }
    Minimal_addReference(type);
    value->typeobj = type;
}

void Minimal_setSumPos(MinimalValue value, int sum_pos) {
    value->sum_pos = sum_pos;
}

char* Minimal_Value_string(MinimalValue v) {
    char* r;
    if(v == NULL) {
        r = (char*)malloc(strlen("(null)")+1);
        strcpy(r, "(null)");
        return r;
    }

    switch(v->type) {
    case NIL:
        r = (char*)malloc(strlen("Nil")+1);
        strcpy(r, "Nil");
        return r;
    case BOOLEAN:
        if(v->boolean) {
            r = (char*)malloc(strlen("True")+1);
            strcpy(r, "True");
            return r;
        } else {
            r = (char*)malloc(strlen("False")+1);
            strcpy(r, "False");
            return r;
        }
    case INTEGER:
        {
        int size = snprintf(r, 0, "%li", v->integer);
        r = (char*)malloc(size + 1);
        sprintf(r, "%li", v->integer);
        return r;
        }
    case FLOAT:
        {
        int size = snprintf(r, 0, "%f", v->floating);
        r = (char*)malloc(size + 1);
        sprintf(r, "%f", v->floating);
        return r;
        }
    case STRING:
        {
        int size = snprintf(r, 0, "%s", v->string);
        r = (char*)malloc(size + 1);
        sprintf(r, "%s", v->string);
        return r;
        }
    case TYPE:
        r = (char*)malloc(strlen("<Type>")+1);
        strcpy(r, "<Type>");
        return r;
    case TSREF:
        r = (char*)malloc(strlen("<TSRef>")+1);
        strcpy(r, "<TSRef>");
        return r;
    case TUPLE:
        r = (char*)malloc(strlen("<Tuple>")+1);
        strcpy(r, "<Tuple>");
        return r;
    case FUNCTION:
        r = (char*)malloc(strlen("<Function>")+1);
        strcpy(r, "<Function>");
        return r;
    default:
        fprintf(stderr, "Unknown value type in Minimal_Value_string (%i)\n", v->type);
        return NULL;
    }
}

void Minimal_Value_free(MinimalValue v) {
    if(v == NULL) { return; }

    if(v->typeobj != NULL) {
        Minimal_delReference(v->typeobj);
    }

    switch(v->type) {
    case NIL:
        break;
    case BOOLEAN:
        break;
    case INTEGER:
        break;
    case FLOAT:
        break;
    case STRING:
        free(v->string);
        break;
    case TYPE:
        free(v->type_name);
        Minimal_SyntaxTree_free(v->type_spec);
        break;
    case TSREF:
        break;
    case TUPLE:
        {
        int i;
        for(i=0; i<v->size; i++) {
            Minimal_delReference(v->values[i]);
        }
        free(v->values);
        break;
        }
    case FUNCTION:
        free(v->func_name);
        Minimal_SyntaxTree_free(v->func_type);
        Minimal_SyntaxTree_free(v->parameter_list);
        Minimal_SyntaxTree_free(v->code);
        Minimal_delReference(v->layer);
        break;
    default:
        fprintf(stderr, "Unknown value type in Minimal_Value_free (%i)\n", v->type);
        return;
    }
    free(v);
}
