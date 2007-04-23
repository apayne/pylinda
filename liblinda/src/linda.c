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

#include "linda_internal.h"

#ifdef USE_DOMAIN_SOCKETS
#include <sys/un.h>
#endif
#include <sys/types.h> 
#ifdef USE_DOMAIN_SOCKETS
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

char* Linda_version = "0.7a";

char* Linda_process_id = NULL;

unsigned char Linda_is_server = 0;

int Linda_port = 2102;

int Linda_active_connections = 0;

unsigned char Linda_inited = 0;

LindaValue Linda_uts;

#ifndef TYPES
unsigned char Linda_use_types = 0;
#else
unsigned char Linda_use_types = 1;
#ifdef REGISTER_TYPES
unsigned char Linda_register_types = 1;
#else
unsigned char Linda_register_types = 0;
#endif
#endif

LindaValue* Linda_unregistered_type_list = NULL;

void Linda_init() {
    if(Linda_inited) { return; }
    Linda_inited = 1;

#ifdef TYPES
    Minimal_use_types = 1;
#ifdef REGISTER_TYPES
    Minimal_setOverrideTypeFunc(Linda_registerType);
#endif
#else
    Minimal_use_types = 0;
#endif
    Minimal_setLindaTSAddRefFunc(Linda_addTSReference);
    Minimal_setLindaTSDelRefFunc(Linda_delTSReference);

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
    struct sockaddr_in addr_in;
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

#ifdef REGISTER_TYPES
    if(Linda_active_connections == 1 && Linda_unregistered_type_list != NULL) { /* is this the first time we've connected? */
        int i;
        for(i = 0; Linda_unregistered_type_list[i] != NULL; i++) {
            Linda_registerType(Linda_unregistered_type_list[i]);
            Linda_delReference(Linda_unregistered_type_list[i]);
        }
        free(Linda_unregistered_type_list);
        Linda_unregistered_type_list = NULL;
    }
#endif

    return 1;
}

