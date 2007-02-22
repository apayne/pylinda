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

#ifndef LINDA_INTERNAL_H
#define LINDA_INTERNAL_H

#ifdef __LINDA_C_H__
#ifndef HACKY_MAGIC
#error Only include linda_internal.h, not linda.h and linda_internal.h
#endif
#endif
#define LINDA_INTERNAL
#include "linda.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#define USE_DOMAIN_SOCKETS
#endif

struct MsgID_t {
    char* dest;
    char* source;
    int count;
};

typedef struct MsgID_t MsgID;
void MsgID_clear(MsgID* msgid);

struct Message_t {
     enum {
        L_DONE,
        L_DONT_KNOW,
        L_RESULT_STRING,
        L_RESULT_INT,
        L_RESULT_TUPLE,
        L_UNBLOCK,
        L_OUT,
        L_IN,
        L_RD,
        L_INP,
        L_RDP,
        L_COLLECT,
        L_COPY_COLLECT,
        L_CREATE_TUPLESPACE,
        L_ADD_REFERENCE,
        L_DELETE_REFERENCE,
/* Monitor messages */
        L_MONITOR,
        L_LIST_TS,
        L_INSPECT,
        L_GET_ROUTES,
/* Monitor messages */
        L_REGISTER_PROCESS,
        L_REGISTER_THREAD,
        L_REGISTER_TYPE,
        L_UPDATE_TYPE,
/* Server Messages */
        L_MY_NAME_IS,
        L_GET_NODE_ID,
        L_REGISTER_PARTITION,
        L_GET_PARTITIONS,
        L_DELETED_PARTITION,
        L_GET_REQUESTS,
        L_GET_NEIGHBOURS,
        L_GET_CONNECTION_DETAILS,
        L_TUPLE_REQUEST,
        L_CANCEL_REQUEST,
        L_MULTIPLE_IN
/* Server Messages */
     } type;
     MsgID* msgid;
     union {
         char* string;
         int i;
         LindaValue tuple;
         LindaValue ts;
         struct {
             LindaValue ts;
             MinimalValue t;
         } out;
         struct {
             LindaValue ts;
             MinimalValue t;
             char* tid;
         } in;
         struct {
             LindaValue ts;
             MinimalValue t;
             char* tid;
         } rd;
         struct {
             LindaValue ts1;
             LindaValue ts2;
             MinimalValue t;
         } collect;
         struct {
            LindaValue ts;
            char* tid;
         } ref;
         struct {
            LindaValue ts;
            MinimalValue t;
         } tuple_request;
        struct {
            int type_id;
            LindaValue typeobj;
            char* pid;
        } typestruct;
     };
};

typedef struct Message_t Message;

char* Message_getString(Message* msg);
EXPORT void Message_send(int sd, MsgID* msgid, Message* msg);
EXPORT Message* Message_recv(int s);
EXPORT Message* Message_parse(char* text, int len, unsigned char final);

EXPORT Message* Message_result_string(char* text);
EXPORT Message* Message_result_int(int i);
EXPORT Message* Message_result_tuple(LindaValue t);

EXPORT Message* Message_done();
EXPORT Message* Message_dont_know();

EXPORT Message* Message_out(LindaValue ts, LindaValue t);
EXPORT Message* Message_in(LindaValue ts, LindaValue t);
EXPORT Message* Message_rd(LindaValue ts, LindaValue t);
EXPORT Message* Message_inp(LindaValue ts, LindaValue t);
EXPORT Message* Message_rdp(LindaValue ts, LindaValue t);
EXPORT Message* Message_collect(LindaValue ts1, LindaValue ts2, LindaValue t);
EXPORT Message* Message_copy_collect(LindaValue ts1, LindaValue ts2, LindaValue t);
EXPORT Message* Message_unblock();

EXPORT Message* Message_createTuplespace();
EXPORT Message* Message_addReference(LindaValue ts);
EXPORT Message* Message_addReference2(LindaValue ts, char* id);
EXPORT Message* Message_deleteReference(LindaValue ts);

EXPORT Message* Message_monitor();
EXPORT Message* Message_list_ts();
EXPORT Message* Message_inspect(LindaValue ts);
EXPORT Message* Message_get_routes();

EXPORT Message* Message_register_process();
EXPORT Message* Message_register_thread();
#ifdef REGISTER_TYPES
EXPORT Message* Message_register_type(LindaValue v);
EXPORT Message* Message_update_type(int type_id, LindaValue v);
#endif

EXPORT Message* Message_my_name_is(char* name);
EXPORT Message* Message_get_node_id();

EXPORT Message* Message_register_partition(LindaValue ts, char* ref);
EXPORT Message* Message_get_partitions(LindaValue ts);
EXPORT Message* Message_deleted_partition(LindaValue ts, char* ref);

EXPORT Message* Message_get_requests(LindaValue ts);

EXPORT Message* Message_get_neighbours();
EXPORT Message* Message_get_connection_details(char* id);

EXPORT Message* Message_tuple_request(LindaValue ts, LindaValue t);
EXPORT Message* Message_cancel_request(LindaValue ts, LindaValue t);
EXPORT Message* Message_multiple_in(LindaValue ts, LindaValue t);

EXPORT void Message_free(Message* m);

void Linda_scanTuple2(LindaValue t, LindaValue ref, LindaValue** scanned);
static inline void Linda_scanTuple(LindaValue t, LindaValue ref) { Linda_scanTuple2(t, ref, NULL); }

struct Tuplequeue_t {
    struct Tuplequeue_t* next;
    LindaValue tuple;
};
typedef struct Tuplequeue_t* Tuplequeue;

#ifdef LINDA_SERVER
#ifdef USE_DOMAIN_SOCKETS
extern int Linda_udd;
#endif
LSIMPORT extern int Linda_sd;
#endif
extern int Linda_active_connections;

EXPORT void Linda_addTSReference(LindaValue ts);
EXPORT void Linda_delTSReference(LindaValue ts);

#ifdef __cplusplus
}
#endif

#endif
