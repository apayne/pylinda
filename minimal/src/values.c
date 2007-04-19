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

#ifdef WIN32
#define snprintf _snprintf
#endif

EXPORT MinimalValue Minimal_Nil;

unsigned char Minimal_isNil(MinimalValue v) {
    return v->type == M_NIL;
}

MinimalValue Minimal_nil() {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_NIL;
    v->typeobj = NULL;
    Minimal_setType(v, Minimal_nilType);
    v->sum_pos = -1;
    return v;
}

unsigned char Minimal_isBool(MinimalValue v) {
    return v->type == M_BOOLEAN;
}

MinimalValue Minimal_bool(unsigned char b) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_BOOLEAN;
    v->boolean = b;
    v->typeobj = NULL;
    Minimal_setType(v, Minimal_boolType);
    v->sum_pos = -1;
    return v;
}

unsigned char Minimal_getBool(MinimalValue v) {
    return v->boolean;
}

unsigned char Minimal_isByte(MinimalValue v) {
    return v->type == M_BYTE;
}
unsigned char Minimal_isShort(MinimalValue v) {
    return v->type == M_SHORT;
}
unsigned char Minimal_isInt(MinimalValue v) {
    return v->type == M_INTEGER;
}
unsigned char Minimal_isLong(MinimalValue v) {
    return v->type == M_LONG;
}
unsigned char Minimal_isUByte(MinimalValue v) {
    return v->type == M_UBYTE;
}
unsigned char Minimal_isUShort(MinimalValue v) {
    return v->type == M_USHORT;
}
unsigned char Minimal_isUInt(MinimalValue v) {
    return v->type == M_UINTEGER;
}
unsigned char Minimal_isULong(MinimalValue v) {
    return v->type == M_ULONG;
}

MinimalValue Minimal_byte(char i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_BYTE;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_byteType);
    return v;
}
MinimalValue Minimal_short(short i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_SHORT;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_shortType);
    return v;
}
MinimalValue Minimal_int(int i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_INTEGER;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_intType);
    return v;
}
MinimalValue Minimal_long(long i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_LONG;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_longType);
    return v;
}

MinimalValue Minimal_ubyte(unsigned char i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_BYTE;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_ubyteType);
    return v;
}
MinimalValue Minimal_ushort(unsigned short i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_SHORT;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_ushortType);
    return v;
}
MinimalValue Minimal_uint(unsigned int i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_INTEGER;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_uintType);
    return v;
}
MinimalValue Minimal_ulong(unsigned long i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_LONG;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_ulongType);
    return v;
}

char Minimal_getByte(MinimalValue v) {
    return (char)v->integer;
}
short Minimal_getShort(MinimalValue v) {
    return (short)v->integer;
}
int Minimal_getInt(MinimalValue v) {
    return (int)v->integer;
}
long Minimal_getLong(MinimalValue v) {
    return (int)v->integer;
}

unsigned char Minimal_getUByte(MinimalValue v) {
    return (unsigned char)v->uinteger;
}
unsigned short Minimal_getUShort(MinimalValue v) {
    return (unsigned short)v->uinteger;
}
unsigned int Minimal_getUInt(MinimalValue v) {
    return (unsigned int)v->uinteger;
}
unsigned long Minimal_getULong(MinimalValue v) {
    return (unsigned int)v->uinteger;
}

unsigned char Minimal_isFloat(MinimalValue v) {
    return v->type == M_FLOAT;
}

MinimalValue Minimal_float(float f) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_FLOAT;
    v->singlefloat = f;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_floatType);
    return v;
}

float Minimal_getFloat(MinimalValue v) {
    return v->singlefloat;
}

unsigned char Minimal_isDouble(MinimalValue v) {
    return v->type == M_DOUBLE;
}

MinimalValue Minimal_double(double f) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_DOUBLE;
    v->doublefloat = f;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_doubleType);
    return v;
}

double Minimal_getDouble(MinimalValue v) {
    return v->doublefloat;
}

unsigned char Minimal_isString(MinimalValue v) {
    return v->type == M_STRING;
}

MinimalValue Minimal_string(char* s) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_STRING;
    v->length = strlen(s) + 1;
    v->string = malloc(v->length);
    memcpy(v->string, s, v->length);
    v->typeobj = NULL;
    v->sum_pos = -1;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_stringType);
    return v;
}

MinimalValue Minimal_string2(char* s, unsigned int len) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_STRING;
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
    return v->type == M_TYPE;
}

void (*Minimal_override_type_func)(MinimalValue t) = NULL;

