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
#include <string.h>

#include "minimal_internal.h"

struct CyclicGarbageList* Minimal_newCyclicGarbageList() {
    struct CyclicGarbageList* list = malloc(sizeof(struct CyclicGarbageList));
    list->size = 100;
    list->used = 0;
    list->list = malloc(sizeof(struct CyclicGarbage)*100);
    return list;
};

void Minimal_addToCyclicGarbageList(struct CyclicGarbageList* list, MinimalObject ptr) {
    if(ptr == NULL) {
        return;
    } else if(list->used < list->size) {
        int refcount = Minimal_getReferenceCount(ptr);
        int k = 0;
        if(refcount < 0) { return; }
        struct CyclicGarbage* item = &(list->list[list->used]);
        while(k < list->used) {
             if(list->list[k].ptr == ptr) {
                list->list[k].count++;
                return;
            }

            k++;
        }

        list->used++;

        item->ptr = ptr;
        item->type_id = Minimal_getTypeId(ptr);
        item->count = 1;
        item->refcount = refcount;
    } else {
        struct CyclicGarbage* newlist = malloc(sizeof(struct CyclicGarbage)*(list->used*2));
        memcpy(newlist, list->list, sizeof(struct CyclicGarbage)*list->used);
        free(list->list); list->list = newlist;
        list->size = list->used * 2;
        Minimal_addToCyclicGarbageList(list, ptr);
    }
}

void Minimal_delCyclicGarbageList(struct CyclicGarbageList* list) {
    free(list->list);
    free(list);
}

void Minimal_performCyclicCollection(MinimalObject ptr) {
    int i;
    struct CyclicGarbageList* list;
    struct CyclicGarbage* item;

    if(Minimal_getReferenceCount(ptr) < 0) {
        return;
    }

    list = Minimal_newCyclicGarbageList();
    item = &(list->list[0]);
    item->ptr = ptr;
    item->type_id = Minimal_getTypeId(ptr);
    item->count = 0;
    item->refcount = Minimal_getReferenceCount(ptr);
    list->used = 1;

    /* Calculate the clique */

    i = 0;
    while(i < list->used) {
        item = &(list->list[i]);
        Minimal_getReferences(list, item->type_id, item->ptr);

        i++;
    }

    /* Are we garbage? */

    i = 0;
    while(i < list->used) {
        if(list->list[i].refcount > list->list[i].count) {
            Minimal_delCyclicGarbageList(list);
            return;
        } else if(list->list[i].refcount < list->list[i].count) {
            fprintf(stderr, "Minimal: Real reference count (%i) lower than cyclic count (%i). This should not happen! (%p type %i)\n", list->list[i].refcount, list->list[i].count, list->list[i].ptr, Minimal_getTypeId(list->list[i].ptr));
            Minimal_delCyclicGarbageList(list);
            *((int*)NULL) = 0;
            return;
        }

        i++;
    }

    /* Destroy them all! Mwahahaha! */
    i = 0;
    while(i < list->used) {
        Minimal_setReferenceCount(list->list[i].ptr, -1);
        i++;
    }
    i = 0;
    while(i < list->used) {
        Minimal_delObject(list->list[i].type_id, list->list[i].ptr, 0);
        i++;
    }
    i = 0;
    while(i < list->used) {
        free(list->list[i].ptr);
        i++;
    }
    Minimal_delCyclicGarbageList(list);
}
