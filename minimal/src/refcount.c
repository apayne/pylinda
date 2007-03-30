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

struct MinimalRefCount {
    void* ptr;
    MinimalTypeId type_id;
    long count;
    struct MinimalRefCount* next;
    struct MinimalRefCount* prev;
    char* file;
    int line;
};

int Minimal_refCountListSize = 0;
int Minimal_refCountListUsed = 0;
struct MinimalRefCount** Minimal_refCountList = NULL;

void* Minimal_newReference2(MinimalTypeId type_id, void* ptr, char* file, int line) {
    int index;
    struct MinimalRefCount* newchain;

    if(Minimal_refCountListSize == 0) {
        int i;
        Minimal_refCountListSize = 128;
        Minimal_refCountList = malloc(sizeof(void*) * Minimal_refCountListSize);
        for(i = 0; i < Minimal_refCountListSize; i++) { Minimal_refCountList[i] = NULL; }
    } else if(Minimal_refCountListSize * 2 <= Minimal_refCountListUsed) {
        int i;
        int copy = 0;
        struct MinimalRefCount** newlist = malloc(sizeof(void*)*Minimal_refCountListSize * 4);
        for(i = 0; i < Minimal_refCountListSize * 4; i++) { newlist[i] = NULL; }
        for(i = 0; i < Minimal_refCountListSize; i++) {
            struct MinimalRefCount* chain = Minimal_refCountList[i];
            while(chain != NULL) {
                int index = ((unsigned long)chain->ptr >> 3) % (Minimal_refCountListSize * 4);

                struct MinimalRefCount* newchain = malloc(sizeof(struct MinimalRefCount));
                newchain->ptr = chain->ptr;
                newchain->type_id = chain->type_id;
                newchain->count = chain->count;
                newchain->next = newlist[index];
                newchain->prev = NULL;
                newchain->file = chain->file;
                newchain->line = chain->line;
                if(newlist[index] != NULL) { newlist[index]->prev = newchain; }
                newlist[index] = newchain;
                copy++;

                chain = chain->next;
            }
        }
        free(Minimal_refCountList);
        Minimal_refCountList = newlist;
        Minimal_refCountListSize = Minimal_refCountListSize * 4;
    }

    index = hash(ptr);
    newchain = malloc(sizeof(struct MinimalRefCount));
    newchain->ptr = ptr;
    newchain->type_id = type_id;
    newchain->count = 1;
    newchain->next = Minimal_refCountList[index];
    newchain->prev = NULL;
    newchain->file = file;
    newchain->line = line;
    if(Minimal_refCountList[index] != NULL) { Minimal_refCountList[index]->prev = newchain; }
    Minimal_refCountList[index] = newchain;

    Minimal_refCountListUsed++;
    return ptr;
}

void Minimal_addReference2(MinimalObject ptr, char* file, int line) {
    struct MinimalRefCount* list;
    int i;

    if(ptr == NULL) {
        fprintf(stderr, "Error: Minimal_addReference on NULL. (%s:%i)\n", file, line);
        (*(int*)NULL)++;
        return;
    }
    list = Minimal_refCountList[hash(ptr)];
    while(list != NULL) {
        if(list->ptr == ptr) {
            list->count++;
            return;
        }
        list = list->next;
    }
    fprintf(stderr, "Error: addReference to pointer (%p) not allocated with Minimal_newReference (%s:%i).\n", ptr, file, line);
    i = *((int*)ptr);
    i = i + 1;
}

int Minimal_getReferenceCount(MinimalObject ptr) {
    struct MinimalRefCount* list = Minimal_refCountList[hash(ptr)];
    while(list != NULL) {
        if(list->ptr == ptr) {
            return list->count;
        }
        list = list->next;
    }
    return -2;
}

void Minimal_setReferenceCount(MinimalObject ptr, long i) {
    struct MinimalRefCount* list = Minimal_refCountList[hash(ptr)];
    while(list != NULL) {
        if(list->ptr == ptr) {
            list->count = i;
            return;
        }
        list = list->next;
    }
}

