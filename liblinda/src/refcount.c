/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of LibLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    LibLinda is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    LibLinda is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with LibLinda; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>

#include "linda_internal.h"

void Linda_scanTuple2(LindaValue t, LindaValue ref, LindaValue** scanned) {
    int i;
    Message* m;
    LindaValue* realscanned = NULL;
    Linda_thread_data* tdata;
    LindaValue* newscanned;

    if(scanned == NULL) {
        realscanned = (LindaValue*)malloc(sizeof(void*));
        realscanned[0] = NULL;
        scanned = &realscanned;
    }

    i = 0;
    while((*scanned)[i] != NULL) {
        if((*scanned)[i] == Linda_getPtr(t)) {
            break;
        }
        i++;
    }
     if((*scanned)[i] != NULL) {
        if(&realscanned == scanned) {
            free(realscanned);
        }
        return;
    }
    newscanned = malloc(sizeof(void*)*(i+2));
    memcpy(newscanned, *scanned, sizeof(void*)*i);
    newscanned[i] = Linda_getPtr(t);
    newscanned[i+1] = NULL;
    free(*scanned);
    *scanned = newscanned;

    for(i=0; i<Linda_getTupleSize(t); i++) {
        LindaValue v = Linda_tupleGet(t, i);
        switch(v->type) {
        case M_NIL:
        case M_BOOLEAN:
        case M_BYTE:
        case M_SHORT:
        case M_INTEGER:
        case M_LONG:
        case M_UBYTE:
        case M_USHORT:
        case M_UINTEGER:
        case M_ULONG:
        case M_FLOAT:
        case M_DOUBLE:
        case M_STRING:
        case M_TYPE:
        case M_FUNCTION:
            break;
        case M_TSREF:
            tdata = Linda_get_thread_data();
            m = Message_addReference2(v, Minimal_getTupleSpace(ref));
            Message_send(tdata->sd, NULL, m);
            Message_free(m);
            m = Message_recv(tdata->sd);
            Message_free(m);
            break;
        case M_TUPLE:
            Linda_scanTuple2(v, ref, scanned);
            break;
        case M_POINTER:
            Linda_scanTuple2(Linda_getPtr(v), ref, scanned);
            break;
        case M_SYNTAX_TREE:
            break;
        }
    }

    if(&realscanned == scanned) {
        free(realscanned);
    }
}
