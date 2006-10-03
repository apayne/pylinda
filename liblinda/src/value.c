/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of LibLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    LibLinda is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    LibLinda is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with LibLinda; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linda.h"

Value Value_new() {
    Value v;
    v.type = NIL;
    return v;
}

Value Value_copy(Value* v) {
    Value nv;
    nv.type = v->type;
    switch(v->type) {
    case NIL:
        break;
    case BOOLEAN:
        nv.boolean = v->boolean;
        break;
    case INTEGER:
        nv.integer = v->integer;
        break;
    case FLOAT:
        nv.floating = v->floating;
        break;
    case STRING:
        nv.string = (char*)malloc(v->length+1);
        memcpy(nv.string, v->string, v->length+1);
        nv.length = v->length;
        break;
    case TYPE:
        nv.typespec = (char*)malloc(strlen(v->typespec)+1);
        strcpy(nv.typespec, v->typespec);
    case TSREF:
        nv.tsid = (char*)malloc(strlen(v->tsid)+1);
        strcpy(nv.tsid, v->tsid);
        break;
    case TUPLE:
        nv.tuple = Tuple_copy(v->tuple);
        break;
    default:
        fprintf(stderr, "Invalid type (%i) when copying value.\n", v->type);
    }
    return nv;
}

Value* Value_copyptr(Value* v) {
    Value* nv = (Value*)malloc(sizeof(Value));
    *nv = Value_copy(v);
    return nv;
}

void Value_clear(Value* v) {
    if(v->type == STRING) {
        free(v->string);
    } else if(v->type == TSREF) {
        free(v->tsid);
    } else if(v->type == TUPLE) {
        free(v->tuple);
    }
    v->type = NIL;
}
void Value_free(Value* v) {
    Value_clear(v);
    free(v);
}

unsigned char Value_is_nil(Value* v) {
    return v->type == NIL;
}

Value Value_nil() {
    Value v;
    v.type = NIL;
    return v;
}

unsigned char Value_is_bool(Value* v) {
    return v->type == BOOLEAN;
}

Value Value_bool(unsigned char b) {
    Value v;
    v.type = BOOLEAN;
    v.boolean = b;
    return v;
}

void Value_set_bool(Value* v, unsigned char b) {
    Value_clear(v);
    v->type = BOOLEAN;
    v->boolean = b;
}

unsigned char Value_get_bool(Value* v) {
    assert(v->type == BOOLEAN);
    return v->boolean;
}

Value Value_int(int i) {
    Value v;
    v.type = INTEGER;
    v.integer = i;
    return v;
}

long Value_is_int(Value* v) {
    return v->type == INTEGER;
}

void Value_set_int(Value* v, int i) {
    Value_clear(v);
    v->type = INTEGER;
    v->integer = i;
}

long Value_get_int(Value* v) {
    assert(v->type == INTEGER);
    return v->integer;
}

unsigned char Value_is_float(Value* v) {
    return v->type == FLOAT;
}

Value Value_float(float f) {
    Value v;
    v.type = FLOAT;
    v.floating = f;
    return v;
}

float Value_get_float(Value* v) {
    return v->floating;
}

unsigned char Value_is_string(Value* v) {
    return v->type == STRING;
}

Value Value_string(char* s) {
    Value v;
    v.type = STRING;
    v.string = (char*)malloc(strlen(s) + 1);
    strcpy(v.string, s);
    v.length = strlen(s);
    return v;
}

char* Value_get_string(Value* v) {
    return v->string;
}

int Value_get_string_len(Value* v) {
    return v->length;
}

unsigned char Value_is_type(Value* v) {
    return v->type == TYPE;
}

Value Value_type(char* t) {
    Value v;
    v.type = TYPE;
    v.typespec = (char*)malloc(strlen(t) + 1);
    strcpy(v.typespec, t);
    return v;
}

char* Value_get_type(Value* v) {
    return v->typespec;
}

unsigned char Value_is_tsref(Value* v) {
    return v->type == TSREF;
}

Value Value_tsref(char* tsid) {
    Value v;
    v.type = TSREF;
    v.tsid = (char*)malloc(strlen(tsid)+1);
    strcpy(v.tsid, tsid);
    return v;
}

char* Value_get_tsref(Value* v) {
    return v->tsid;
}

unsigned char Value_is_tuple(Value* v) {
    return v->type == TUPLE;
}

Value Value_tuple(Tuple t) {
    Value v;
    v.type = TUPLE;
    v.tuple = Tuple_copy(t);
    return v;
}

Tuple Value_get_tuple(Value* v) {
    return v->tuple;
}

Value Value_boolType = {TYPE, {.typespec = "bool"}};
Value Value_intType = {TYPE, {.typespec = "int"}};
Value Value_floatType = {TYPE, {.typespec = "float"}};
Value Value_stringType = {TYPE, {.typespec = "string"}};
Value Value_tsType = {TYPE, {.typespec = "ts"}};
Value Value_tupleType = {TYPE, {.typespec = "tuple"}};
