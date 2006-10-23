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

#ifndef MINIMAL_H

void Minimal_init();
void Minimal_finalise();

typedef void* MinimalObject;

struct Tuple_t;
typedef struct Tuple_t* Tuple;

struct MinimalValue_t;
typedef struct MinimalValue_t* MinimalValue;

struct Minimal_SyntaxTree_t;
typedef struct Minimal_SyntaxTree_t Minimal_SyntaxTree;

struct MinimalLayer_t;
typedef struct MinimalLayer_t* MinimalLayer;

struct MinimalValue_t {
    enum {
        NIL,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        TYPE,
        TSREF,
        TUPLE,
        FUNCTION
    } type;
    union {
        unsigned char boolean;
        long integer;
        float floating;
        struct {
            char* string;
            unsigned int length;
        };
        struct {
            char* type_name;
            Minimal_SyntaxTree* type_spec;
        };
        char* tsid;
        Tuple tuple;
        struct {
            char* func_name;
            Minimal_SyntaxTree* func_type;
            Minimal_SyntaxTree* parameter_list;
            Minimal_SyntaxTree* code;
            MinimalLayer layer;
        };
    };
};

struct Minimal_Tuple_t {
    MinimalValue* values;
    int size;
};

MinimalValue Minimal_nil();

unsigned char Minimal_isInt(MinimalValue v);
MinimalValue Minimal_int(int i);
int Minimal_getInt(MinimalValue v);

unsigned char Minimal_isFloat(MinimalValue v);
MinimalValue Minimal_float(float f);
float Minimal_getFloat(MinimalValue v);

unsigned char Minimal_isString(MinimalValue v);
MinimalValue Minimal_string(char* s);
MinimalValue Minimal_string2(char* s, unsigned int len);
char* Minimal_getString(MinimalValue v);
unsigned int Minimal_getStringLen(MinimalValue v);

unsigned char Minimal_isTypeSpec(MinimalValue v);
MinimalValue Minimal_typeSpec(char* type_name, Minimal_SyntaxTree* type_spec);
MinimalValue Minimal_function(char* func_name, Minimal_SyntaxTree* type_spec, Minimal_SyntaxTree* parameters, Minimal_SyntaxTree* code);

char* Minimal_Value_string(MinimalValue v);

struct Minimal_SyntaxTree_t {
    enum {
        ST_BLANK,
        ST_IDENTIFIER,
        ST_INTEGER,
        ST_SEQENTIAL_DEFS,
        ST_TYPE_SPEC,
        ST_TYPE_FUNCTION,
        ST_FUNCTION_DEF,
        ST_PARAMETER_LIST,
        ST_FUNCTION_CALL,
        ST_ARGUMENT_LIST,
        ST_OPERATOR
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
            struct Minimal_SyntaxTree_t* type_def;
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
    };
};

struct Minimal_NameValueMap_t {
    char* name;
    MinimalValue value;
    struct Minimal_NameValueMap_t* left;
    struct Minimal_NameValueMap_t* right;
};
typedef struct Minimal_NameValueMap_t Minimal_NameValueMap;

struct MinimalLayer_t {
    char* name;
    MinimalLayer parent;
    Minimal_NameValueMap map;
};

MinimalLayer Minimal_createLayer();
MinimalLayer Minimal_createLayer2(MinimalLayer parent);

Minimal_SyntaxTree* Minimal_parseTypeSpec(char* code);
Minimal_SyntaxTree* Minimal_parseCode(char* code);

MinimalValue Minimal_evaluate(Minimal_SyntaxTree* code, MinimalLayer layer);

MinimalValue Minimal_getFunction(char* funcname);

void Minimal_addReference(MinimalObject obj);
void Minimal_delReference(MinimalObject obj);

MinimalLayer Minimal_getCurrentLayer();
MinimalLayer Minimal_setCurrentLayer(MinimalLayer layer);
extern MinimalLayer Minimal_defaultLayer;

#endif
