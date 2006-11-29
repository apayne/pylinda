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

struct CyclicGarbage {
    MinimalObject ptr;
    MinimalTypeId type_id;
    int count;
};

void Minimal_performCyclicCollection(MinimalObject ptr) {
    int i;
    int j;
    int k;
    struct CyclicGarbage* list = malloc(sizeof(struct CyclicGarbage)*2);
    list[0].ptr = ptr;
    list[0].type_id = Minimal_getTypeId(ptr);
    list[0].count = 0;
    list[1].ptr = NULL;

    /* Calculate the clique */

    i = 0;
    while(list[i].ptr != NULL) {
        MinimalObject* newrefs = Minimal_getReferences(list[i].type_id, list[i].ptr);

        j = 0;
        while(newrefs[j] != NULL) {
            k = 0;
            while(list[k].ptr != NULL) {
                if(list[k].ptr == newrefs[j]) {
                    list[k].count++;
                    break;
                }

                k++;
            }
            if(list[k].ptr == NULL) {
                struct CyclicGarbage* newlist = malloc(sizeof(struct CyclicGarbage)*(k+2));
                memcpy(newlist, list, sizeof(struct CyclicGarbage)*k);
                newlist[k].ptr = newrefs[j];
                newlist[k].type_id = Minimal_getTypeId(newrefs[j]);
                newlist[k].count = 1;
                newlist[k+1].ptr = NULL;
                free(list); list = newlist;
            }

            j++;
        }

        free(newrefs);

        i++;
    }

    /* Are we garbage? */

    i = 0;
    while(list[i].ptr != NULL) {
        int count = Minimal_getReferenceCount(list[i].ptr);
        if(count == -1) { /* We're already being garbage collected. */
            free(list);
            return;
        } else if(count > list[i].count) {
            free(list);
            return;
        } else if(count < list[i].count) {
            fprintf(stderr, "Minimal: Real reference count (%i) lower than cyclic count (%i). This should not happen!\n", count, list[i].count);
            free(list);
            return;
        }

        i++;
    }

    /* Destroy them all! Mwahahaha! */
    i = 0;
    while(list[i].ptr != NULL) {
        Minimal_setReferenceCount(list[i].ptr, -1);
        i++;
    }
    i = 0;
    while(list[i].ptr != NULL) {
        Minimal_delObject(list[i].type_id, list[i].ptr);
        i++;
    }
    i = 0;
    while(list[i].ptr != NULL) {
        Minimal_setReferenceCount(list[i].ptr, 0);
        i++;
    }

    free(list);
}
