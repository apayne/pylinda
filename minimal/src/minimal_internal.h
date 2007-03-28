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

#include <libxml/tree.h>

#ifndef MINIMAL_INTERNAL_H
#define MINIMAL_INTERNAL_H

#ifdef MINIMAL_C_H
#ifndef HACKY_MAGIC
#error Only include minimal_internal.h, not minimal.h and minimal_internal.h
#endif
#endif
#define MINIMAL_INTERNAL
#include "minimal.h"

#ifdef WIN32
#define inline
#endif

void Minimal_Layer_init();
void Minimal_Layer_finalise();
void Minimal_refCountFinalise();

MinimalValue Minimal_SyntaxTree_createBlank();
MinimalValue Minimal_SyntaxTree_createNil();
MinimalValue Minimal_SyntaxTree_createID(char* id);
MinimalValue Minimal_SyntaxTree_createInteger(int i);
MinimalValue Minimal_SyntaxTree_createSequence();
MinimalValue Minimal_SyntaxTree_createSequence2(MinimalValue v1, MinimalValue v2);
MinimalValue Minimal_SyntaxTree_addToSequence(MinimalValue v1, MinimalValue v2);
MinimalValue Minimal_SyntaxTree_prependToSequence(MinimalValue v1, MinimalValue v2);
MinimalValue Minimal_SyntaxTree_createTypeSpec(MinimalValue id, MinimalValue type_spec);
MinimalValue Minimal_SyntaxTree_createIfExpr(MinimalValue test, MinimalValue expr1, MinimalValue expr2);
MinimalValue Minimal_SyntaxTree_createFunction(MinimalValue arg, MinimalValue value);
MinimalValue Minimal_SyntaxTree_createFunctionDef(MinimalValue name, MinimalValue args, MinimalValue expr);
MinimalValue Minimal_SyntaxTree_createProductType();
MinimalValue Minimal_SyntaxTree_addToProductType(MinimalValue type, MinimalValue v);
MinimalValue Minimal_SyntaxTree_prependToProductType(MinimalValue type, MinimalValue v);
MinimalValue Minimal_SyntaxTree_createSumType();
MinimalValue Minimal_SyntaxTree_addToSumType(MinimalValue type, MinimalValue v);
MinimalValue Minimal_SyntaxTree_prependToSumType(MinimalValue type, MinimalValue v);
MinimalValue Minimal_SyntaxTree_createPointer(MinimalValue v1);
MinimalValue Minimal_SyntaxTree_createOperator(MinimalValue op, MinimalValue expr1, MinimalValue expr2);
MinimalValue Minimal_SyntaxTree_createParameterList1(MinimalValue v1);
MinimalValue Minimal_SyntaxTree_createParameterList2(MinimalValue v1, MinimalValue v2);
MinimalValue Minimal_SyntaxTree_createArgumentList1(MinimalValue v1);
MinimalValue Minimal_SyntaxTree_createArgumentList2(MinimalValue v1, MinimalValue v2);
MinimalValue Minimal_SyntaxTree_createFunctionCall(MinimalValue func, MinimalValue args);

EXPORT int Minimal_SyntaxTree_cmp(Minimal_SyntaxTree* t1, Minimal_SyntaxTree* t2);

MinimalValue Minimal_SyntaxTree_createTuple(int size);
void Minimal_SyntaxTree_setTuple(MinimalValue tuple, int i, MinimalValue tree);
void Minimal_SyntaxTree_addToTuple(MinimalValue tuple, MinimalValue tree);

EXPORT Minimal_SyntaxTree* Minimal_SyntaxTree_copy(Minimal_SyntaxTree* tree);
EXPORT void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree);

void Minimal_addName(Minimal_NameValueMap* map, char* name, MinimalValue tree);
MinimalValue Minimal_getName2(Minimal_NameValueMap* map, char* name);
void Minimal_delName(Minimal_NameValueMap* map, char* name);
void Minimal_SyntaxMap_empty(Minimal_NameValueMap* map);