void Minimal_setOverrideTypeFunc(void (*func)(MinimalValue t)) {
    Minimal_override_type_func = func;
}

void (*Minimal_linda_ts_addref_func)(MinimalValue t) = NULL;

void Minimal_setLindaTSAddRefFunc(void (*func)(MinimalValue t)) {
    Minimal_linda_ts_addref_func = func;
}

void (*Minimal_linda_ts_delref_func)(MinimalValue t) = NULL;

void Minimal_setLindaTSDelRefFunc(void (*func)(MinimalValue t)) {
    Minimal_linda_ts_delref_func = func;
}

MinimalValue Minimal_typeSpec(const char* type_name, Minimal_SyntaxTree type_spec) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_TYPE;
    v->type_name = (char*)malloc(strlen(type_name)+1);
    strcpy(v->type_name, type_name);
    if(type_spec->type == ST_TYPE_SPEC) {
        Minimal_addReference(type_spec->type_def);
        v->type_spec = type_spec->type_def;
        Minimal_delReference(type_spec);
    } else {
        v->type_spec = type_spec;
    }
    v->typeobj = NULL;
    v->typemap = Minimal_getCurrentLayer();
    v->type_id = 0;
    v->sum_pos = -1;
    if(Minimal_typeType != NULL) {
        Minimal_setType(v, Minimal_typeType);
    }

    if(Minimal_override_type_func != NULL) {
        Minimal_override_type_func(v);
    }

    return v;
}

MinimalValue Minimal_typeFromId(char* tid) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_TYPE;
    v->type_name = NULL;
    v->type_spec = NULL;
    v->typeobj = NULL;
    v->typemap = NULL;
    v->type_id = malloc(strlen(tid) + 1);
    strcpy(v->type_id, tid);
    v->sum_pos = -1;
    if(Minimal_typeType != NULL) {
        Minimal_setType(v, Minimal_typeType);
    }

    return v;
}

unsigned char Minimal_isFunction(MinimalValue v) {
    return v->type == M_FUNCTION;
}

MinimalValue Minimal_function(char* code) {
    MinimalValue f;
    MinimalLayer layer;
    Minimal_SyntaxTree tree = Minimal_parseCode(code);
    Minimal_SyntaxTree tree2;
    MinimalLayer l;
    if(tree == NULL) {
        return NULL;
    }
    l = Minimal_getCurrentLayer();
    Minimal_Layer_addTree(l, tree);
    Minimal_delReference(l);

    tree2 = tree;
    while(tree2->type != ST_TYPE_SPEC && tree2->type != ST_FUNCTION_DEF) {
        switch(tree2->type) {
        case ST_SEQENTIAL_DEFS:
            tree2 = tree2->branches[0];
            break;
        default:
            fprintf(stderr, "Error: Tried to parse function but didn't get a function back - got %i.\n", tree2->type);
            return NULL;
        }
    }

    layer = Minimal_getCurrentLayer();
    if(tree2->type == ST_TYPE_SPEC) {
        f = Minimal_getName(layer, tree2->type_name);
    } else {
        f = Minimal_getName(layer, tree2->func_name);
    }
    Minimal_delReference(layer);
    Minimal_delReference(tree);
    return f;
}

MinimalValue Minimal_function2(char* func_name, Minimal_SyntaxTree func_type, Minimal_SyntaxTree parameter_list, Minimal_SyntaxTree code) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_FUNCTION;
    v->func_name = (char*)malloc(strlen(func_name)+1);
    strcpy(v->func_name, func_name);
    v->parameter_list = parameter_list;
    if(v->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL.\n"); }
    v->layer = Minimal_getCurrentLayer();
    v->code = code;
    if(func_type != NULL) {
        Minimal_addReference(func_type);
        v->typeobj = Minimal_typeSpec(v->func_name, func_type);
    } else {
        v->typeobj = NULL;
    }
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_type(const char* typespec) {
    MinimalLayer layer;
    MinimalValue v;
    Minimal_SyntaxTree tree = Minimal_parseTypeSpec(typespec);
    if(tree == NULL) {
        fprintf(stderr, "Error: Tried to parse type spec but got NULL\n");
        return NULL;
    } else if(tree->type != ST_TYPE_SPEC) {
        fprintf(stderr, "Error: Tried to parse type spec but didn't get a type spec back - got %i\n", tree->type);
        return NULL;
    }
    layer = Minimal_getCurrentLayer();
    Minimal_Layer_addTree(layer, tree);
    v = Minimal_getName(layer, tree->type_name);
    Minimal_delReference(tree);
    Minimal_delReference(layer);

    return v;
}

unsigned char Minimal_isTuple(MinimalValue v) {
    return v->type == M_TUPLE;
}

MinimalValue Minimal_tuple(int size) {
    int i;
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_TUPLE;
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
    if(pos < 0 || pos >= tuple->size) {
        return NULL;
    } else {
        return tuple->values[pos];
    }
}

unsigned char Minimal_isPtr(MinimalValue v) {
    return v->type == M_POINTER;
}

MinimalValue Minimal_ptr(MinimalValue value) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_POINTER;
    if(value != NULL) {
        Minimal_addReference(value);
    }
    if(value != NULL && value->typeobj == NULL) {
        fprintf(stderr, "Minimal: Warning, pointer to value taken when value has no type.\n");
    }
    v->ptr = value;
    v->typeobj = NULL;
    v->sum_pos = -1;
    return v;
}

