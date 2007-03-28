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

#include <string.h>

#include "minimal_internal.h"

unsigned char Minimal_addTypeToTypeList(Minimal_TypeList* list, MinimalValue type) {
    Minimal_TypeList newlist;
    int i;
    for(i = 0; (*list)[i] != NULL; i++) {
        if((*list)[i] == type) {
            return 0;
        }
    }
    newlist = malloc(sizeof(void*)*(i+2));
    memcpy(newlist, (*list), sizeof(void*)*i);
    Minimal_addReference(type);
    newlist[i] = type;
    newlist[i+1] = NULL;
    free(*list);
    *list = newlist;

    if(type->typeobj != NULL) {
        if(Minimal_addTypeToTypeList(list, type->typeobj)) {
            Minimal_getTypeList2(type->typeobj->type_spec, type->typeobj->typemap, list);
        }
    }

    return 1;
}

Minimal_TypeList Minimal_getTypeList(MinimalValue type) {
    Minimal_TypeList list = malloc(sizeof(void*));
    list[0] = NULL;

    Minimal_addTypeToTypeList(&list, type);

    if(type->type_spec != NULL) {
        Minimal_getTypeList2(type->type_spec, type->typemap, &list);
    }

    return list;
}

void Minimal_getTypeList2(MinimalValue type, MinimalLayer typemap, Minimal_TypeList* list) {
    switch(type->type) {
    case ST_NIL:
        break;
    case ST_IDENTIFIER:
        {
        if(!Minimal_isBuiltIn(type->syntax_tree->string)) {
            MinimalValue v = Minimal_getName(typemap, type->string);
            if(v == NULL) { fprintf(stderr, "Minimal_getTypeList2(%i): Unable to get type %s from %p.\n", __LINE__, type->string, typemap); break; }

            if(Minimal_addTypeToTypeList(list, v)) {
                Minimal_getTypeList2(v->type_spec, typemap, list);
            }
            Minimal_delReference(v);
        }
        }
        break;
    case ST_TYPE_FUNCTION:
        Minimal_getTypeList2(type->syntax_tree->branches[0], typemap, list);
        Minimal_getTypeList2(type->syntax_tree->branches[1], typemap, list);
        break;
    case ST_PRODUCT_TYPE:
        {
        int i;
        for(i = 0; i < type->length; i++) {
            Minimal_getTypeList2(type->syntax_tree->branches[i], typemap, list);
        }
        break;
        }
    case ST_SUM_TYPE:
        {
        int i;
        for(i = 0; i < type->length; i++) {
            Minimal_getTypeList2(type->syntax_tree->branches[i], typemap, list);
        }
        break;
        }
    case ST_POINTER:
        {
        if(!Minimal_isBuiltIn(type->syntax_tree->ptr)) {
            MinimalValue v = Minimal_getName(typemap, type->syntax_tree->ptr);
            if(v == NULL) { fprintf(stderr, "Minimal_getTypeList2(%i): Unable to get type %s.\n", __LINE__, type->string); break; }
            if(Minimal_addTypeToTypeList(list, v)) {
                Minimal_getTypeList2(v->type_spec, typemap, list);
            }
            Minimal_delReference(v);
        }
        }
        break;
    default:
        fprintf(stderr, "Error: Unknown syntax tree id in Minimal_getTypeList2 (%i).\n", type->type);
        break;
    }
}

void Minimal_freeTypeList(Minimal_TypeList list) {
    int i;
    for(i = 0; list[i] != NULL; i++) {
        Minimal_delReference(list[i]);
    }
    free(list);
}
