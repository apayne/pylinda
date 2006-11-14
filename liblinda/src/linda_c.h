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

#include "minimal.h"

#ifndef __LINDA_C_H__
#define __LINDA_C_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef MinimalValue LindaValue;

extern LindaValue Linda_typeType;
extern LindaValue Linda_boolType;
extern LindaValue Linda_intType;
extern LindaValue Linda_floatType;
extern LindaValue Linda_stringType;
extern LindaValue Linda_tupleSpaceType;

static inline void Linda_setType(LindaValue value, LindaValue type) { Minimal_setType(value, type); }
static inline LindaValue Linda_getType(LindaValue value) { return Minimal_getType(value); }

static inline unsigned char Linda_isNil(LindaValue v) { return Minimal_isNil(v); }
static inline LindaValue Linda_nil() { return Minimal_nil(); }

static inline unsigned char Linda_isBool(LindaValue v) { return Minimal_isBool(v); }
static inline LindaValue Linda_bool(unsigned char b) { LindaValue v = Minimal_bool(b); Linda_setType(v, Linda_boolType); return v; }
static inline unsigned char Linda_getBool(LindaValue v) { return Minimal_getBool(v); }

static inline unsigned char Linda_isInt(LindaValue v) { return Minimal_isInt(v); }
static inline LindaValue Linda_int(int i) { LindaValue v = Minimal_int(i); Linda_setType(v, Linda_intType); return v; }
static inline int Linda_getInt(LindaValue v) { return Minimal_getInt(v); }

static inline unsigned char Linda_isFloat(LindaValue v) { return Minimal_isFloat(v); }
static inline LindaValue Linda_float(float f) { LindaValue v = Minimal_float(f); Linda_setType(v, Linda_floatType); return v; }
static inline float Linda_getFloat(LindaValue v) { return Minimal_getFloat(v); }

static inline unsigned char Linda_isType(LindaValue v) { return Minimal_isType(v); }
static inline LindaValue Linda_type(const char* typespec) { LindaValue v = Minimal_type(typespec); if(Linda_typeType != NULL) { Linda_setType(v, Linda_typeType); } return v; }

static inline unsigned char Linda_isString(LindaValue v) { return Minimal_isString(v); }
static inline LindaValue Linda_string(char* s) { LindaValue v = Minimal_string(s); Linda_setType(v, Linda_stringType); return v; }
static inline LindaValue Linda_string2(char* s, int len) { LindaValue v = Minimal_string2(s, len); Linda_setType(v, Linda_stringType); return v; }
static inline char* Linda_getString(LindaValue v) { return Minimal_getString(v); }
static inline int Linda_getStringLen(LindaValue v) { return Minimal_getStringLen(v); }

static inline unsigned char Linda_isTupleSpace(LindaValue v) { return Minimal_isTupleSpace(v); }
static inline LindaValue Linda_tupleSpace(const char* ts) { LindaValue v = Minimal_tupleSpace(ts); Linda_setType(v, Linda_tupleSpaceType); return v; }
static inline char* Linda_getTupleSpace(LindaValue v) { return Minimal_getTupleSpace(v); }

static inline unsigned char Linda_isTuple(LindaValue v) { return Minimal_isTuple(v); }
static inline LindaValue Linda_tuple(int i) { return Minimal_tuple(i); }
static inline int Linda_getTupleSize(LindaValue v) { return Minimal_getTupleSize(v); }
static inline void Linda_tupleAdd(LindaValue t, LindaValue v) { Minimal_tupleAdd(t, v); }
static inline void Linda_tupleSet(LindaValue t, int i, LindaValue v) { Minimal_tupleSet(t, i, v); }
static inline LindaValue Linda_tupleGet(LindaValue t, int i) { return Minimal_tupleGet(t, i); }

static inline LindaValue Linda_copy(LindaValue v) { return Minimal_copy(v); }
#define Linda_addReference(obj) Linda_addReference2(obj, __FILE__, __LINE__);
static inline void Linda_addReference2(LindaValue v, char* file, int line) { Minimal_addReference2(v, file, line); }
static inline int Linda_getReferenceCount(LindaValue v) { return Minimal_getReferenceCount(v); }
#define Linda_delReference(ptr) Linda_delReference2(ptr, __FILE__, __LINE__);
static inline void Linda_delReference2(LindaValue v, char* file, int line) { Minimal_delReference2(v, file, line); }

extern char* version;
extern char* process_id;

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef LINDA_SERVER
void Linda_init();
unsigned char Linda_connect(int port);
extern int Linda_port;
void Linda_disconnect();
#endif

LindaValue Linda_createTuplespace();

void Linda_addTSReference(LindaValue ts);
void Linda_delTSReference(LindaValue ts);

void Linda_out(LindaValue ts, LindaValue t);
LindaValue Linda_in(LindaValue ts, LindaValue t);
LindaValue Linda_rd(LindaValue ts, LindaValue t);
LindaValue Linda_inp(LindaValue ts, LindaValue t);
LindaValue Linda_rdp(LindaValue ts, LindaValue t);

int Linda_collect(LindaValue ts1, LindaValue ts2, LindaValue t);
int Linda_copy_collect(LindaValue ts1, LindaValue ts2, LindaValue t);

extern LindaValue Linda_uts;

#ifdef __cplusplus
}
#endif

#endif
