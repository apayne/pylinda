#include <errno.h>
#include <string.h>
#include "stdio.h"

#include "linda_c.h"
#include "linda_internal.h"

/*
<msg>
<action>out</action>

<tuplespace
<tuple>
<element type="int">1</element>
<element type="string">abc</element>
</tuple>
</msg>
*/

char* Message_getTupleString(Tuple t);
char* Message_getElementString(Value* v);

char* Message_getString(Message* msg) {
    char* msgstr = NULL;
    char* msgid = NULL;
    char* action = NULL;
    char* body = NULL;
    char* tmp = NULL;
    Value v;

    if(msg->msgid) {
        int size = snprintf(NULL, 0, "%i", msg->msgid->count);
        msgid = (char*)malloc(37 + strlen(msg->msgid->source) + strlen(msg->msgid->dest) + size);
        sprintf(msgid, "<msgid source=\"%s\" dest=\"%s\" count=\"%i\" />", msg->msgid->source, msg->msgid->dest, msg->msgid->count);
    } else {
        msgid = (char*)malloc(1);
        msgid[0] = '\0';
    }

    switch(msg->type) {
    case DONE:
        action = "<action>done</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case DONT_KNOW:
        action = "<action>dont_know</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case RESULT_STRING:
        action = "<action>result_string</action>";
        v = Value_string(msg->string);
        body = Message_getElementString(&v);
        Value_clear(&v);
        break;
    case RESULT_INT:
        action = "<action>result_int</action>";
        v = Value_int(msg->i);
        body = Message_getElementString(&v);
        Value_clear(&v);
        break;
    case RESULT_TUPLE:
        action = "<action>result_tuple</action>";
        body = Message_getTupleString(msg->tuple);
        break;
    case UNBLOCK:
        action = "<action>unblock</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case OUT:
        action = "<action>out</action>";
        tmp = Message_getTupleString(msg->out.t);
        body = (char*)malloc(14 + strlen(msg->out.ts) + strlen(tmp));
        sprintf(body, "<ts id=\"%s\" />%s", msg->out.ts, tmp);
        break;
    case IN:
        action = "<action>in</action>";
        tmp = Message_getTupleString(msg->in.t);
        body = (char*)malloc(30 + strlen(tmp) + strlen(thread_id));
        sprintf(body, "<ts id=\"%s\" />%s<tid id=\"%s\" />", msg->in.ts, tmp, thread_id);
        break;
    case RD:
        action = "<action>rd</action>";
        tmp = Message_getTupleString(msg->rd.t);
        body = (char*)malloc(30 + strlen(tmp) + strlen(thread_id));
        sprintf(body, "<ts id=\"%s\" />%s<tid id=\"%s\" />", msg->rd.ts, tmp, thread_id);
        break;
    case INP:
        action = "<action>inp</action>";
        tmp = Message_getTupleString(msg->in.t);
        body = (char*)malloc(30 + strlen(tmp) + strlen(thread_id));
        sprintf(body, "<ts id=\"%s\" />%s<tid id=\"%s\" />", msg->in.ts, tmp, thread_id);
        break;
    case RDP:
        action = "<action>rdp</action>";
        tmp = Message_getTupleString(msg->rd.t);
        body = (char*)malloc(30 + strlen(tmp) + strlen(thread_id));
        sprintf(body, "<ts id=\"%s\" />%s<tid id=\"%s\" />", msg->rd.ts, tmp, thread_id);
        break;
    case COLLECT:
        action = "<action>collect</action>";
        tmp = Message_getTupleString(msg->out.t);
        body = (char*)malloc(26 + strlen(msg->collect.ts1) + strlen(msg->collect.ts2) + strlen(tmp));
        sprintf(body, "<ts id=\"%s\" /><ts id=\"%s\" />%s", msg->collect.ts1, msg->collect.ts2, tmp);
        break;
    case COPY_COLLECT:
        action = "<action>copy_collect</action>";
        tmp = Message_getTupleString(msg->out.t);
        body = (char*)malloc(26 + strlen(msg->collect.ts1) + strlen(msg->collect.ts2) + strlen(tmp));
        sprintf(body, "<ts id=\"%s\" /><ts id=\"%s\" />%s", msg->collect.ts1, msg->collect.ts2, tmp);
        break;
    case CREATE_TUPLESPACE:
        action = "<action>create_tuplespace</action>";
        body = (char*)malloc(18 + strlen(msg->string));
        sprintf(body, "<string>%s</string>", msg->string);
        break;
    case ADD_REFERENCE:
        action = "<action>add_reference</action>";
        body = (char*)malloc(32 + strlen(msg->ref.ts) + strlen(msg->ref.tid));
        sprintf(body, "<ts id=\"%s\" /><string>%s</string>", msg->ref.ts, msg->ref.tid);
        break;
    case DELETE_REFERENCE:
        action = "<action>delete_reference</action>";
        body = (char*)malloc(32 + strlen(msg->ref.ts) + strlen(msg->ref.tid));
        sprintf(body, "<ts id=\"%s\" /><string>%s</string>", msg->ref.ts, msg->ref.tid);
        break;
    case MONITOR:
        action = "<action>monitor</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case LIST_TS:
        action = "<action>list_ts</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case INSPECT:
        action = "<action>inspect</action>";
        body = (char*)malloc(14 + strlen(msg->ts));
        sprintf(body, "<ts id=\"%s\" />", msg->ts);
        break;
    case GET_ROUTES:
        action = "<action>get_routes</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case REGISTER_PROCESS:
        action = "<action>register_process</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case REGISTER_THREAD:
        action = "<action>register_thread</action>";
        body = (char*)malloc(18 + strlen(process_id));
        sprintf(body, "<string>%s</string>", process_id);
        break;
    case GET_NODE_ID:
        action = "<action>get_node_id</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case MY_NAME_IS:
        action = "<action>my_name_is</action>";
        body = (char*)malloc(18 + strlen(msg->string));
        sprintf(body, "<string>%s</string>", msg->string);
        break;
    case REGISTER_PARTITION:
        action = "<action>register_partition</action>";
        body = (char*)malloc(31 + strlen(msg->ref.ts) + strlen(msg->ref.tid));
        sprintf(body, "<ts id=\"%s\" /><string>%s</string>", msg->ref.ts, msg->ref.tid);
        break;
    case GET_PARTITIONS:
        action = "<action>get_partitions</action>";
        body = (char*)malloc(14 + strlen(msg->ref.ts));
        sprintf(body, "<ts id=\"%s\" />", msg->ref.ts);
        break;
    case DELETED_PARTITION:
        action = "<action>deleted_partition</action>";
        body = (char*)malloc(31 + strlen(msg->ref.ts) + strlen(msg->ref.tid));
        sprintf(body, "<ts id=\"%s\" /><string>%s</string>", msg->ref.ts, msg->ref.tid);
        break;
    case GET_REQUESTS:
        action = "<action>get_requests</action>";
        body = (char*)malloc(14 + strlen(msg->ref.ts));
        sprintf(body, "<ts id=\"%s\" />", msg->ref.ts);
        break;
    case GET_NEIGHBOURS:
        action = "<action>get_neighbours</action>";
        body = (char*)malloc(1);
        sprintf(body, "");
        break;
    case GET_CONNECTION_DETAILS:
        action = "<action>get_connection_details</action>";
        body = (char*)malloc(18 + strlen(msg->string));
        sprintf(body, "<string>%s</string>", msg->string);
        break;
    case TUPLE_REQUEST:
        action = "<action>tuple_request</action>";
        tmp = Message_getTupleString(msg->tuple_request.t);
        body = (char*)malloc(14 + strlen(msg->tuple_request.ts) + strlen(tmp));
        sprintf(body, "<ts id=\"%s\" />%s", msg->tuple_request.ts, tmp);
        break;
    default:
        fprintf(stderr, "Get String: Error, invalid message type.\n");
    }
    msgstr = (char*)malloc(4 + strlen(msgid) + strlen(action) + strlen(body) + 25);
    sprintf(msgstr, "<msg>%s%s<body>%s</body></msg>", msgid, action, body);
    free(body); free(tmp); free(msgid);
    return msgstr;
}

