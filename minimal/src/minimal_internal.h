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

#include "minimal.h"

#ifndef MINIMAL_INTERNAL_H

void Minimal_Layer_init();
void Minimal_Layer_finalise();

Minimal_SyntaxTree Minimal_SyntaxTree_createID(char* id);
Minimal_SyntaxTree Minimal_SyntaxTree_createInteger(int i);
Minimal_SyntaxTree Minimal_SyntaxTree_createOperator(char* op);

Minimal_SyntaxTree Minimal_SyntaxTree_createTuple(int size);
void Minimal_SyntaxTree_addToTuple(Minimal_SyntaxTree* tuple, Minimal_SyntaxTree* tree);

Minimal_SyntaxTree* Minimal_SyntaxTree_copy(Minimal_SyntaxTree* tree);
void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree);
void Minimal_SyntaxTree_clear(Minimal_SyntaxTree* tree);

void Minimal_addName(Minimal_NameValueMap* map, char* name, MinimalValue tree);
MinimalValue Minimal_getName(MinimalLayer layer, char* name);
MinimalValue Minimal_getName2(Minimal_NameValueMap* map, char* name);
void Minimal_delName(Minimal_NameValueMap* map, char* name);
void Minimal_SyntaxMap_empty(Minimal_NameValueMap* map);

void Minimal_Layer_addTree(MinimalLayer layer, Minimal_SyntaxTree* tree);

void Minimal_interpreter();

void Minimal_Layer_free(MinimalLayer layer);
void Minimal_Value_free(MinimalValue val);

#define Minimal_SyntaxMap_init(map) \
    (map)->name = NULL; \
    (map)->left = NULL; \
    (map)->right = NULL

enum MinimalTypeId_t {
    MINIMAL_VALUE,
    MINIMAL_LAYER
};
typedef enum MinimalTypeId_t MinimalTypeId;

#define Minimal_newReference(type_id, ptr_type, val_type) ((ptr_type)Minimal_newReference2(type_id, malloc(sizeof(val_type))))

void* Minimal_newReference2(MinimalTypeId type_id, void* ptr);

MinimalValue Minimal_Value_add(MinimalValue op1, MinimalValue op2);
MinimalValue Minimal_Value_sub(MinimalValue op1, MinimalValue op2);
MinimalValue Minimal_Value_mul(MinimalValue op1, MinimalValue op2);
MinimalValue Minimal_Value_div(MinimalValue op1, MinimalValue op2);

void Minimal_serialiseValue(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f);
void Minimal_serialiseFunction(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f);

#endif
