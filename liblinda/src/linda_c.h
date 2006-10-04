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

#ifndef __LINDA_C_H__
#define __LINDA_C_H__

#ifdef __cplusplus
extern "C" {
#endif

extern char* version;
extern char* process_id;
extern char* thread_id;

#ifndef NULL
#define NULL (void*)0
#endif

struct Tuple_t;
struct Value_t;

typedef struct Tuple_t* Tuple;

typedef struct Value_t Value;

struct Value_t {
    enum {
        NIL,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        TYPE,
        TSREF,
        TUPLE
    } type;
    union {
        unsigned char boolean;
        long integer;
        float floating;
        struct {
            char* string;
            int length;
        };
        char* typespec;
        char* tsid;
        Tuple tuple;
    };
};

struct Tuple_t {
    Value* values;
    int size;
};

Value Value_new();
Value Value_copy(Value* v);
Value* Value_copyptr(Value* v);
void Value_clear(Value* v);
void Value_free(Value* v);

unsigned char Value_is_nil(Value* v);
Value Value_nil();

unsigned char Value_is_bool(Value* v);
Value Value_bool(unsigned char b);
void Value_set_bool(Value* v, unsigned char b);
unsigned char Value_get_bool(Value* v);

long Value_is_int(Value* v);
Value Value_int(int i);
void Value_set_int(Value* v, int i);
long Value_get_int(Value* v);

unsigned char Value_is_float(Value* v);
Value Value_float(float f);
void Value_set_float(Value* v, float f);
float Value_get_float(Value* v);

unsigned char Value_is_string(Value* v);
Value Value_string(char* s);
Value Value_string_len(char* s, int len);
void Value_set_string_null(Value* v, char* s);
void Value_set_string_len(Value* v, char* s, int len);
char* Value_get_string(Value* v);
int Value_get_string_len(Value* v);

unsigned char Value_is_type(Value* v);
Value Value_type(char* t);
char* Value_get_type(Value* v);

extern Value Value_boolType;
extern Value Value_intType;
extern Value Value_floatType;
extern Value Value_stringType;
extern Value Value_tsType;
extern Value Value_tupleType;
extern Value Value_tuplespaceType;

unsigned char Value_is_tsref(Value* v);
Value Value_tsref(char* tsid);
char* Value_get_tsref(Value* v);

unsigned char Value_is_tuple(Value* v);
Value Value_tuple(Tuple t);
Tuple Value_get_tuple(Value* v);

Tuple Template_new(int size);
Tuple Tuple_new(int size);
void Tuple_delete(Tuple t);
void Tuple_add(Tuple t, Value val);
void Tuple_set(Tuple t, int pos, Value val);
Value* Tuple_get(Tuple t, int pos);
int Tuple_get_size(Tuple t);
Tuple Tuple_copy(Tuple t);
void Tuple_clear(Tuple t);
void Tuple_free(Tuple t);

#ifndef LINDA_SERVER
unsigned char Linda_connect(int port);
extern int Linda_port;
void Linda_disconnect();
#endif

typedef char* Linda_tuplespace;

Linda_tuplespace Linda_createTuplespace();

void Linda_addReference(const Linda_tuplespace ts);
void Linda_deleteReference(const Linda_tuplespace ts);

void Linda_out(const Linda_tuplespace ts, struct Tuple_t* t);
struct Tuple_t* Linda_in(const Linda_tuplespace ts, struct Tuple_t* t);
struct Tuple_t* Linda_rd(const Linda_tuplespace ts, struct Tuple_t* t);
struct Tuple_t* Linda_inp(const Linda_tuplespace ts, struct Tuple_t* t);
struct Tuple_t* Linda_rdp(const Linda_tuplespace ts, struct Tuple_t* t);

int Linda_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, struct Tuple_t* t);
int Linda_copy_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, struct Tuple_t* t);

#ifdef __cplusplus
}
#endif

#endif