char* Message_getTupleString(Tuple t) {
    int i;
    char* tuplestr;
    char* elements = (char*)malloc(1);
    char* tmp;
    char* tmp2;
    elements[0] = '\0';
    for(i=0; i<t->size; i++) {
        tmp = Message_getElementString(Tuple_get(t, i));
        tmp2 = (char*)malloc(strlen(elements) + strlen(tmp) + 1);
        sprintf(tmp2, "%s%s", elements, tmp);
        free(elements); free(tmp);
        elements = tmp2;
    }
    tuplestr = (char*)malloc(strlen(elements) + 16);
    sprintf(tuplestr, "<tuple>%s</tuple>", elements);
    free(elements);
    return tuplestr;
}

char* Message_getElementString(Value* v) {
    int size;
    char* tmp;
    if(Value_is_bool(v)) {
        if(Value_get_bool(v)) {
            tmp = (char*)malloc(9);
            sprintf(tmp, "<true />");
        } else {
            tmp = (char*)malloc(10);
            sprintf(tmp, "<false />");
        }
    } else if(Value_is_int(v)) {
        size = snprintf(NULL, 0, "%i", Value_get_int(v));
        tmp = (char*)malloc(13 + size);
        sprintf(tmp, "<int>%i</int>", Value_get_int(v));
    } else if(Value_is_float(v)) {
        size = snprintf(NULL, 0, "%f", Value_get_float(v));
        tmp = (char*)malloc(17 + size);
        sprintf(tmp, "<float>%f</float", Value_get_float(v));
    } else if(Value_is_string(v)) {
        tmp = (char*)malloc(18 + Value_get_string_len(v));
        sprintf(tmp, "<string>%s</string>", Value_get_string(v));
    } else if(Value_is_tsref(v)) {
        tmp = (char*)malloc(13 + strlen(Value_get_tsref(v)));
        sprintf(tmp, "<ts id=\"%s\" />", Value_get_tsref(v));
    } else if(Value_is_tuple(v)) {
        tmp = Message_getTupleString(Value_get_tuple(v));
    } else {
        fprintf(stderr, "Error, invalid value type.\n");
        tmp = (char*)malloc(1);
        sprintf(tmp, "");
    }
    return tmp;
}

