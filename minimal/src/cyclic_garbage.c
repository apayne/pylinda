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
    struct CyclicGarbage* item = &(list->list[list->used]);
    int refcount;
    int k = 0;

    if(ptr == NULL) {
        return;
    } else if(list->used == list->size) {
        struct CyclicGarbage* newlist = malloc(sizeof(struct CyclicGarbage)*(list->used*2));
        memcpy(newlist, list->list, sizeof(struct CyclicGarbage)*list->used);
        free(list->list); list->list = newlist;
        list->size = list->used * 2;
        Minimal_addToCyclicGarbageList(list, ptr);
        return;
    }

    refcount = Minimal_getReferenceCount(ptr);

    while(k < list->used) {
        if(list->list[k].ptr == ptr) {
            list->list[k].count++;
            break;
        }

        k++;
    }
    if(k == list->used) {
        list->used++;

        item->ptr = ptr;
        item->type_id = Minimal_getTypeId(ptr);
        item->count = 1;
        item->refcount = refcount;
        item->ptrcount = 0;
        item->ptrsize = 10;
        item->ptrtos = malloc(sizeof(int) * 10);
        item->in_clique = 0;
    }

    if(list->list[list->current_object].ptrcount == list->list[list->current_object].ptrsize) {
        int* newlist = malloc(sizeof(int) * (list->list[list->current_object].ptrsize + 10));
        memcpy(newlist, list->list[list->current_object].ptrtos, sizeof(int) * list->list[list->current_object].ptrsize);
        list->list[list->current_object].ptrsize += 10;
        free(list->list[list->current_object].ptrtos);
        list->list[list->current_object].ptrtos = newlist;
    }

    list->list[list->current_object].ptrtos[list->list[list->current_object].ptrcount] = k;
    list->list[list->current_object].ptrcount++;
}

void Minimal_delCyclicGarbageList(struct CyclicGarbageList* list) {
    int i = 0;
    i = 0;
    while(i < list->used) {
        if(list->list[i].ptrtos != NULL) {
            free(list->list[i].ptrtos);
        }
        i++;
    }
    free(list->list);
    free(list);
}

void Minimal_performCyclicCollection(MinimalObject ptr) {
    int i;
    int j;
    int k;
    int l;
    int possible;
    struct CyclicGarbageList* list;
    struct CyclicGarbage* item;
    unsigned char change = 1;

    if(Minimal_getReferenceCount(ptr) < 0) {
        return;
    }

    list = Minimal_newCyclicGarbageList();
    item = &(list->list[0]);
    item->ptr = ptr;
    item->type_id = Minimal_getTypeId(ptr);
    item->count = 0;
    item->refcount = Minimal_getReferenceCount(ptr);
    item->ptrcount = 0;
    item->ptrsize = 10;
    item->ptrtos = malloc(sizeof(int) * 10);
    item->in_clique = 1;
    list->used = 1;

    /* Calculate the maximum possible clique */

    i = 0;
    while(i < list->used) {
        item = &(list->list[i]);
        list->current_object = i;
        Minimal_getReferences(list, item->type_id, item->ptr);

        i++;
    }

    if(list->list[0].refcount > list->list[0].count) {
        Minimal_delCyclicGarbageList(list);
        return;
    } else if(list->list[0].refcount < list->list[0].count) {
        fprintf(stderr, "Minimal: Real reference count (%i) lower than cyclic count (%i). This should not happen! (%p type %i)\n", list->list[0].refcount, list->list[0].count, list->list[0].ptr, Minimal_getTypeId(list->list[0].ptr));
        Minimal_delCyclicGarbageList(list);
        *((int*)NULL) = 0;
        return;
    }

    /* Calculate the clique */
    while(change) {
        change = 0;
        j = 0;
        while(j < list->used) {
            if(list->list[j].in_clique) { j++; continue; }

            for(k=0; k<list->list[j].ptrcount; k++) {
                possible = list->list[j].ptrtos[k];
                if(list->list[possible].in_clique || list->list[possible].refcount < 0) { continue; }

                for(l=0; l<list->list[possible].ptrcount; l++) {
                    if(list->list[list->list[possible].ptrtos[l]].in_clique) {
                        list->list[possible].in_clique = 1;
                        change = 1;

                        if(list->list[possible].refcount > list->list[possible].count) {
                            Minimal_delCyclicGarbageList(list);
                            return;
                        } else if(list->list[possible].refcount < list->list[possible].count) {
                            fprintf(stderr, "Minimal: Real reference count (%i) lower than cyclic count (%i). This should not happen! (%p type %i)\n", list->list[possible].refcount, list->list[possible].count, list->list[possible].ptr, Minimal_getTypeId(list->list[possible].ptr));
                            Minimal_delCyclicGarbageList(list);
                            *((int*)NULL) = 0;
                            return;
                        }

                        break;
                    }
                }
                if(change) { break; }
            }

            if(change) { break; }
            j++;
        }
    }

    /* Destroy them all! Mwahahaha! */
    i = 0;
    while(i < list->used) {
        if(!list->list[i].in_clique) { i++; continue; }

        Minimal_setReferenceCount(list->list[i].ptr, -1);
        i++;
    }
    i = 0;
    while(i < list->used) {
        if(!list->list[i].in_clique) { i++; continue; }

        Minimal_delObject(list->list[i].type_id, list->list[i].ptr, 0);
        i++;
    }
    i = 0;
    while(i < list->used) {
        if(!list->list[i].in_clique) { i++; continue; }

        free(list->list[i].ptr);
        i++;
    }
    Minimal_delCyclicGarbageList(list);
}
