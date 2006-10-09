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

#include "linda.h"
#include "linda_internal.h"

void Linda_scanTuple(Tuple t, char* ref) {
    int i;
    Message* m;
    Linda_thread_data* tdata;

    for(i=0; i<Tuple_get_size(t); i++) {
        Value* v = Tuple_get(t, i);
        switch(v->type) {
        case NIL:
        case BOOLEAN:
        case INTEGER:
        case FLOAT:
        case STRING:
        case TYPE:
            break;
        case TSREF:
            tdata = Linda_get_thread_data();
            m = Message_addReference2(v->tsid, ref);
            Message_send(tdata->sd, NULL, m);
            Message_free(m);
            m = Message_recv(tdata->sd);
            Message_free(m);
            break;
        case TUPLE:
            Linda_scanTuple(Value_get_tuple(v), ref);
            break;
        default:
            fprintf(stderr, "Unknown value (%i) found when scanning tuple.\n", v->type);
        }
    }
}
