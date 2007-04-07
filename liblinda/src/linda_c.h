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

#include <stdlib.h>

#ifndef __LINDA_C_H__
#define __LINDA_C_H__

#include "minimal.h"

#ifdef WIN32
#define inline
#ifdef LINDA_SERVER
#define EXPORT
#define IMPORT __declspec(dllimport)
#ifdef LINDA_INTERNAL
#define LSEXPORT __declspec(dllexport)
#define LSIMPORT __declspec(dllexport)
#else
#define LSEXPORT
#define LSIMPORT __declspec(dllimport)
#endif
#else
#ifdef LINDA_INTERNAL
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllexport)
#else
#define EXPORT
#define IMPORT __declspec(dllimport)
#endif
#endif
#else
#define EXPORT
#define LSEXPORT
#define IMPORT
#define LSIMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef MinimalValue LindaValue;

#define Linda_typeType Minimal_typeType
#define Linda_nilType Minimal_nilType
#define Linda_boolType Minimal_boolType
#ifdef TYPES
#define Linda_byteType Minimal_byteType
#define Linda_shortType Minimal_shortType
#endif
#define Linda_intType Minimal_intType
#define Linda_longType Minimal_longType
#ifdef TYPES
#define Linda_ubyteType Minimal_ubyteType
#define Linda_ushortType Minimal_ushortType
#define Linda_uintType Minimal_uintType
#define Linda_ulongType Minimal_ulongType
#endif
#define Linda_floatType Minimal_floatType
#define Linda_stringType Minimal_stringType
#define Linda_tupleSpaceType Minimal_tupleSpaceType

#ifdef TYPES
#ifdef LINDA_SERVER
LSIMPORT extern unsigned char Linda_register_types;
#else
IMPORT extern unsigned char Linda_register_types;
#endif
#endif

#ifdef TYPES
static inline void Linda_setType(LindaValue value, LindaValue type) { Minimal_setType(value, type); }
#endif
static inline LindaValue Linda_getType(LindaValue value) { return Minimal_getType(value); }

static inline void Linda_setSumPos(LindaValue value, int i) { Minimal_setSumPos(value, i); }
static inline int Linda_getSumPos(LindaValue value) { return Minimal_getSumPos(value); }

static inline unsigned char Linda_isNil(LindaValue v) { return Minimal_isNil(v); }
static inline LindaValue Linda_nil() { return Minimal_nil(); }

static inline unsigned char Linda_isBool(LindaValue v) { return Minimal_isBool(v); }
static inline LindaValue Linda_bool(unsigned char b) { return Minimal_bool(b); }
static inline unsigned char Linda_getBool(LindaValue v) { return Minimal_getBool(v); }

static inline unsigned char Linda_isInt(LindaValue v) { return Minimal_isInt(v); }
static inline LindaValue Linda_int(int i) { return Minimal_int(i); }
static inline int Linda_getInt(LindaValue v) { return Minimal_getInt(v); }

static inline unsigned char Linda_isLong(LindaValue v) { return Minimal_isLong(v); }
static inline LindaValue Linda_long(long i) { return Minimal_long(i); }
static inline long Linda_getLong(LindaValue v) { return Minimal_getLong(v); }

static inline unsigned char Linda_isFloat(LindaValue v) { return Minimal_isFloat(v); }
static inline LindaValue Linda_float(float f) { return Minimal_float(f); }
static inline float Linda_getFloat(LindaValue v) { return Minimal_getFloat(v); }

static inline unsigned char Linda_isType(LindaValue v) { return Minimal_isType(v); }

#ifdef TYPES
#ifdef REGISTER_TYPES
EXPORT void Linda_registerType(LindaValue v);
#endif

static inline LindaValue Linda_type(const char* typespec) { return Minimal_type(typespec); }
#endif

static inline unsigned char Linda_isString(LindaValue v) { return Minimal_isString(v); }
static inline LindaValue Linda_string(char* s) { return Minimal_string(s); }
static inline LindaValue Linda_string2(char* s, int len) { return Minimal_string2(s, len); }
static inline char* Linda_getString(LindaValue v) { return Minimal_getString(v); }
static inline int Linda_getStringLen(LindaValue v) { return Minimal_getStringLen(v); }