MinimalTypeId Minimal_getTypeId(MinimalObject ptr) {
    struct MinimalRefCount* list = Minimal_refCountList[hash(ptr)];
    while(list != NULL) {
        if(list->ptr == ptr) {
            return list->type_id;
        }
        list = list->next;
    }
    return -1;
}

void Minimal_delReference2(MinimalObject ptr, char* file, int line) {
    struct MinimalRefCount* list;

    if(Minimal_refCountListSize == 0) {
        fprintf(stderr, "Error: delReference to pointer (%p) not allocated with Minimal_newReference (%s:%i).\n", ptr, file, line);
        return;
    }
    list = Minimal_refCountList[hash(ptr)];
    while(list != NULL) {
        if(list->ptr == ptr) {
            if(list->count > 0) {
                list->count--;
                if(list->count < 0) {
                    fprintf(stderr, "Error: delReference to pointer (%p) with negative ref count (%s:%i).\n", ptr, file, line);
                } else if(list->count == 0) {
                    Minimal_delObject(list->type_id, ptr);
                    if(list->prev == NULL) {
                        Minimal_refCountList[hash(ptr)] = list->next;
                        if(list->next != NULL) {
                            list->next->prev = NULL;
                        }
                    } else {
                        list->prev->next = list->next;
                        if(list->next != NULL) {
                            list->next->prev = list->prev;
                        }
                    }
                    free(list);
                    Minimal_refCountListUsed--;
                } else {
                    Minimal_performCyclicCollection(ptr);
                }
            }
            return;
        }
        list = list->next;
    }
    fprintf(stderr, "Error: delReference to pointer (%p) not allocated with Minimal_newReference (%s:%i).\n", ptr, file, line);
    (*((int*)ptr))++;
}

void Minimal_removeFromRefHashTable(MinimalObject ptr) {
    struct MinimalRefCount* list = Minimal_refCountList[hash(ptr)];
    while(list != NULL) {
        if(list->ptr == ptr) {
            if(list->prev == NULL) {
                Minimal_refCountList[hash(ptr)] = list->next;
                if(list->next != NULL) {
                    list->next->prev = NULL;
                }
            } else {
                list->prev->next = list->next;
                if(list->next != NULL) {
                    list->next->prev = list->prev;
                }
            }
            free(list);
            Minimal_refCountListUsed--;
            return;
        }
        list = list->next;
    }

}

void Minimal_delObject(MinimalTypeId type_id, MinimalObject ptr) {
    switch(type_id) {
    case MINIMAL_VALUE:
        Minimal_Value_free((MinimalValue)ptr);
        break;
    case MINIMAL_LAYER:
        Minimal_Layer_free((MinimalLayer)ptr);
        break;
    case MINIMAL_MAP:
        Minimal_SyntaxMap_empty((Minimal_NameValueMap*)ptr);
        break;
    case MINIMAL_SYNTAXTREE:
        Minimal_SyntaxTree_free((Minimal_SyntaxTree)ptr);
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

int Minimal_countAllReferences() {
    int i;
    int count = 0;
    for(i = 0; i < Minimal_refCountListSize; i++) {
        struct MinimalRefCount* list = Minimal_refCountList[i];
        while(list != NULL) {
            count += list->count;
            list = list->next;
        }
    }
    return count;
}

static void Minimal_refCountEmpty() {
    int i;
    struct MinimalRefCount* prev;
    for(i = 0; i < Minimal_refCountListSize; i++) {
        struct MinimalRefCount* list = Minimal_refCountList[i];
        while(list != NULL) {
            fprintf(stderr, "%p (%s:%i) still has %li references.\n", list->ptr, list->file, list->line, list->count);
            prev = list;
            list = list->next;
            free(prev);
        }
        Minimal_refCountList[i] = NULL;
    }
    Minimal_refCountListUsed = 0;
}

void Minimal_refCountFinalise() {
    int i = Minimal_countAllReferences();
    if(i > 0) {
        fprintf(stderr, "Warning, %i references unfreed. %i of %i remaining.\n", i, Minimal_refCountListUsed, Minimal_refCountListSize);
    }
    if(Minimal_refCountList != NULL) {
        Minimal_refCountEmpty();
        free(Minimal_refCountList);
        Minimal_refCountList = NULL;
        Minimal_refCountListSize = 0;
    }
}
