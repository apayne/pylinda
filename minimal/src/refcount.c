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

#include <stdlib.h>
#include <stdio.h>

#include "minimal_internal.h"

#define hash(ptr) (((unsigned long)(ptr) >> 3) % Minimal_refCountListSize)

void* Minimal_newReference2(MinimalTypeId type_id, void* ptr, char* file, int line) {

    ((MinimalRefCount*)ptr)->ref_count = 1;
    ((MinimalRefCount*)ptr)->value_type = type_id;
    return ptr;
}

void Minimal_addReference2(MinimalObject ptr, char* file, int line) {
    ((MinimalRefCount*)ptr)->ref_count++;
}

int Minimal_getReferenceCount(MinimalObject ptr) {
    return ((MinimalRefCount*)ptr)->ref_count;
}

void Minimal_setReferenceCount(MinimalObject ptr, long i) {
    ((MinimalRefCount*)ptr)->ref_count = i;
}

MinimalTypeId Minimal_getTypeId(MinimalObject ptr) {
    return ((MinimalRefCount*)ptr)->value_type;
}

void Minimal_delReference2(MinimalObject ptr, char* file, int line) {
    if(((MinimalRefCount*)ptr)->ref_count < 0) {
        return;
    }
    ((MinimalRefCount*)ptr)->ref_count--;
    if(((MinimalRefCount*)ptr)->ref_count == 0) {
        Minimal_delObject(((MinimalRefCount*)ptr)->value_type, ptr, 1);
    } else {
        Minimal_performCyclicCollection(ptr);
    }
}

void Minimal_delObject(MinimalTypeId type_id, MinimalObject ptr, unsigned char do_free) {
    switch(type_id) {
    case MINIMAL_VALUE:
        Minimal_Value_free((MinimalValue)ptr);
        if(do_free) {
            free((MinimalValue)ptr);
        }
        break;
    case MINIMAL_LAYER:
        Minimal_Layer_free((MinimalLayer)ptr);
        if(do_free) {
            free((MinimalLayer)ptr);
        }
        break;
    case MINIMAL_MAP:
        Minimal_SyntaxMap_empty((Minimal_NameValueMap*)ptr);
        if(do_free) {
            free((Minimal_NameValueMap*)ptr);
        }
        break;
    case MINIMAL_SYNTAXTREE:
        Minimal_SyntaxTree_free((Minimal_SyntaxTree)ptr);
        if(do_free) {
            free((Minimal_SyntaxTree)ptr);
        }
        break;
    }
}

void Minimal_getReferences(struct CyclicGarbageList* list, MinimalTypeId type_id, MinimalObject ptr) {
    switch(type_id) {
    case MINIMAL_VALUE:
        Minimal_Value_getReferences(list, (MinimalValue)ptr);
        break;
    case MINIMAL_LAYER:
        Minimal_Layer_getReferences(list, (MinimalLayer)ptr);
        break;
    case MINIMAL_MAP:
        Minimal_SyntaxMap_getReferences(list, (Minimal_NameValueMap*)ptr);
        break;
    case MINIMAL_SYNTAXTREE:
        Minimal_SyntaxTree_getReferences(list, (Minimal_SyntaxTree)ptr);
        break;
    }
}
