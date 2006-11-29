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

#include <sys/un.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "linda.h"
#include "linda_internal.h"

char* Linda_version = "0.7a";

char* Linda_process_id = NULL;

unsigned char Linda_is_server = 0;

int Linda_port = 2102;

int Linda_active_connections = 0;

unsigned char Linda_inited = 0;

LindaValue Linda_uts;

void Linda_init() {
    if(Linda_inited) { return; }
    Linda_inited = 1;

    Minimal_init();

    Linda_uts = Minimal_tupleSpace("UTS");
}

void Linda_finalise() {
    if(!Linda_inited) { return; }
    Linda_inited = 0;

    Linda_delReference(Linda_uts);

    free(Linda_process_id);

    Minimal_finalise();
}

unsigned char Linda_connect(int port) {
    int err;
    Message* m;
    Linda_thread_data* tdata = Linda_get_thread_data();

    Linda_init();

    if(tdata->sd != 0) { return 1; }

#ifdef USE_DOMAIN_SOCKETS
    tdata->sd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(tdata->sd != -1) {
        struct sockaddr_un addr_in;
        addr_in.sun_family = AF_UNIX;
        memcpy(addr_in.sun_path, "/tmp/pylinda", 13);

        err = connect(tdata->sd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
        if(err == -1) {
            close(tdata->sd);
            tdata->sd = -1;
        }
    }
    if(tdata->sd == -1) {
#endif
    tdata->sd = socket(PF_INET, SOCK_STREAM, 0);
    if(tdata->sd == -1) return 0;

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    if(inet_aton("127.0.0.1", (struct in_addr*)&(addr_in.sin_addr.s_addr)) == 0) { close(tdata->sd); tdata->sd = 0; return 0; }

    memset(&(addr_in.sin_zero), 0, 8);

    err = connect(tdata->sd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
    if(err == -1) { close(tdata->sd); tdata->sd = 0; return 0 ; }

#ifdef USE_DOMAIN_SOCKETS
    }
#endif

    Linda_active_connections += 1;

    if(Linda_process_id == NULL) {
        m = Message_register_process();
        Message_send(tdata->sd, NULL, m);
        Message_free(m);
        m = Message_recv(tdata->sd);
        Linda_process_id = (char*)malloc(strlen(m->string)+1);
        strcpy(Linda_process_id, m->string);
        Message_free(m);
    }

    m = Message_register_thread();
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    tdata->thread_id = (char*)malloc(strlen(m->string)+1);
    strcpy(tdata->thread_id, m->string);
    Message_free(m);

    return 1;
}

void Linda_disconnect() {
    Linda_thread_data* tdata = Linda_get_thread_data();
    if(tdata->sd == 0) { return; }
    shutdown(tdata->sd, SHUT_RDWR);
    tdata->sd = 0;
    free(tdata->thread_id);
    tdata->thread_id = NULL;
    free(tdata);

    Linda_active_connections -= 1;

    if(Linda_active_connections == 0) {
        Linda_finalise();
    }
}

void Linda_out(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    Linda_scanTuple(t, ts);
    Message* m = Message_out(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    Message_free(m);
}

LindaValue Linda_in(LindaValue ts, LindaValue t) {
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    Message* m = Message_in(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return NULL; }
    r = Linda_copy(m->tuple);
    Message_free(m);
    return r;
}

LindaValue Linda_rd(LindaValue ts, LindaValue t) {
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    Message* m = Message_rd(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return NULL; }
    r = Linda_copy(m->tuple);
    Message_free(m);
    return r;
}

LindaValue Linda_inp(LindaValue ts, LindaValue t) {
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = Message_inp(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return NULL; }
    if(m->type == UNBLOCK) {
        Message_free(m);
        return NULL;
    } else {
        r = Linda_copy(m->tuple);
        Message_free(m);
        return r;
    }
}

LindaValue Linda_rdp(LindaValue ts, LindaValue t) {
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = Message_rdp(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return NULL; }
    if(m->type == UNBLOCK) {
        Message_free(m);
        return NULL;
    } else {
        r = Linda_copy(m->tuple);
        Message_free(m);
        return r;
    }
}

int Linda_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    int i;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = Message_collect(ts1, ts2, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return -1; }
    i = m->i;
    Message_free(m);
    return i;
}

int Linda_copy_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    int i;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = Message_copy_collect(ts1, ts2, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return -1; }
    i = m->i;
    Message_free(m);
    return i;
}