MinimalValue Minimal_getPtr(MinimalValue v) {
    return v->ptr;
}

void Minimal_setPtr(MinimalValue v, MinimalValue v2) {
    if(v->ptr != NULL) {
        Minimal_delReference(v->ptr);
    }
    v->ptr = v2;
}

MinimalValue Minimal_getPtrType(MinimalValue v) {
    return v->ptr->typeobj;
}

unsigned char Minimal_isTupleSpace(MinimalValue v) {
    return v->type == M_TSREF;
}

MinimalValue Minimal_tupleSpace(const char* ts) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = M_TSREF;
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
    MinimalValue tmp = value->typeobj;
    Minimal_addReference(type);
    value->typeobj = type;
    if(tmp != NULL) {
        Minimal_delReference(tmp);
    }
}

MinimalValue Minimal_getType(MinimalValue value) {
    return value->typeobj;
}

void Minimal_setSumPos(MinimalValue value, int sum_pos) {
    value->sum_pos = sum_pos;
}

int Minimal_getSumPos(MinimalValue value) {
    return value->sum_pos;
}

void Minimal_setTypeMap(MinimalValue v, MinimalLayer types) {
    MinimalLayer tmp;
    if(v->type == M_TYPE) {
        Minimal_addReference(types);
        tmp = v->typemap;
        v->typemap = types;
        if(tmp != NULL) {
            Minimal_delReference(tmp);
        }
    }
    if(v->typeobj != NULL) {
        Minimal_addReference(types);
        tmp = v->typeobj->typemap;
        v->typeobj->typemap = types;
        if(tmp != NULL) {
            Minimal_delReference(tmp);
        }
    }
}

unsigned char Minimal_isTrue(MinimalValue value) {
    switch(value->type) {
    case M_NIL:
        return 0;
    case M_BOOLEAN:
        return value->boolean;
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
        return value->integer != 0;
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
        return value->uinteger != 0;
    case M_FLOAT:
        return value->singlefloat != 0.0;
    case M_DOUBLE:
        return value->doublefloat != 0.0;
    case M_STRING:
        return value->length != 0;
    case M_TYPE:
    case M_TSREF:
        return 1;
    case M_TUPLE:
        return value->size != 0;
    case M_FUNCTION:
        return 1;
    case M_POINTER:
        return value->ptr != NULL;
    case M_SYNTAX_TREE:
        return 1;
    }
    return 0;
}

