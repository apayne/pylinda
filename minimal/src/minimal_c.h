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

#ifdef WIN32
#ifdef MINIMAL_INTERNAL
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllexport)
#else
#define EXPORT
#define IMPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#define IMPORT
#endif

EXPORT void Minimal_init();
EXPORT void Minimal_finalise();

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
        M_NIL,
        M_BOOLEAN,
        M_BYTE,
        M_SHORT,
        M_INTEGER,
        M_LONG,
        M_UBYTE,
        M_USHORT,
        M_UINTEGER,
        M_ULONG,
        M_FLOAT,
        M_DOUBLE,
        M_STRING,
        M_TYPE,
        M_TSREF,
        M_TUPLE,
        M_FUNCTION,
        M_POINTER
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
            char* type_id;
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

IMPORT extern MinimalValue Minimal_Nil;

EXPORT unsigned char Minimal_isNil(MinimalValue v);
EXPORT MinimalValue Minimal_nil();

EXPORT unsigned char Minimal_isBool(MinimalValue v);
EXPORT MinimalValue Minimal_bool(unsigned char b);
EXPORT unsigned char Minimal_getBool(MinimalValue v);

EXPORT unsigned char Minimal_isByte(MinimalValue v);
EXPORT unsigned char Minimal_isShort(MinimalValue v);
EXPORT unsigned char Minimal_isInt(MinimalValue v);
EXPORT unsigned char Minimal_isLong(MinimalValue v);
EXPORT unsigned char Minimal_isInteger(MinimalValue v);
EXPORT unsigned char Minimal_isUByte(MinimalValue v);
EXPORT unsigned char Minimal_isUShort(MinimalValue v);
EXPORT unsigned char Minimal_isUInt(MinimalValue v);
EXPORT unsigned char Minimal_isULong(MinimalValue v);
EXPORT MinimalValue Minimal_byte(char i);
EXPORT MinimalValue Minimal_short(short i);
EXPORT MinimalValue Minimal_int(int i);
EXPORT MinimalValue Minimal_long(long i);
EXPORT MinimalValue Minimal_ubyte(unsigned char i);
EXPORT MinimalValue Minimal_ushort(unsigned short i);
EXPORT MinimalValue Minimal_uint(unsigned int i);
EXPORT MinimalValue Minimal_ulong(unsigned long i);
EXPORT char Minimal_getByte(MinimalValue v);
EXPORT short Minimal_getShort(MinimalValue v);
EXPORT int Minimal_getInt(MinimalValue v);
EXPORT long Minimal_getLong(MinimalValue v);
EXPORT unsigned char Minimal_getUByte(MinimalValue v);
EXPORT unsigned short Minimal_getUShort(MinimalValue v);
EXPORT unsigned int Minimal_getUInt(MinimalValue v);
EXPORT unsigned long Minimal_getULong(MinimalValue v);

EXPORT unsigned char Minimal_isFloat(MinimalValue v);
EXPORT MinimalValue Minimal_float(float f);
EXPORT float Minimal_getFloat(MinimalValue v);

EXPORT unsigned char Minimal_isDouble(MinimalValue v);
EXPORT MinimalValue Minimal_double(double f);
EXPORT double Minimal_getDouble(MinimalValue v);

EXPORT unsigned char Minimal_isString(MinimalValue v);
EXPORT MinimalValue Minimal_string(char* s);
EXPORT MinimalValue Minimal_string2(char* s, unsigned int len);
EXPORT char* Minimal_getString(MinimalValue v);
EXPORT unsigned int Minimal_getStringLen(MinimalValue v);

EXPORT unsigned char Minimal_isType(MinimalValue v);
EXPORT MinimalValue Minimal_type(const char* typespec);
EXPORT MinimalValue Minimal_typeSpec(const char* type_name, Minimal_SyntaxTree* type_spec);
EXPORT MinimalValue Minimal_function(char* code);
EXPORT MinimalValue Minimal_function2(char* func_name, Minimal_SyntaxTree* type_spec, Minimal_SyntaxTree* parameters, Minimal_SyntaxTree* code);

EXPORT MinimalValue Minimal_copy(MinimalValue v);
EXPORT char* Minimal_Value_string(MinimalValue v);

EXPORT unsigned char Minimal_isTuple(MinimalValue v);
EXPORT MinimalValue Minimal_tuple(int size);
EXPORT int Minimal_getTupleSize(MinimalValue tuple);
EXPORT void Minimal_tupleAdd(MinimalValue tuple, MinimalValue value);
EXPORT void Minimal_tupleSet(MinimalValue tuple, int pos, MinimalValue value);
EXPORT MinimalValue Minimal_tupleGet(MinimalValue tuple, int pos);

