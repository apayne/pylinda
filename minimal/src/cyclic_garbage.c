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
    int i = 0;

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
            item->ptrtos = NULL;
        }
    } else {
        struct CyclicGarbage* newlist = malloc(sizeof(struct CyclicGarbage)*(list->used*2));
        memcpy(newlist, list->list, sizeof(struct CyclicGarbage)*list->used);
        free(list->list); list->list = newlist;
        list->size = list->used * 2;
        Minimal_addToCyclicGarbageList(list, ptr);
    }

    while(list->list[i].ptr != ptr) {
        i++;
    }

    if(list->list[list->current_object].ptrcount == 0) {
        list->list[list->current_object].ptrcount = 1;
        list->list[list->current_object].ptrtos = malloc(sizeof(int));
        list->list[list->current_object].ptrtos[0] = i;
    } else {
        int* newlist = malloc(sizeof(int) * (list->list[list->current_object].ptrcount + 1));
        memcpy(newlist, list->list[list->current_object].ptrtos, sizeof(int) * list->list[list->current_object].ptrcount);
        newlist[list->list[list->current_object].ptrcount] = i;
        list->list[list->current_object].ptrcount++;
        free(list->list[list->current_object].ptrtos);
        list->list[list->current_object].ptrtos = newlist;
    }
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

static unsigned char is_ptr_in_clique(int* clique, int max_size, int ptr) {
    int i;
    for(i=0; i<max_size; i++) {
        if(clique[i] == ptr) {
            return 1;
        }
        if(clique[i] == -1) {
            return 0;
        }
    }
    return 0;
}

void Minimal_performCyclicCollection(MinimalObject ptr) {
    int i;
    int j;
    int k;
    int l;
    int m;
    int possible;
    struct CyclicGarbageList* list;
    struct CyclicGarbage* item;
    int* clique;
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
    item->ptrtos = NULL;
    list->used = 1;

    /* Calculate the possible clique */

    i = 0;
    while(i < list->used) {
        item = &(list->list[i]);
        list->current_object = i;
        Minimal_getReferences(list, item->type_id, item->ptr);

        i++;
    }

    /* Calculate the clique */
    clique = malloc(sizeof(int) * list->used);
    for(i=0; i<list->used; i++) {
        clique[i] = -1;
    }

    i = 0;
    clique[0] = 0;
    while(change) {
        change = 0;
        j = 0;
        while(j < list->used && clique[j] != -1) {
            for(k=0; k<list->list[clique[j]].ptrcount; k++) {
                possible = list->list[clique[j]].ptrtos[k];
                if(is_ptr_in_clique(clique, list->used, possible)) {
                    continue;
                }
                l = 0;
                for(l=0; l<list->list[possible].ptrcount; l++) {
                    if(is_ptr_in_clique(clique, list->used, list->list[possible].ptrtos[l])) {
                        change = 1;
                        break;
                    }
                }
                if(l < list->list[possible].ptrcount) {
                    m = 0;
                    while(clique[m] != -1) {
                        m++;
                    }
                    clique[m] = possible;
                }
                if(change) { break; }
            }

            if(change) { break; }
            j++;
        }
    }

    /* Are we garbage? */

    i = 0;
    while(clique[i] != -1) {
        if(list->list[clique[i]].refcount > list->list[clique[i]].count) {
            free(clique);
            Minimal_delCyclicGarbageList(list);
            return;
        } else if(list->list[clique[i]].refcount < list->list[clique[i]].count) {
            fprintf(stderr, "Minimal: Real reference count (%i) lower than cyclic count (%i). This should not happen! (%p type %i)\n", list->list[i].refcount, list->list[i].count, list->list[i].ptr, Minimal_getTypeId(list->list[i].ptr));
            free(clique);
            Minimal_delCyclicGarbageList(list);
            *((int*)NULL) = 0;
            return;
        }

        i++;
    }

    /* Destroy them all! Mwahahaha! */
    i = 0;
    while(clique[i] != -1) {
        Minimal_setReferenceCount(list->list[clique[i]].ptr, -1);
        i++;
    }
    i = 0;
    while(clique[i] != -1) {
        Minimal_delObject(list->list[clique[i]].type_id, list->list[clique[i]].ptr, 0);
        i++;
    }
    i = 0;
    while(clique[i] != -1) {
        free(list->list[clique[i]].ptr);
        i++;
    }
    free(clique);
    Minimal_delCyclicGarbageList(list);
}
