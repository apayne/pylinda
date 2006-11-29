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

#include "../../liblinda/src/linda_internal.h"

MinimalValue Minimal_Nil;

unsigned char Minimal_isNil(MinimalValue v) {
    return v->type == NIL;
}

MinimalValue Minimal_nil() {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = NIL;
    v->typeobj = NULL;
    Minimal_setType(v, Minimal_nilType);
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
    Minimal_setType(v, Minimal_boolType);
    v->sum_pos = -1;
    return v;
}

unsigned char Minimal_getBool(MinimalValue v) {
    return v->boolean;
}

unsigned char Minimal_isByte(MinimalValue v) {
    return v->type == BYTE;
}
unsigned char Minimal_isShort(MinimalValue v) {
    return v->type == SHORT;
}
unsigned char Minimal_isInt(MinimalValue v) {
    return v->type == INTEGER;
}
unsigned char Minimal_isLong(MinimalValue v) {
    return v->type == LONG;
}
unsigned char Minimal_isUByte(MinimalValue v) {
    return v->type == UBYTE;
}
unsigned char Minimal_isUShort(MinimalValue v) {
    return v->type == USHORT;
}
unsigned char Minimal_isUInt(MinimalValue v) {
    return v->type == UINTEGER;
}
unsigned char Minimal_isULong(MinimalValue v) {
    return v->type == ULONG;
}

MinimalValue Minimal_byte(char i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = BYTE;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_byteType);
    return v;
}
MinimalValue Minimal_short(short i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = SHORT;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_shortType);
    return v;
}
MinimalValue Minimal_int(int i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = INTEGER;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_intType);
    return v;
}
MinimalValue Minimal_long(long i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = LONG;
    v->integer = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_longType);
    return v;
}

MinimalValue Minimal_ubyte(unsigned char i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = BYTE;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_ubyteType);
    return v;
}
MinimalValue Minimal_ushort(unsigned short i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = SHORT;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_ushortType);
    return v;
}
MinimalValue Minimal_uint(unsigned int i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = INTEGER;
    v->uinteger = i;
    v->typeobj = NULL;
    v->sum_pos = -1;
    Minimal_setType(v, Minimal_uintType);
    return v;
}
MinimalValue Minimal_ulong(unsigned long i) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = LONG;
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
    return v->type == FLOAT;
}

MinimalValue Minimal_float(float f) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = FLOAT;
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
    return v->type == DOUBLE;
}

MinimalValue Minimal_double(double f) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = DOUBLE;
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
    v->typemap = Minimal_getCurrentLayer();
    v->sum_pos = -1;
    if(Minimal_typeType != NULL) {
        Minimal_setType(v, Minimal_typeType);
    }
    return v;
}

MinimalValue Minimal_function(char* code) {
    Minimal_SyntaxTree* tree = Minimal_parseCode(code);
    Minimal_SyntaxTree* tree2;
    if(tree == NULL) {
        return NULL;
    }
    Minimal_Layer_addTree(Minimal_defaultLayer, tree);

    tree2 = tree;
    while(tree2->type != ST_TYPE_SPEC && tree2->type != ST_FUNCTION_DEF) {
        switch(tree2->type) {
        case ST_SEQENTIAL_DEFS:
            tree2 = tree2->branch1;
            break;
        default:
            fprintf(stderr, "Error: Tried to parse function but didn't get a function back - got %i.\n", tree2->type);
            return NULL;
        }
    }

    MinimalValue f;
    MinimalLayer layer = Minimal_getCurrentLayer();
    if(tree2->type == ST_TYPE_SPEC) {
        f = Minimal_getName(layer, tree2->type_name);
    } else {
        f = Minimal_getName(layer, tree2->func_name);
    }
    Minimal_delReference(layer);
    Minimal_SyntaxTree_free(tree);
    return f;
}