Message* Message_done() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = DONE;
    return m;
}

Message* Message_dont_know() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = DONT_KNOW;
    return m;
}

Message* Message_result_string(char* text) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RESULT_STRING;
    m->string = (char*)malloc(strlen(text)+1);
    strcpy(m->string, text);
    return m;
}

Message* Message_result_int(int i) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RESULT_INT;
    m->i = i;
    return m;
}

Message* Message_result_tuple(Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RESULT_TUPLE;
    m->tuple = Tuple_copy(t);
    return m;
}

Message* Message_out(const Linda_tuplespace ts, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = OUT;
    m->out.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->out.ts, ts);
    m->in.t = Tuple_copy(t);
    return m;
}

Message* Message_in(const Linda_tuplespace ts, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = IN;
    m->in.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->in.ts, ts);
    m->in.t = Tuple_copy(t);
    m->in.tid = (char*)malloc(strlen(thread_id)+1);
    strcpy(m->in.tid, thread_id);
    return m;
}

Message* Message_rd(const Linda_tuplespace ts, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RD;
    m->rd.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->rd.ts, ts);
    m->rd.t = Tuple_copy(t);
    m->rd.tid = (char*)malloc(strlen(thread_id)+1);
    strcpy(m->rd.tid, thread_id);
    return m;
}