static inline unsigned char Linda_isTupleSpace(LindaValue v) { return Minimal_isTupleSpace(v); }
static inline char* Linda_getTupleSpace(LindaValue v) { return Minimal_getTupleSpace(v); }

static inline unsigned char Linda_isTuple(LindaValue v) { return Minimal_isTuple(v); }
static inline LindaValue Linda_tuple(int i) { return Minimal_tuple(i); }
static inline int Linda_getTupleSize(LindaValue v) { return Minimal_getTupleSize(v); }
static inline void Linda_tupleAdd(LindaValue t, LindaValue v) { Minimal_tupleAdd(t, v); }
static inline void Linda_tupleSet(LindaValue t, int i, LindaValue v) { Minimal_tupleSet(t, i, v); }
static inline LindaValue Linda_tupleGet(LindaValue t, int i) { return Minimal_tupleGet(t, i); }

static inline unsigned char Linda_isProduct(LindaValue v) { return Minimal_isTuple(v); }
static inline LindaValue Linda_product(int i) { return Minimal_tuple(i); }
static inline int Linda_getProductSize(LindaValue v) { return Minimal_getTupleSize(v); }
static inline void Linda_addProduct(LindaValue t, LindaValue v) { Minimal_tupleAdd(t, v); }
static inline void Linda_setProduct(LindaValue t, int i, LindaValue v) { Minimal_tupleSet(t, i, v); }
static inline LindaValue Linda_getProduct(LindaValue t, int i) { return Minimal_tupleGet(t, i); }

static inline unsigned char Linda_isFunction(LindaValue v) { return Minimal_isFunction(v); }
static inline LindaValue Linda_function(char* code) { return Minimal_function(code); }

static inline unsigned char Linda_isPtr(LindaValue v) { return Minimal_isPtr(v); }
static inline LindaValue Linda_ptr(LindaValue v) { return Minimal_ptr(v); }
static inline LindaValue Linda_getPtr(LindaValue v) { return Minimal_getPtr(v); }
static inline void Linda_setPtr(LindaValue v, LindaValue v2) { return Minimal_setPtr(v, v2); }

static inline LindaValue Linda_copy(LindaValue v) { return Minimal_copy(v); }

#define Linda_addReference(obj) Linda_addReference2(obj, __FILE__, __LINE__);
static inline void Linda_addReference2(LindaValue v, char* file, int line) { Minimal_addReference2(v, file, line); }
static inline int Linda_getReferenceCount(LindaValue v) { return Minimal_getReferenceCount(v); }
#define Linda_delReference(ptr) Linda_delReference2(ptr, __FILE__, __LINE__);
static inline void Linda_delReference2(LindaValue v, char* file, int line) { Minimal_delReference2(v, file, line); }

static inline LindaValue Linda_apply(LindaValue func, LindaValue args) { return Minimal_apply(func, args); }

#ifdef LINDA_SERVER
LSIMPORT extern unsigned char Linda_is_server;
LSIMPORT extern char* Linda_process_id;
LSIMPORT extern LindaValue Linda_uts;
#else
IMPORT extern unsigned char Linda_is_server;
IMPORT extern char* Linda_process_id;
IMPORT extern LindaValue Linda_uts;
#endif

IMPORT extern char* Linda_version;

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef LINDA_SERVER
EXPORT void Linda_init();
EXPORT unsigned char Linda_connect(int port);
IMPORT extern int Linda_port;
EXPORT void Linda_disconnect();
#endif

EXPORT LindaValue Linda_createTuplespace();

EXPORT void Linda_out(LindaValue ts, LindaValue t);
EXPORT LindaValue Linda_in(LindaValue ts, LindaValue t);
EXPORT LindaValue Linda_rd(LindaValue ts, LindaValue t);
EXPORT LindaValue Linda_inp(LindaValue ts, LindaValue t);
EXPORT LindaValue Linda_rdp(LindaValue ts, LindaValue t);

EXPORT int Linda_collect(LindaValue ts1, LindaValue ts2, LindaValue t);
EXPORT int Linda_copy_collect(LindaValue ts1, LindaValue ts2, LindaValue t);

struct Linda_thread_data_t {
    int sd;
    char* thread_id;
};
typedef struct Linda_thread_data_t Linda_thread_data;
EXPORT Linda_thread_data* Linda_get_thread_data();

#ifdef __cplusplus
}
#endif

#endif