void Linda_disconnect() {
    Linda_thread_data* tdata = Linda_get_thread_data();
    if(tdata->sd == 0) { return; }
#ifdef WIN32
    shutdown(tdata->sd, SD_BOTH);
#else
    shutdown(tdata->sd, SHUT_RDWR);
#endif
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
    Message* m;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    Linda_scanTuple(t, ts);
#ifdef TYPES
#ifdef DEBUG
    {
    int i;
    for(i=0; i<Linda_getTupleSize(t); i++) {
        if(Linda_getType(Linda_tupleGet(t, i)) == NULL) {
            fprintf(stderr, "Warning, sending a tuple with an element without a type.\n");
        }
    }
    }
#endif
#endif
    m = Message_out(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    Message_free(m);
    Linda_delReference(t);
}

LindaValue Linda_in(LindaValue ts, LindaValue t) {
    Message* m;
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    m = Message_in(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { Linda_delReference(t); return NULL; }
    Linda_addReference(m->tuple);
    r = m->tuple;
    Message_free(m);

#ifdef TYPES
    {
    int i;
    for(i = 0; i < Linda_getTupleSize(r); i++) {
        LindaValue v1 = Linda_tupleGet(t, i);
        LindaValue v2 = Linda_tupleGet(r, i);

        if(Linda_isType(v1)) {
            Linda_setType(v2, v1);
        } else {
            Linda_setType(v2, v1->typeobj);
        }
    }
    }
#endif
    Linda_delReference(t);

    return r;
}

LindaValue Linda_rd(LindaValue ts, LindaValue t) {
    Message* m;
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    m = Message_rd(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { Linda_delReference(t); return NULL; }
    Linda_addReference(m->tuple);
    r = m->tuple;
    Message_free(m);

#ifdef TYPES
    {
    int i;
    for(i = 0; i < Linda_getTupleSize(r); i++) {
        LindaValue v1 = Linda_tupleGet(t, i);
        LindaValue v2 = Linda_tupleGet(r, i);

        Linda_setType(v2, v1->typeobj);
    }
    }
#endif
    Linda_delReference(t);

    return r;
}

LindaValue Linda_inp(LindaValue ts, LindaValue t) {
    Message* m;
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    m = Message_inp(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { Linda_delReference(t); return NULL; }
    if(m->type == L_UNBLOCK) {
        Message_free(m);

        Linda_delReference(t);
        return NULL;
    } else {
        Linda_addReference(m->tuple);
        r = m->tuple;
        Message_free(m);

#ifdef TYPES
        {
        int i;
        for(i = 0; i < Linda_getTupleSize(r); i++) {
            LindaValue v1 = Linda_tupleGet(t, i);
            LindaValue v2 = Linda_tupleGet(r, i);

            Linda_setType(v2, v1->typeobj);
        }
        }
#endif

        Linda_delReference(t);
        return r;
    }
}

LindaValue Linda_rdp(LindaValue ts, LindaValue t) {
    Message* m;
    LindaValue r;
    Linda_thread_data* tdata = Linda_get_thread_data();
    Linda_addReference(t);
    m = Message_rdp(ts, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return NULL; }
    if(m->type == L_UNBLOCK) {
        Message_free(m);

        Linda_delReference(t);
        return NULL;
    } else {
        Linda_addReference(m->tuple);
        r = m->tuple;
        Message_free(m);

#ifdef TYPES
        {
        int i;
        for(i = 0; i < Linda_getTupleSize(r); i++) {
            LindaValue v1 = Linda_tupleGet(t, i);
            LindaValue v2 = Linda_tupleGet(r, i);

            Linda_setType(v2, v1->typeobj);
        }
        }
#endif

        Linda_delReference(t);
        return r;
    }
}

int Linda_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    int i;
    Message* m;
    Linda_thread_data* tdata = Linda_get_thread_data();
    m = Message_collect(ts1, ts2, t);
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
    Message* m;
    Linda_thread_data* tdata = Linda_get_thread_data();
    m = Message_copy_collect(ts1, ts2, t);
    Message_send(tdata->sd, NULL, m);
    Message_free(m);
    m = Message_recv(tdata->sd);
    if(m == NULL) { return -1; }
    i = m->i;
    Message_free(m);
    return i;
}

#ifdef REGISTER_TYPES
void Linda_registerType(LindaValue t) {
    if(t->type_id != 0) {
        return;
    } else if(Linda_active_connections > 0 && Linda_process_id != NULL) {
        int i;
        Linda_thread_data* tdata = Linda_get_thread_data();

        Minimal_TypeList types = Minimal_getTypeList(t);
        for(i = 0; types[i] != NULL; i++) {
            Message* m;
            if(types[i]->type_id != NULL) { continue; }
            m = Message_register_type(types[i]);
            Message_send(tdata->sd, NULL, m);
            Message_free(m);
            m = Message_recv(tdata->sd);
            if(m == NULL) { free(types); return; }
            types[i]->type_id = malloc(strlen(m->string) + 1);
            strcpy(types[i]->type_id, m->string);
            Message_free(m);
        }
        for(i = 0; types[i] != NULL; i++) {
            Message* m;
            m = Message_update_type(types[i]->type_id, types[i]);
            Message_send(tdata->sd, NULL, m);
            Message_free(m);
            m = Message_recv(tdata->sd);
            if(m == NULL) { free(types); return; }
            Message_free(m);
        }
        Minimal_freeTypeList(types);
    } else if(Linda_unregistered_type_list == NULL) {
        Linda_unregistered_type_list = malloc(sizeof(void*)*2);
        Linda_addReference(t);
        Linda_unregistered_type_list[0] = t;
        Linda_unregistered_type_list[1] = NULL;
    } else {
        LindaValue* l;
        int i = 0;
        while(Linda_unregistered_type_list[i] != NULL) { i++; }
        l = malloc(sizeof(void*)*(i+2));
        memcpy(l, Linda_unregistered_type_list, sizeof(void*)*i);
        Linda_addReference(t);
        l[i] = t;
        l[i+1] = NULL;
        free(Linda_unregistered_type_list);
        Linda_unregistered_type_list = l;
    }
}
#endif