Message* Message_inp(const Linda_tuplespace ts, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = INP;
    m->in.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->in.ts, ts);
    m->in.t = Tuple_copy(t);
    m->in.tid = (char*)malloc(strlen(thread_id)+1);
    strcpy(m->in.tid, thread_id);
    return m;
}

Message* Message_rdp(const Linda_tuplespace ts, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RDP;
    m->rd.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->rd.ts, ts);
    m->rd.t = Tuple_copy(t);
    m->rd.tid = (char*)malloc(strlen(thread_id)+1);
    strcpy(m->rd.tid, thread_id);
    return m;
}

Message* Message_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = COLLECT;
    m->collect.ts1 = (Linda_tuplespace)malloc(strlen(ts1) + 1);
    strcpy(m->collect.ts1, ts1);
    m->collect.ts2 = (Linda_tuplespace)malloc(strlen(ts2) + 1);
    strcpy(m->collect.ts2, ts2);
    m->collect.t = Tuple_copy(t);
    return m;
}

Message* Message_copy_collect(const Linda_tuplespace ts1, const Linda_tuplespace ts2, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = COPY_COLLECT;
    m->collect.ts1 = (Linda_tuplespace)malloc(strlen(ts1) + 1);
    strcpy(m->collect.ts1, ts1);
    m->collect.ts2 = (Linda_tuplespace)malloc(strlen(ts2) + 1);
    strcpy(m->collect.ts2, ts2);
    m->collect.t = Tuple_copy(t);
    return m;
}

Message* Message_createTuplespace() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = CREATE_TUPLESPACE;
    m->string = (char*)malloc(strlen(thread_id)+1);
    strcpy(m->string, thread_id);
    return m;
}

Message* Message_addReference(const Linda_tuplespace ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = ADD_REFERENCE;
    m->ref.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->ref.ts, ts);
    m->ref.tid = (char*)malloc(strlen(thread_id) + 1);
    strcpy(m->ref.tid, thread_id);
    return m;
}

Message* Message_addReference2(const Linda_tuplespace ts, char* id) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = ADD_REFERENCE;
    m->ref.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->ref.ts, ts);
    m->ref.tid = (char*)malloc(strlen(id) + 1);
    strcpy(m->ref.tid, id);
    return m;
}

Message* Message_deleteReference(const Linda_tuplespace ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = DELETE_REFERENCE;
    m->ref.ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->ref.ts, ts);
    m->ref.tid = (char*)malloc(strlen(thread_id) + 1);
    strcpy(m->ref.tid, thread_id);
    return m;
}

Message* Message_monitor() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = MONITOR;
    return m;
}

Message* Message_list_ts() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = LIST_TS;
    return m;
}

Message* Message_inspect(const Linda_tuplespace ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = INSPECT;
    m->ts = (Linda_tuplespace)malloc(strlen(ts) + 1);
    strcpy(m->ts, ts);
    return m;
}

Message* Message_get_routes() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_ROUTES;
    return m;
}

Message* Message_register_process() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = REGISTER_PROCESS;
    return m;
}

Message* Message_register_thread() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = REGISTER_THREAD;
    m->string = (char*)malloc(strlen(process_id)+1);
    strcpy(m->string, process_id);
    return m;
}

Message* Message_my_name_is(char* name) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = MY_NAME_IS;
    m->string = (char*)malloc(strlen(name)+1);
    strcpy(m->string, name);
    return m;
}

Message* Message_get_node_id() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_NODE_ID;
    return m;
}

Message* Message_register_partition(const Linda_tuplespace ts, char* ref) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = REGISTER_PARTITION;
    m->ref.ts = (char*)malloc(strlen(ts)+1);
    strcpy(m->ref.ts, ts);
    m->ref.tid = (char*)malloc(strlen(ref)+1);
    strcpy(m->ref.tid, ref);
    return m;
}

Message* Message_get_partitions(const Linda_tuplespace ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_PARTITIONS;
    m->ref.ts = (char*)malloc(strlen(ts)+1);
    strcpy(m->ts, ts);
    return m;
}

