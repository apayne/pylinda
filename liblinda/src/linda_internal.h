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

#ifndef LINDA_INTERNAL

#ifdef __cplusplus
extern "C" {
#endif

#define USE_DOMAIN_SOCKETS

struct MsgID_t {
    char* dest;
    char* source;
    int count;
};

typedef struct MsgID_t MsgID;
void MsgID_clear(MsgID* msgid);

struct Message_t {
     enum {
        DONE,
        DONT_KNOW,
        RESULT_STRING,
        RESULT_INT,
        RESULT_TUPLE,
        UNBLOCK,
        OUT,
        IN,
        RD,
        INP,
        RDP,
        COLLECT,
        COPY_COLLECT,
        CREATE_TUPLESPACE,
        ADD_REFERENCE,
        DELETE_REFERENCE,
/* Monitor messages */
        MONITOR,
        LIST_TS,
        INSPECT,
        GET_ROUTES,
/* Monitor messages */
        REGISTER_PROCESS,
        REGISTER_THREAD,
/* Server Messages */
        MY_NAME_IS,
        GET_NODE_ID,
        REGISTER_PARTITION,
        GET_PARTITIONS,
        DELETED_PARTITION,
        GET_REQUESTS,
        GET_NEIGHBOURS,
        GET_CONNECTION_DETAILS,
        TUPLE_REQUEST,
        CANCEL_REQUEST,
        MULTIPLE_IN
/* Server Messages */
     } type;
     MsgID* msgid;
     union {
         char* string;
         int i;
         MinimalValue tuple;
         Linda_tuplespace ts;
         struct {
             Linda_tuplespace ts;
             MinimalValue t;
         } out;
         struct {
             Linda_tuplespace ts;
             MinimalValue t;
             char* tid;
         } in;
         struct {
             Linda_tuplespace ts;
             MinimalValue t;
             char* tid;
         } rd;
         struct {
             Linda_tuplespace ts1;
             Linda_tuplespace ts2;
             MinimalValue t;
         } collect;
         struct {
            Linda_tuplespace ts;
            char* tid;
         } ref;
         struct {
            Linda_tuplespace ts;
            MinimalValue t;
         } tuple_request;
     };
};

typedef struct Message_t Message;

char* Message_getString(Message* msg);
void Message_send(int sd, MsgID* msgid, Message* msg);
Message* Message_recv(int s);
Message* Message_parse(char* text, int len, unsigned char final);

Message* Message_result_string(char* text);
Message* Message_result_int(int i);
Message* Message_result_tuple(LindaValue t);

Message* Message_done();
Message* Message_dont_know();

Message* Message_out(const Linda_tuplespace ts, LindaValue t);
Message* Message_in(const Linda_tuplespace ts, LindaValue t);
Message* Message_rd(const Linda_tuplespace ts, LindaValue t);
Message* Message_inp(const Linda_tuplespace ts, LindaValue t);
Message* Message_rdp(const Linda_tuplespace ts, LindaValue t);
Message* Message_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, LindaValue t);
Message* Message_copy_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, LindaValue t);
Message* Message_unblock();

Message* Message_createTuplespace();
Message* Message_addReference(const Linda_tuplespace ts);
Message* Message_addReference2(const Linda_tuplespace ts, char* id);
Message* Message_deleteReference(const Linda_tuplespace ts);

Message* Message_monitor();
Message* Message_list_ts();
Message* Message_inspect(const Linda_tuplespace ts);
Message* Message_get_routes();

Message* Message_register_process();
Message* Message_register_thread();

Message* Message_my_name_is(char* name);
Message* Message_get_node_id();

Message* Message_register_partition(const Linda_tuplespace ts, char* ref);
Message* Message_get_partitions(const Linda_tuplespace ts);
Message* Message_deleted_partition(const Linda_tuplespace ts, char* ref);

Message* Message_get_requests(const Linda_tuplespace ts);

Message* Message_get_neighbours();
Message* Message_get_connection_details(char* id);

Message* Message_tuple_request(const Linda_tuplespace ts, LindaValue t);
Message* Message_cancel_request(const Linda_tuplespace ts, LindaValue t);
Message* Message_multiple_in(const Linda_tuplespace ts, LindaValue t);

void Message_free(Message* m);

void Linda_scanTuple(LindaValue t, char* ref);

struct Tuplequeue_t {
    struct Tuplequeue_t* next;
    LindaValue tuple;
};
typedef struct Tuplequeue_t* Tuplequeue;

Tuplequeue Tuplequeue_push(Tuplequeue tq);
Tuplequeue Tuplequeue_pop(Tuplequeue tq);
LindaValue Tuplequeue_top(Tuplequeue tq);

struct Linda_thread_data_t {
    int sd;
    char* thread_id;
};
typedef struct Linda_thread_data_t Linda_thread_data;
Linda_thread_data* Linda_get_thread_data();

#ifdef LINDA_SERVER
#ifdef USE_DOMAIN_SOCKETS
extern int Linda_udd;
#endif
extern int Linda_sd;
#endif
extern int Linda_active_connections;

#ifdef __cplusplus
}
#endif

#endif