void Minimal_Layer_free(MinimalLayer layer);
void Minimal_Value_free(MinimalValue val);

static inline void Minimal_SyntaxMap_init(Minimal_NameValueMap* map) {
    map->name = NULL;
    map->left = NULL;
    map->right = NULL;
}

enum MinimalTypeId_t {
    MINIMAL_VALUE,
    MINIMAL_LAYER,
    MINIMAL_MAP,
    MINIMAL_SYNTAXTREE
};
typedef enum MinimalTypeId_t MinimalTypeId;

#define Minimal_newReference(type_id, ptr_type, val_type) ((ptr_type)Minimal_newReference2(type_id, malloc(sizeof(val_type)), __FILE__, __LINE__))

void* Minimal_newReference2(MinimalTypeId type_id, void* ptr, char* file, int line);

void Minimal_performCyclicCollection(MinimalObject ptr);

void Minimal_setReferenceCount(MinimalObject ptr, long count);
void Minimal_delObject(MinimalTypeId type_id, MinimalObject ptr);
MinimalTypeId Minimal_getTypeId(MinimalObject ptr);

MinimalValue Minimal_Value_add(MinimalValue op1, MinimalValue op2);
MinimalValue Minimal_Value_sub(MinimalValue op1, MinimalValue op2);
MinimalValue Minimal_Value_mul(MinimalValue op1, MinimalValue op2);
MinimalValue Minimal_Value_div(MinimalValue op1, MinimalValue op2);
unsigned char Minimal_Value_ge(MinimalValue op1, MinimalValue op2);
unsigned char Minimal_Value_eq(MinimalValue op1, MinimalValue op2);

void Minimal_serialiseValue(xmlDocPtr doc, xmlNodePtr root, xmlNodePtr parent, MinimalValue f, MinimalValue** memo, unsigned char include_type, unsigned char include_type_spec);
void Minimal_serialiseFunction(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f, MinimalValue** memo, unsigned char include_type);

void Minimal_setTypeMap(MinimalValue v, MinimalLayer types);

MinimalValue Minimal_parseXMLValue(const char* code);

struct PointerReplacementArray { unsigned long ptrid; void** realptr; };

struct ValueMemo_t {
    int found_size;
    struct PointerReplacementArray* found;
    int waiting_size;
    struct PointerReplacementArray* waiting;
};
typedef struct ValueMemo_t ValueMemo;

unsigned char Minimal_addTypeToTypeList(Minimal_TypeList* list, MinimalValue type);
void Minimal_getTypeList2(MinimalValue type, MinimalLayer typemap, Minimal_TypeList* list);
void Minimal_freeTypeList(Minimal_TypeList list);

unsigned char Minimal_isBuiltIn(char* type_name);

extern void (*Minimal_override_type_func)(MinimalValue t);

void Minimal_removeFromRefHashTable(MinimalObject ptr);

struct CyclicGarbage {
    MinimalObject ptr;
    MinimalTypeId type_id;
    int count;
    int refcount;
};

struct CyclicGarbageList {
    int size;
    int used;
    struct CyclicGarbage* list;
};

void Minimal_addToCyclicGarbageList(struct CyclicGarbageList* list, MinimalValue ptr);

void Minimal_getReferences(struct CyclicGarbageList* list, MinimalTypeId type_id, MinimalObject ptr);

void Minimal_Value_getReferences(struct CyclicGarbageList* list, MinimalValue ptr);
void Minimal_Layer_getReferences(struct CyclicGarbageList* list, MinimalLayer layer);
void Minimal_SyntaxMap_getReferences(struct CyclicGarbageList* list, Minimal_NameValueMap* ptr);
void Minimal_SyntaxTree_getReferences(struct CyclicGarbageList* list, Minimal_SyntaxTree* ptr);

#endif