char* Minimal_Value_string(MinimalValue v) {
    char* r = NULL;
    if(v == NULL) {
        r = (char*)malloc(strlen("(null)")+1);
        strcpy(r, "(null)");
        return r;
    }

    switch(v->type) {
    case M_NIL:
        r = (char*)malloc(strlen("Nil")+1);
        strcpy(r, "Nil");
        return r;
    case M_BOOLEAN:
        if(v->boolean) {
            r = (char*)malloc(strlen("True")+1);
            strcpy(r, "True");
            return r;
        } else {
            r = (char*)malloc(strlen("False")+1);
            strcpy(r, "False");
            return r;
        }
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
        {
        int size = snprintf(r, 0, "%li", v->integer);
        r = (char*)malloc(size + 1);
        sprintf(r, "%li", v->integer);
        return r;
        }
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
        {
        int size = snprintf(r, 0, "%li", v->uinteger);
        r = (char*)malloc(size + 1);
        sprintf(r, "%li", v->uinteger);
        return r;
        }
    case M_FLOAT:
        {
        int size = snprintf(r, 0, "%f", v->singlefloat);
        r = (char*)malloc(size + 1);
        sprintf(r, "%f", v->singlefloat);
        return r;
        }
    case M_DOUBLE:
        {
        int size = snprintf(r, 0, "%f", v->doublefloat);
        r = (char*)malloc(size + 1);
        sprintf(r, "%f", v->doublefloat);
        return r;
        }
    case M_STRING:
        {
        int size = snprintf(r, 0, "%s", v->string);
        r = (char*)malloc(size + 1);
        sprintf(r, "%s", v->string);
        return r;
        }
    case M_TYPE:
        {
        if(v->type_name != NULL) {
            int size = snprintf(r, 0, "<Type %s>", v->type_name);
            r = (char*)malloc(size + 1);
            sprintf(r, "<Type %s>", v->type_name);
            return r;
        } else {
            int size = snprintf(r, 0, "<Type id=%s>", v->type_id);
            r = (char*)malloc(size + 1);
            sprintf(r, "<Type id=%s>", v->type_id);
            return r;
        }
        }
    case M_TSREF:
        r = (char*)malloc(strlen("<TSRef>")+1);
        strcpy(r, "<TSRef>");
        return r;
    case M_TUPLE:
        {
        int i;
        r = (char*)malloc(2);
        r[0] = '('; r[1] = '\0';
        for(i=0; i<v->size; i++) {
            char* tmp = Minimal_Value_string(v->values[i]);
            char* tmp2 = (char*)malloc(strlen(r)+strlen(tmp)+3);
            strcpy(tmp2, r); strcpy(&(tmp2[strlen(r)]), tmp);
            tmp2[strlen(r)+strlen(tmp)] = ','; tmp2[strlen(r)+strlen(tmp)+1] = ' '; tmp2[strlen(r)+strlen(tmp)+2] = '\0';
            free(r);
            free(tmp); tmp = NULL;
            r = tmp2;
        }
        if(strlen(r) == 1) {
            free(r);
            r = (char*)malloc(3);
            r[0] = '('; r[1] = ')'; r[2] = '\0';
            return r;
        } else {
            char* tmp2 = (char*)malloc(strlen(r));
            memcpy(tmp2, r, strlen(r)); tmp2[strlen(r)-2] = ')'; tmp2[strlen(r)-1] = '\0';;
            free(r);
            return tmp2;
        }
        }
    case M_FUNCTION:
        r = (char*)malloc(strlen("<Function>")+1);
        strcpy(r, "<Function>");
        return r;
    case M_POINTER:
        {
        int size = snprintf(r, 0, "<Pointer %li>", (unsigned long)(v->ptr));
        r = (char*)malloc(size+1);
        sprintf(r, "<Pointer %li>", (unsigned long)(v->ptr));
        return r;
        }
    case M_SYNTAX_TREE:
        r = (char*)malloc(strlen("<Syntax Tree>")+1);
        strcpy(r, "<Syntax Tree>");
        return r;
    }
    return "";
}

void Minimal_Value_getReferences(struct CyclicGarbageList* list, MinimalValue v) {
    if(v == NULL) { return; }

    if(v->typeobj != NULL) {
        Minimal_addToCyclicGarbageList(list, v->typeobj);
    }

    switch(v->type) {
    case M_NIL:
    case M_BOOLEAN:
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
    case M_FLOAT:
    case M_DOUBLE:
    case M_STRING:
    case M_TSREF:
        break;
    case M_FUNCTION:
        Minimal_addToCyclicGarbageList(list, (MinimalValue)v->layer);
        Minimal_addToCyclicGarbageList(list, (MinimalValue)v->parameter_list);
        Minimal_addToCyclicGarbageList(list, (MinimalValue)v->code);
        break;
    case M_TYPE:
        Minimal_addToCyclicGarbageList(list, (MinimalObject)v->typemap);
        Minimal_addToCyclicGarbageList(list, (MinimalObject)v->type_spec);
        break;
    case M_TUPLE:
        {
        int i;
        for(i = 0; i < v->size; i++) {
            Minimal_addToCyclicGarbageList(list, v->values[i]);
        }
        break;
        }
    case M_POINTER:
        Minimal_addToCyclicGarbageList(list, v->ptr);
        break;
    case M_SYNTAX_TREE:
        Minimal_addToCyclicGarbageList(list, (MinimalValue)v->syntax_tree);
        break;
    }
}

MinimalValue Minimal_copy_internal(MinimalValue v, MinimalValue** memo);
MinimalValue Minimal_copy(MinimalValue v) {
    MinimalValue r;
    MinimalValue* memo = malloc(sizeof(void*));
    memo[0] = NULL;

    r = Minimal_copy_internal(v, &memo);
    free(memo);
    return r;
}