Message* Message_deleted_partition(const Linda_tuplespace ts, char* ref) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = DELETED_PARTITION;
    m->ref.ts = (char*)malloc(strlen(ts)+1);
    strcpy(m->ref.ts, ts);
    m->ref.tid = (char*)malloc(strlen(ref)+1);
    strcpy(m->ref.tid, ref);
    return m;
}

Message* Message_get_requests(const Linda_tuplespace ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_REQUESTS;
    m->ref.ts = (char*)malloc(strlen(ts)+1);
    strcpy(m->ref.ts, ts);
    return m;
}

Message* Message_get_neighbours() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_NEIGHBOURS;
    return m;
}

Message* Message_get_connection_details(char* id) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_CONNECTION_DETAILS;
    m->string = (char*)malloc(strlen(id)+1);
    strcpy(m->string, id);
    return m;
}

Message* Message_tuple_request(const Linda_tuplespace ts, Tuple t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = TUPLE_REQUEST;
    m->tuple_request.ts = (char*)malloc(strlen(ts)+1);
    strcpy(m->tuple_request.ts, ts);
    m->tuple_request.t = Tuple_copy(t);
    return m;
}

void Message_free(Message* msg) {
    if(msg != NULL) {
    if(msg->msgid != NULL) {
        free(msg->msgid->source); free(msg->msgid->dest); free(msg->msgid);
    }
    switch(msg->type) {
    case DONE:
        break;
    case RESULT_STRING:
        free(msg->string);
        break;
    case RESULT_INT:
        break;
    case RESULT_TUPLE:
        Tuple_free(msg->tuple);
        break;
    case UNBLOCK:
        break;
    case OUT:
        free(msg->out.ts);
        Tuple_free(msg->out.t);
        break;
    case RD:
    case RDP:
        free(msg->rd.tid);
        free(msg->rd.ts);
        Tuple_free(msg->rd.t);
        break;
    case IN:
    case INP:
        free(msg->in.tid);
        free(msg->in.ts);
        Tuple_free(msg->in.t);
        break;
    case COLLECT:
    case COPY_COLLECT:
        free(msg->collect.ts1);
        free(msg->collect.ts2);
        Tuple_free(msg->collect.t);
        break;
    case CREATE_TUPLESPACE:
        free(msg->string);
        break;
    case ADD_REFERENCE:
        free(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case DELETE_REFERENCE:
        free(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case MONITOR:
    case GET_ROUTES:
        break;
    case REGISTER_PROCESS:
        break;
    case REGISTER_THREAD:
        free(msg->string);
        break;
    case MY_NAME_IS:
        free(msg->string);
        break;
    case GET_NODE_ID:
        break;
    case REGISTER_PARTITION:
    case DELETED_PARTITION:
        free(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case GET_PARTITIONS:
    case GET_REQUESTS:
        free(msg->ref.ts);
        break;
    case GET_NEIGHBOURS:
        break;
    case GET_CONNECTION_DETAILS:
        free(msg->string);
        break;
    case TUPLE_REQUEST:
        free(msg->tuple_request.ts);
        Tuple_free(msg->tuple_request.t);
        break;
    default:
        fprintf(stderr, "Invalid message free (%i).\n", msg->type);
    }
    free(msg);
    }
}

void Message_send(int sd, MsgID* msgid, Message* msg) {
    msg->msgid = msgid;
    char* msgstr = Message_getString(msg);
    int len = strlen(msgstr);
    int sent = 0;

    char msglen[4];
    *((int*)msglen) = htonl(len);

    while(sent < 4) {
        sent += send(sd, &(msglen[sent]), 4-sent, 0);
        if(sent < 0) {
            fprintf(stderr, "Error sending message %s to %i (%s).\n", msgstr, sd, strerror(errno));
            free(msgstr);
            return;
        }
    }
    sent = 0;
    while(sent < len) {
        sent += send(sd, &(msgstr[sent]), len-sent, 0);
    }
    free(msgstr);
}
