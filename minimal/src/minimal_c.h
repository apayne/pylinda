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

#include "libxml/tree.h"

#ifndef MINIMAL_C_H
#define MINIMAL_C_H

#ifdef __cplusplus
extern "C" {
#endif

void Minimal_init();
void Minimal_finalise();

typedef void* MinimalObject;

struct MinimalValue_t;
typedef struct MinimalValue_t* MinimalValue;

struct Minimal_SyntaxTree_t;
typedef struct Minimal_SyntaxTree_t Minimal_SyntaxTree;

struct MinimalLayer_t;
typedef struct MinimalLayer_t* MinimalLayer;

struct Minimal_NameValueMap_t;
typedef struct Minimal_NameValueMap_t Minimal_NameValueMap;

struct MinimalValue_t {
    enum {
        NIL,
        BOOLEAN,
        BYTE,
        SHORT,
        INTEGER,
        LONG,
        UBYTE,
        USHORT,
        UINTEGER,
        ULONG,
        FLOAT,
        DOUBLE,
        STRING,
        TYPE,
        TSREF,
        TUPLE,
        FUNCTION,
        POINTER
    } type;
    MinimalValue typeobj;
    int sum_pos;
    union {
        unsigned char boolean;
        long integer;
        unsigned long uinteger;
        float singlefloat;
        double doublefloat;
        struct {
            char* string;
            unsigned int length;
        };
        struct {
            char* type_name;
            Minimal_SyntaxTree* type_spec;
            MinimalLayer typemap;
            unsigned long type_id; 
        };
        char* tsid;
        struct {
            MinimalValue* values;
            int size;
        };
        struct {
            char* func_name;
            Minimal_SyntaxTree* parameter_list;
            Minimal_SyntaxTree* code;
            MinimalLayer layer;
        };
        struct {
            MinimalValue ptr;
        };
    };
};

extern MinimalValue Minimal_Nil;

unsigned char Minimal_isNil(MinimalValue v);
MinimalValue Minimal_nil();

unsigned char Minimal_isBool(MinimalValue v);
MinimalValue Minimal_bool(unsigned char b);
unsigned char Minimal_getBool(MinimalValue v);

unsigned char Minimal_isByte(MinimalValue v);
unsigned char Minimal_isShort(MinimalValue v);
unsigned char Minimal_isInt(MinimalValue v);
unsigned char Minimal_isLong(MinimalValue v);
unsigned char Minimal_isInteger(MinimalValue v);
unsigned char Minimal_isUByte(MinimalValue v);
unsigned char Minimal_isUShort(MinimalValue v);
unsigned char Minimal_isUInt(MinimalValue v);
unsigned char Minimal_isULong(MinimalValue v);
MinimalValue Minimal_byte(char i);
MinimalValue Minimal_short(short i);
MinimalValue Minimal_int(int i);
MinimalValue Minimal_long(long i);
MinimalValue Minimal_ubyte(unsigned char i);
MinimalValue Minimal_ushort(unsigned short i);
MinimalValue Minimal_uint(unsigned int i);
MinimalValue Minimal_ulong(unsigned long i);
char Minimal_getByte(MinimalValue v);
short Minimal_getShort(MinimalValue v);
int Minimal_getInt(MinimalValue v);
long Minimal_getLong(MinimalValue v);
unsigned char Minimal_getUByte(MinimalValue v);
unsigned short Minimal_getUShort(MinimalValue v);
unsigned int Minimal_getUInt(MinimalValue v);
unsigned long Minimal_getULong(MinimalValue v);

unsigned char Minimal_isFloat(MinimalValue v);
MinimalValue Minimal_float(float f);
float Minimal_getFloat(MinimalValue v);

unsigned char Minimal_isDouble(MinimalValue v);
MinimalValue Minimal_double(double f);
double Minimal_getDouble(MinimalValue v);

unsigned char Minimal_isString(MinimalValue v);
MinimalValue Minimal_string(char* s);
MinimalValue Minimal_string2(char* s, unsigned int len);
char* Minimal_getString(MinimalValue v);
unsigned int Minimal_getStringLen(MinimalValue v);

unsigned char Minimal_isType(MinimalValue v);
MinimalValue Minimal_type(const char* typespec);
MinimalValue Minimal_typeSpec(const char* type_name, Minimal_SyntaxTree* type_spec);
MinimalValue Minimal_function(char* code);
MinimalValue Minimal_function2(char* func_name, Minimal_SyntaxTree* type_spec, Minimal_SyntaxTree* parameters, Minimal_SyntaxTree* code);

MinimalValue Minimal_copy(MinimalValue v);
char* Minimal_Value_string(MinimalValue v);

unsigned char Minimal_isTuple(MinimalValue v);
MinimalValue Minimal_tuple(int size);
int Minimal_getTupleSize(MinimalValue tuple);
void Minimal_tupleAdd(MinimalValue tuple, MinimalValue value);
void Minimal_tupleSet(MinimalValue tuple, int pos, MinimalValue value);
MinimalValue Minimal_tupleGet(MinimalValue tuple, int pos);

unsigned char Minimal_isPtr(MinimalValue v);
MinimalValue Minimal_ptr(MinimalValue ptr);
MinimalValue Minimal_getPtr(MinimalValue v);
MinimalValue Minimal_getPtrType(MinimalValue v);

unsigned char Minimal_isTupleSpace(MinimalValue v);
MinimalValue Minimal_tupleSpace(const char* ts);
char* Minimal_getTupleSpace(MinimalValue v);

void Minimal_setType(MinimalValue value, MinimalValue type);
MinimalValue Minimal_getType(MinimalValue value);
void Minimal_setSumPos(MinimalValue value, int sum_pos);

struct Minimal_SyntaxTree_t {
    enum {
        ST_BLANK,
        ST_NIL,
        ST_IDENTIFIER,
        ST_INTEGER,
        ST_SEQENTIAL_DEFS,
        ST_TYPE_SPEC,
        ST_TYPE_FUNCTION,
        ST_FUNCTION_DEF,
        ST_PARAMETER_LIST,
        ST_FUNCTION_CALL,
        ST_ARGUMENT_LIST,
        ST_OPERATOR,
        ST_PRODUCT_TYPE,
        ST_SUM_TYPE,
        ST_TUPLE,
        ST_POINTER
    } type;
    union {
        int integer;
        double floating;
        char* string;
        struct {
            struct Minimal_SyntaxTree_t* branch1;
            struct Minimal_SyntaxTree_t* branch2;
        };
        struct {
            char* type_name;
            struct Minimal_SyntaxTree_t* type_def;
        };
        struct {
            char* var_name;
            struct Minimal_SyntaxTree_t* next_var;
        };
        struct {
            char* func_name;
            struct Minimal_SyntaxTree_t* type_spec;
            struct Minimal_SyntaxTree_t* parameter_list;
            struct Minimal_SyntaxTree_t* body;
        };
        struct {
            struct Minimal_SyntaxTree_t* function;
            struct Minimal_SyntaxTree_t* arguments;
        };
        struct {
            struct Minimal_SyntaxTree_t* argument;
            struct Minimal_SyntaxTree_t* next_arg;
        };
        struct {
            char* _operator;
            struct Minimal_SyntaxTree_t* op1;
            struct Minimal_SyntaxTree_t* op2;
        };
        struct {
            int size;
            struct Minimal_SyntaxTree_t** tuple;
        };
        char* ptr;
    };
};

struct Minimal_NameValueMap_t {
    char* name;
    MinimalValue value;
    struct Minimal_NameValueMap_t* left;
    struct Minimal_NameValueMap_t* right;
};

struct MinimalLayer_t {
    char* name;
    MinimalLayer parent;
    Minimal_NameValueMap map;
};

MinimalLayer Minimal_createLayer();
MinimalLayer Minimal_createLayer2(MinimalLayer parent);

Minimal_SyntaxTree* Minimal_parseTypeSpec(const char* code);
Minimal_SyntaxTree* Minimal_parseCode(char* code);
MinimalValue Minimal_parseValue(char* code);

int Minimal_SyntaxMap_size(Minimal_NameValueMap* map);

MinimalValue Minimal_apply(MinimalValue func, MinimalValue args);
MinimalValue Minimal_evaluate(Minimal_SyntaxTree* code, MinimalLayer layer);

MinimalValue Minimal_getFunction(char* funcname);

#define Minimal_addReference(obj) Minimal_addReference2(obj, __FILE__, __LINE__);
void Minimal_addReference2(MinimalObject ptr, char* file, int line);
int Minimal_getReferenceCount(MinimalObject ptr);
#define Minimal_delReference(ptr) Minimal_delReference2(ptr, __FILE__, __LINE__);
void Minimal_delReference2(MinimalObject ptr, char* file, int line);
MinimalValue Minimal_copy(MinimalValue v);

MinimalLayer Minimal_getCurrentLayer();
MinimalLayer Minimal_setCurrentLayer(MinimalLayer layer);
extern MinimalLayer Minimal_defaultLayer;

char* Minimal_serialise(MinimalValue f, unsigned char include_type);
xmlDocPtr Minimal_serialiseXML(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f, unsigned char include_type);

Minimal_SyntaxTree* Minimal_parseXMLCode(const char* code);
MinimalValue Minimal_xmlToValue(xmlNodePtr node);

extern MinimalValue Minimal_typeType;
extern MinimalValue Minimal_nilType;
extern MinimalValue Minimal_boolType;
extern MinimalValue Minimal_byteType;
extern MinimalValue Minimal_shortType;
extern MinimalValue Minimal_intType;
extern MinimalValue Minimal_longType;
extern MinimalValue Minimal_ubyteType;
extern MinimalValue Minimal_ushortType;
extern MinimalValue Minimal_uintType;
extern MinimalValue Minimal_ulongType;
extern MinimalValue Minimal_floatType;
extern MinimalValue Minimal_doubleType;
extern MinimalValue Minimal_stringType;
extern MinimalValue Minimal_tupleSpaceType;

void Minimal_setOverrideTypeFunc(void (*func)(MinimalValue t));

#ifdef __cplusplus
}
#endif

#endif
