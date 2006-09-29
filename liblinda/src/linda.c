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

#include "linda.h"
#include "linda_internal.h"

char* version = "0.7a";

char* process_id = NULL;
char* thread_id = NULL;

int Linda_sd = 0;
int Linda_port = 2102;

int Linda_active_connections = 0;

unsigned char Linda_connect(int port) {
    int err;

#ifdef USE_DOMAIN_SOCKETS
    Linda_sd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(Linda_sd != -1) {
        struct sockaddr_un addr_in;
        addr_in.sun_family = AF_UNIX;
        memcpy(addr_in.sun_path, "/tmp/pylinda", 13);

        err = connect(Linda_sd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
        if(err == -1) {
            close(Linda_sd);
            Linda_sd = -1;
        }
    }
    if(Linda_sd == -1) {
#endif
    Linda_sd = socket(PF_INET, SOCK_STREAM, 0);
    if(Linda_sd == -1) return 0;

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    if(inet_aton("127.0.0.1", (struct in_addr*)&(addr_in.sin_addr.s_addr)) == 0) return 0;

    memset(&(addr_in.sin_zero), 0, 8);

    int err = connect(Linda_sd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
    if(err == -1) return 0;

#ifdef USE_DOMAIN_SOCKETS
    }
#endif

    Linda_active_connections += 1;

    Message* m = Message_register_process();
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    process_id = (char*)malloc(strlen(m->string)+1);
    strcpy(process_id, m->string);
    Message_free(m);

    m = Message_register_thread();
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    thread_id = (char*)malloc(strlen(m->string)+1);
    strcpy(thread_id, m->string);
    Message_free(m);

    return 1;
}

void Linda_disconnect() {
    if(Linda_active_connections == 0) { return; }
    shutdown(Linda_sd, SHUT_RDWR);

    Linda_active_connections -= 1;
    if(Linda_active_connections == 0) {
        Message_shutdown();
    }
}

void Linda_out(const Linda_tuplespace ts, Tuple t) {
    Linda_scanTuple(t, ts);
    Message* m = Message_out(ts, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    Message_free(m);
}

Tuple Linda_in(const Linda_tuplespace ts, Tuple t) {
    Tuple r;
    Message* m = Message_in(ts, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    r = Tuple_copy(m->tuple);
    Message_free(m);
    return r;
}

Tuple Linda_rd(const Linda_tuplespace ts, Tuple t) {
    Tuple r;
    Message* m = Message_rd(ts, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    r = Tuple_copy(m->tuple);
    Message_free(m);
    return r;
}

Tuple Linda_inp(const Linda_tuplespace ts, Tuple t) {
    Tuple r;
    Message* m = Message_inp(ts, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    if(m->type == UNBLOCK) {
        Message_free(m);
        return NULL;
    } else {
        r = Tuple_copy(m->tuple);
        Message_free(m);
        return r;
    }
}

Tuple Linda_rdp(const Linda_tuplespace ts, Tuple t) {
    Tuple r;
    Message* m = Message_rdp(ts, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    if(m->type == UNBLOCK) {
        Message_free(m);
        return NULL;
    } else {
        r = Tuple_copy(m->tuple);
        Message_free(m);
        return r;
    }
}

int Linda_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, Tuple t) {
    int i;
    Message* m = Message_collect(ts1, ts2, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    i = m->i;
    Message_free(m);
    return i;
}

int Linda_copy_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, Tuple t) {
    int i;
    Message* m = Message_copy_collect(ts1, ts2, t);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    i = m->i;
    Message_free(m);
    return i;
}