MinimalValue Minimal_function2(char* func_name, Minimal_SyntaxTree* func_type, Minimal_SyntaxTree* parameter_list, Minimal_SyntaxTree* code) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = FUNCTION;
    v->func_name = (char*)malloc(strlen(func_name)+1);
    strcpy(v->func_name, func_name);
    v->parameter_list = Minimal_SyntaxTree_copy(parameter_list);
    if(v->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL.\n"); }
    v->layer = Minimal_getCurrentLayer();
    v->code = Minimal_SyntaxTree_copy(code);
    if(func_type != NULL) {
        v->typeobj = Minimal_typeSpec(v->func_name, func_type);
    } else {
        v->typeobj = NULL;
    }
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
    MinimalLayer layer = Minimal_getCurrentLayer();
    Minimal_Layer_addTree(layer, tree);
    MinimalValue v = Minimal_getName(layer, tree->type_name);
    Minimal_SyntaxTree_free(tree);
    Minimal_delReference(layer);

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

MinimalValue Minimal_ptr(MinimalValue value) {
    MinimalValue v = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    v->type = POINTER;
    if(value != NULL) {
        Minimal_addReference(value);
    }
    if(value->typeobj == NULL) {
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

MinimalValue Minimal_getPtrType(MinimalValue v) {
    return v->ptr->typeobj;
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

void Minimal_setTypeMap(MinimalValue v, MinimalLayer types) {
    if(v->type == TYPE) {
        Minimal_addReference(types);
        v->typemap = types;
    }
    if(v->typeobj != NULL) {
        Minimal_addReference(types);
        v->typeobj->typemap = types;
    }
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
    case BYTE:
    case SHORT:
    case INTEGER:
    case LONG:
        {
        int size = snprintf(r, 0, "%li", v->integer);
        r = (char*)malloc(size + 1);
        sprintf(r, "%li", v->integer);
        return r;
        }
    case UBYTE:
    case USHORT:
    case UINTEGER:
    case ULONG:
        {
        int size = snprintf(r, 0, "%li", v->uinteger);
        r = (char*)malloc(size + 1);
        sprintf(r, "%li", v->uinteger);
        return r;
        }
    case FLOAT:
        {
        int size = snprintf(r, 0, "%f", v->singlefloat);
        r = (char*)malloc(size + 1);
        sprintf(r, "%f", v->singlefloat);
        return r;
        }
    case DOUBLE:
        {
        int size = snprintf(r, 0, "%f", v->doublefloat);
        r = (char*)malloc(size + 1);
        sprintf(r, "%f", v->doublefloat);
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
        {
        int size = snprintf(r, 0, "<Type %s>", v->type_name);
        r = (char*)malloc(size + 1);
        sprintf(r, "<Type %s>", v->type_name);
        return r;
        }
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
        if(strlen(r) == 1) {
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

MinimalObject* Minimal_Value_getReferences(MinimalValue v) {
    if(v == NULL) { return NULL; }

    switch(v->type) {
    case NIL:
    case BOOLEAN:
    case BYTE:
    case SHORT:
    case INTEGER:
    case LONG:
    case UBYTE:
    case USHORT:
    case UINTEGER:
    case ULONG:
    case FLOAT:
    case DOUBLE:
    case STRING:
    case TSREF:
        if(v->typeobj != NULL) {
            MinimalObject* list = malloc(sizeof(void*)*2);
            list[0] = v->typeobj;
            list[1] = NULL;
            return list;
        } else {
            MinimalObject* list = malloc(sizeof(void*));
            list[0] = NULL;
            return list;
        }
    case FUNCTION:
        if(v->typeobj != NULL) {
            MinimalObject* list = malloc(sizeof(void*)*3);
            list[0] = v->layer;
            list[1] = v->typeobj;
            list[2] = NULL;
            return list;
        } else {
            MinimalObject* list = malloc(sizeof(void*)*2);
            list[0] = v->layer;
            list[1] = NULL;
            return list;
        }
    case TYPE:
        if(v->typeobj != NULL) {
            MinimalObject* list = malloc(sizeof(void*)*3);
            list[0] = v->typemap;
            list[1] = v->typeobj;
            list[2] = NULL;
            return list;
        } else {
            MinimalObject* list = malloc(sizeof(void*)*2);
            list[0] = v->typemap;
            list[1] = NULL;
            return list;
        }
    case TUPLE:
        if(v->typeobj != NULL) {
            int i;
            MinimalObject* list = malloc(sizeof(void*)*(3+v->size));
            list[0] = v->typeobj;
            for(i=1; i<=v->size; i++) {
                list[i] = v->values[i-1];
            }
            list[2] = NULL;
            return list;
        } else {
            int i;
            MinimalObject* list = malloc(sizeof(void*)*(2+v->size));
            for(i=0; i<=v->size; i++) {
                list[i] = v->values[i];
            }
            list[1] = NULL;
            return list;
        }
    case POINTER:
        if(v->typeobj != NULL) {
            MinimalObject* list = malloc(sizeof(void*)*3);
            list[0] = v->ptr;
            list[1] = v->typeobj;
            list[2] = NULL;
            return list;
        } else {
            MinimalObject* list = malloc(sizeof(void*)*2);
            list[0] = v->ptr;
            list[1] = NULL;
            return list;
        }
    default:
        fprintf(stderr, "Unknown value type in Minimal_Value_getReferences (%i)\n", v->type);
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
    case BYTE:
    case SHORT:
    case INTEGER:
    case LONG:
        nv->integer = v->integer;
        break;
    case UBYTE:
    case USHORT:
    case UINTEGER:
    case ULONG:
        nv->uinteger = v->uinteger;
        break;
    case FLOAT:
        nv->singlefloat = v->singlefloat;
        break;
    case DOUBLE:
        nv->doublefloat = v->doublefloat;
        break;
    case STRING:
        nv->string = (char*)malloc(v->length);
        memcpy(nv->string, v->string, v->length);
        nv->length = v->length;
        break;
    case TYPE:
        nv->type_name = (char*)malloc(strlen(v->type_name) + 1);
        strcpy(nv->type_name, v->type_name);
        nv->type_spec = Minimal_SyntaxTree_copy(v->type_spec);
        if(v->typemap != NULL) {
            Minimal_addReference(v->typemap);
        }
        nv->typemap = v->typemap;
        break;
    case TSREF:
        Linda_addTSReference(v);
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

        nv->parameter_list = Minimal_SyntaxTree_copy(v->parameter_list);
        if(nv->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL when creating function.\n"); }
        nv->code = Minimal_SyntaxTree_copy(v->code);
        nv->layer = v->layer;
        Minimal_addReference(nv->layer);
        break;
    case POINTER:
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
    case BYTE:
    case SHORT:
    case INTEGER:
    case LONG:
    case UBYTE:
    case USHORT:
    case UINTEGER:
    case ULONG:
        break;
    case FLOAT:
        break;
    case STRING:
        free(v->string);
        break;
    case TYPE:
        free(v->type_name);
        Minimal_SyntaxTree_free(v->type_spec);
        if(v->typemap != NULL) {
            Minimal_delReference(v->typemap);
        }
        break;
    case TSREF:
        Linda_delTSReference(v);
        free(v->string);
        break;
    case TUPLE:
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
    case FUNCTION:
        free(v->func_name);
        Minimal_SyntaxTree_free(v->parameter_list);
        Minimal_SyntaxTree_free(v->code);
        Minimal_delReference(v->layer);
        break;
    case POINTER:
        Minimal_delReference(v->ptr);
        break;
    default:
        fprintf(stderr, "Unknown value type in Minimal_Value_free (%i)\n", v->type);
        return;
    }
    free(v);
}