EXPORT unsigned char Minimal_isPtr(MinimalValue v);
EXPORT MinimalValue Minimal_ptr(MinimalValue ptr);
EXPORT MinimalValue Minimal_getPtr(MinimalValue v);
EXPORT MinimalValue Minimal_getPtrType(MinimalValue v);

EXPORT unsigned char Minimal_isTupleSpace(MinimalValue v);
EXPORT MinimalValue Minimal_tupleSpace(const char* ts);
EXPORT char* Minimal_getTupleSpace(MinimalValue v);

EXPORT void Minimal_setType(MinimalValue value, MinimalValue type);
EXPORT MinimalValue Minimal_getType(MinimalValue value);
EXPORT void Minimal_setSumPos(MinimalValue value, int sum_pos);
EXPORT int Minimal_getSumPos(MinimalValue value);

EXPORT unsigned char Minimal_isTrue(MinimalValue value);

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
        ST_POINTER,
        ST_IFEXPR
    } type;
    char* type_id;
    union {
        int integer;
        double floating;
        char* string;
        struct {
            int length;
            struct Minimal_SyntaxTree_t** branches;
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
        struct {
            struct Minimal_SyntaxTree_t* _if;
            struct Minimal_SyntaxTree_t* _then;
            struct Minimal_SyntaxTree_t* _else;
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

EXPORT MinimalLayer Minimal_createLayer();
EXPORT MinimalLayer Minimal_createLayer2(MinimalLayer parent);

EXPORT Minimal_SyntaxTree* Minimal_parseTypeSpec(const char* code);
EXPORT Minimal_SyntaxTree* Minimal_parseCode(char* code);
EXPORT MinimalValue Minimal_parseValue(char* code);

EXPORT int Minimal_SyntaxMap_size(Minimal_NameValueMap* map);

EXPORT MinimalValue Minimal_apply(MinimalValue func, MinimalValue args);
EXPORT MinimalValue Minimal_evaluate(Minimal_SyntaxTree* code, MinimalLayer layer);

EXPORT MinimalValue Minimal_getFunction(char* funcname);

#define Minimal_addReference(obj) Minimal_addReference2(obj, __FILE__, __LINE__);
EXPORT void Minimal_addReference2(MinimalObject ptr, char* file, int line);
EXPORT int Minimal_getReferenceCount(MinimalObject ptr);
#define Minimal_delReference(ptr) Minimal_delReference2(ptr, __FILE__, __LINE__);
EXPORT void Minimal_delReference2(MinimalObject ptr, char* file, int line);
EXPORT MinimalValue Minimal_copy(MinimalValue v);

EXPORT MinimalLayer Minimal_getCurrentLayer();
EXPORT MinimalLayer Minimal_setCurrentLayer(MinimalLayer layer);

IMPORT extern MinimalLayer Minimal_defaultLayer;

EXPORT char* Minimal_serialise(MinimalValue f, unsigned char include_type, unsigned char include_type_spec);
EXPORT xmlDocPtr Minimal_serialiseXML(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f, unsigned char include_type, unsigned char include_type_spec);

EXPORT Minimal_SyntaxTree* Minimal_parseXMLCode(const char* code);
EXPORT MinimalValue Minimal_xmlToValue(xmlNodePtr node);

IMPORT extern MinimalValue Minimal_typeType;
IMPORT extern MinimalValue Minimal_nilType;
IMPORT extern MinimalValue Minimal_boolType;
IMPORT extern MinimalValue Minimal_byteType;
IMPORT extern MinimalValue Minimal_shortType;
IMPORT extern MinimalValue Minimal_intType;
IMPORT extern MinimalValue Minimal_longType;
IMPORT extern MinimalValue Minimal_ubyteType;
IMPORT extern MinimalValue Minimal_ushortType;
IMPORT extern MinimalValue Minimal_uintType;
IMPORT extern MinimalValue Minimal_ulongType;
IMPORT extern MinimalValue Minimal_floatType;
IMPORT extern MinimalValue Minimal_doubleType;
IMPORT extern MinimalValue Minimal_stringType;
IMPORT extern MinimalValue Minimal_tupleSpaceType;

EXPORT void Minimal_setOverrideTypeFunc(void (*func)(MinimalValue t));
EXPORT void Minimal_setLindaTSAddRefFunc(void (*func)(MinimalValue t));
EXPORT void Minimal_setLindaTSDelRefFunc(void (*func)(MinimalValue t));

typedef MinimalValue* Minimal_TypeList;
EXPORT Minimal_TypeList Minimal_getTypeList(MinimalValue type);

IMPORT extern unsigned char Minimal_use_types;

/* These should probably be internal. */

EXPORT void Minimal_Layer_addTree(MinimalLayer layer, Minimal_SyntaxTree* tree);

EXPORT void Minimal_interpreter();

EXPORT MinimalValue Minimal_getName(MinimalLayer layer, char* name);

#ifdef __cplusplus
}
#endif

#endif
