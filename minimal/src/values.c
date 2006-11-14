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

unsigned char Minimal_isNil(MinimalValue v) {
    return v->type == NIL;
}

MinimalValue Minimal_nil() {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = NIL;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

unsigned char Minimal_isBool(MinimalValue v) {
    return v->type == BOOLEAN;
}

MinimalValue Minimal_bool(unsigned char b) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = BOOLEAN;
    v->boolean = b;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

unsigned char Minimal_getBool(MinimalValue v) {
    return v->boolean;
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

unsigned char Minimal_isType(MinimalValue v) {
    return v->type == TYPE;
}

MinimalValue Minimal_typeSpec(const char* type_name, Minimal_SyntaxTree* type_spec) {
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
    if(func_type != NULL) {
        v->func_type = Minimal_SyntaxTree_copy(func_type);
    } else {
        v->func_type = NULL;
    }
    v->parameter_list = Minimal_SyntaxTree_copy(parameter_list);
    v->code = Minimal_SyntaxTree_copy(code);
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_type(const char* typespec) {
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

unsigned char Minimal_isTuple(MinimalValue v) {
    return v->type == TUPLE;
}

MinimalValue Minimal_tuple(int size) {
    int i;
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = TUPLE;
    v->size = size;
    if(size == 0) {
        v->values = NULL;
    } else {
        v->values = (MinimalValue*)malloc(sizeof(void*)*size);
        for(i=0; i<size; i++) {
            v->values[i] = NULL;
        }
    }
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

int Minimal_getTupleSize(MinimalValue tuple) {
    return tuple->size;
}

void Minimal_tupleAdd(MinimalValue tuple, MinimalValue value) {
    Minimal_tupleSet(tuple, tuple->size, value);
}

void Minimal_tupleSet(MinimalValue tuple, int size, MinimalValue value) {
    if(size >= tuple->size) {
        int i;
        MinimalValue* ptrs = (MinimalValue*)malloc(sizeof(void*)*(size+1));
        memcpy(ptrs, tuple->values, sizeof(void*)*tuple->size);
        for(i=tuple->size; i<=size; i++) {
            ptrs[i] = NULL;
        }
        free(tuple->values);
        tuple->values = ptrs;
        tuple->size = size+1;
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

unsigned char Minimal_isPtr(MinimalValue v) {
    return v->type == POINTER;
}

MinimalValue Minimal_ptr(MinimalValue type, MinimalValue value) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = POINTER;
    if(value != NULL) {
        Minimal_addReference(value);
    }
    v->ptr = value;
    if(type != NULL) {
        Minimal_addReference(type);
    }
    v->ptr_type = type;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_getPtr(MinimalValue v) {
    return v->ptr;
}

MinimalValue Minimal_getPtrType(MinimalValue v) {
    return v->ptr_type;
}

unsigned char Minimal_isTupleSpace(MinimalValue v) {
    return v->type == TSREF;
}

MinimalValue Minimal_tupleSpace(const char* ts) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = TSREF;
    v->string = malloc(strlen(ts)+1);
    strcpy(v->string, ts);
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

char* Minimal_getTupleSpace(MinimalValue v) {
    return v->string;
}

void Minimal_setType(MinimalValue value, MinimalValue type) {
    if(value->typeobj != NULL) {
        Minimal_delReference(value->typeobj);
    }
    Minimal_addReference(type);
    value->typeobj = type;
}

MinimalValue Minimal_getType(MinimalValue value) {
    return value->typeobj;
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
        {
        int i;
        r = (char*)malloc(2);
        r[0] = '('; r[1] = '\0';
        for(i=0; i<v->size; i++) {
            char* tmp = Minimal_Value_string(v->values[i]);
            char* tmp2 = (char*)malloc(strlen(r)+strlen(tmp)+3);
            strcpy(tmp2, r); strcpy(&(tmp2[strlen(r)]), tmp);
            tmp2[strlen(r)+strlen(tmp)] = ','; tmp2[strlen(r)+strlen(tmp)+1] = ' '; tmp2[strlen(r)+strlen(tmp)+2] = '\0';
            free(r); free(tmp);
            r = tmp2;
        }
        char* tmp2 = (char*)malloc(strlen(r));
        memcpy(tmp2, r, strlen(r)); tmp2[strlen(r)-2] = ')'; tmp2[strlen(r)-1] = '\0';;
        free(r);
        return tmp2;
        }
    case FUNCTION:
        r = (char*)malloc(strlen("<Function>")+1);
        strcpy(r, "<Function>");
        return r;
    case POINTER:
        r = (char*)malloc(strlen("<Pointer ")+sizeof(void*)*2+3);
        sprintf(r, "<Pointer %p>", v->ptr);
        return r;
    default:
        fprintf(stderr, "Unknown value type in Minimal_Value_string (%i)\n", v->type);
        return NULL;
    }
}

MinimalValue Minimal_copy(MinimalValue v) {
    if(v == NULL) { return NULL; }

    MinimalValue nv = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    nv->type = v->type;
    if(v->typeobj != NULL) {
        Minimal_addReference(v->typeobj)
    }
    nv->typeobj = v->typeobj;
    nv->sum_pos = v->sum_pos;

    switch(v->type) {
    case NIL:
        break;
    case BOOLEAN:
        nv->boolean = v->boolean;
        break;
    case INTEGER:
        nv->integer = v->integer;
        break;
    case FLOAT:
        nv->floating = v->floating;
        break;
    case STRING:
        nv->string = (char*)malloc(v->length);
        memcpy(nv->string, v->string, v->length);
        nv->length = v->length;
        break;
    case TYPE:
        nv->type_name = (char*)malloc(strlen(v->type_name) + 1);
        strcpy(nv->type_name, v->type_name);
        nv->integer = v->integer;
        nv->type_spec = Minimal_SyntaxTree_copy(v->type_spec);
        break;
    case TSREF:
        nv->string = (char*)malloc(strlen(v->string) + 1);
        strcpy(nv->string, v->string);
        break;
    case TUPLE:
        {
        int i;
        nv->size = v->size;
        nv->values = malloc(sizeof(void*)*v->size);
        for(i=0; i<v->size; i++) {
            nv->values[i] = Minimal_copy(v->values[i]);
        }
        break;
        }
    case FUNCTION:
        nv->func_name = (char*)malloc(strlen(v->func_name) + 1);
        strcpy(nv->func_name, v->func_name);

        nv->func_type = Minimal_SyntaxTree_copy(v->func_type);
        nv->parameter_list = Minimal_SyntaxTree_copy(v->parameter_list);
        nv->code = Minimal_SyntaxTree_copy(v->code);
        nv->layer = v->layer;
        Minimal_delReference(nv->layer);
        break;
    case POINTER:
        Minimal_delReference(v->ptr_type);
        Minimal_delReference(v->ptr);
        break;
    default:
        fprintf(stderr, "Unknown value type in Minimal_copy (%i)\n", v->type);
        return NULL;
    }
    return nv;
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
        free(v->string);
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
    case POINTER:
        Minimal_delReference(v->ptr_type);
        Minimal_delReference(v->ptr);
        break;
    default:
        fprintf(stderr, "Unknown value type in Minimal_Value_free (%i)\n", v->type);
        return;
    }
    free(v);
}
