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

#include <stdlib.h>
#include <string.h>

#include "linda.h"
#include "linda_internal.h"

LindaValue Linda_createTuplespace() {
    LindaValue ts;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = Message_createTuplespace();
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return NULL; }
    else if(m->type != RESULT_STRING) {
        return NULL;
    }
    ts = Linda_tupleSpace(m->string);
    Message_free(m);
    return ts;
}

void Linda_addTSReference(LindaValue ts) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    if(strcmp(Linda_getTupleSpace(ts), "UTS") == 0) { return; }
    if(Linda_active_connections == 0) { return; } /* We've been disconnected so ignore this message. */
    Message* m = Message_addReference(ts);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    Message_free(m);
}

void Linda_delTSReference(LindaValue ts) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    if(strcmp(Linda_getTupleSpace(ts), "UTS") == 0) { return; }
    if(Linda_active_connections == 0) { return; } /* We've been disconnected so ignore this message. */
    Message* m = Message_deleteReference(ts);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    Message_free(m);
}