MinimalValue Minimal_copy_internal(MinimalValue v, MinimalValue** memo) {
    int memosize;
    MinimalValue nv;
    MinimalValue* newmemo;
    if(v == NULL) { return NULL; }

    for(memosize=0; (*memo)[memosize*2] != NULL; memosize++) {
        if((*memo)[memosize*2] == v) {
            Minimal_addReference((*memo)[memosize*2+1]);
            return (*memo)[memosize*2+1];
        }
    }

    nv = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);

    newmemo = malloc(sizeof(void*)*(memosize*2+3));
    memcpy(newmemo, (*memo), sizeof(void*)*memosize*2);
    newmemo[memosize*2] = v;
    newmemo[memosize*2+1] = nv;
    newmemo[memosize*2+2] = NULL;
    free((*memo));
    *memo = newmemo;

    nv->type = v->type;
    if(v->typeobj != NULL) {
        Minimal_addReference(v->typeobj)
    }
    nv->typeobj = v->typeobj;
    nv->sum_pos = v->sum_pos;

    switch(v->type) {
    case M_NIL:
        break;
    case M_BOOLEAN:
        nv->boolean = v->boolean;
        break;
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
        nv->integer = v->integer;
        break;
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
        nv->uinteger = v->uinteger;
        break;
    case M_FLOAT:
        nv->singlefloat = v->singlefloat;
        break;
    case M_DOUBLE:
        nv->doublefloat = v->doublefloat;
        break;
    case M_STRING:
        nv->string = (char*)malloc(v->length);
        memcpy(nv->string, v->string, v->length);
        nv->length = v->length;
        break;
    case M_TYPE:
        if(v->type_name != NULL) {
            nv->type_name = (char*)malloc(strlen(v->type_name) + 1);
            strcpy(nv->type_name, v->type_name);
        } else {
            nv->type_name = NULL;
        }
        Minimal_addReference(v->type_spec);
        nv->type_spec = v->type_spec;
        if(v->typemap != NULL) {
            Minimal_addReference(v->typemap);
        }
        nv->typemap = v->typemap;
        if(v->type_id == NULL) {
            nv->type_id = NULL;
        } else {
            nv->type_id = malloc(strlen(v->type_id) + 1);
            strcpy(nv->type_id, v->type_id);
        }
        break;
    case M_TSREF:
        Minimal_linda_ts_addref_func(v);
        nv->string = (char*)malloc(strlen(v->string) + 1);
        strcpy(nv->string, v->string);
        break;
    case M_TUPLE:
        {
        int i;
        nv->size = v->size;
        nv->values = malloc(sizeof(void*)*v->size);
        for(i=0; i<v->size; i++) {
            nv->values[i] = Minimal_copy_internal(v->values[i], memo);
        }
        break;
        }
    case M_FUNCTION:
        nv->func_name = (char*)malloc(strlen(v->func_name) + 1);
        strcpy(nv->func_name, v->func_name);

        Minimal_addReference(v->parameter_list);
        nv->parameter_list = v->parameter_list;
        if(nv->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL when creating function.\n"); }
        Minimal_addReference(v->code);
        nv->code = v->code;
        nv->layer = v->layer;
        Minimal_addReference(nv->layer);
        break;
    case M_POINTER:
        nv->ptr = Minimal_copy_internal(v->ptr, memo);
        break;
    default:
        fprintf(stderr, "Unknown value type in Minimal_copy (%i)\n", v->type);
        Minimal_delReference(nv);
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
    case M_NIL:
        break;
    case M_BOOLEAN:
        break;
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
        break;
    case M_FLOAT:
    case M_DOUBLE:
        break;
    case M_STRING:
        free(v->string);
        break;
    case M_TYPE:
        free(v->type_name);
        if(v->type_spec != NULL) {
            Minimal_delReference(v->type_spec);
        }
        if(v->typemap != NULL) {
            Minimal_delReference(v->typemap);
        }
        if(v->type_id != NULL) {
            free(v->type_id);
        }
        break;
    case M_TSREF:
        Minimal_linda_ts_delref_func(v);
        free(v->string);
        break;
    case M_TUPLE:
        {
        int i;
        for(i=0; i<v->size; i++) {
            if(v->values[i] != NULL) {
                Minimal_delReference(v->values[i]);
            }
        }
        free(v->values);
        break;
        }
    case M_FUNCTION:
        free(v->func_name);
        Minimal_delReference(v->parameter_list);
        Minimal_delReference(v->code);
        Minimal_delReference(v->layer);
        break;
    case M_POINTER:
        Minimal_delReference(v->ptr);
        break;
    case M_SYNTAX_TREE:
        Minimal_delReference(v->syntax_tree);
        break;
    }
}
