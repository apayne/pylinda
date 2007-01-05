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

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include "linda_c.h"
#include "linda_internal.h"

#ifdef TYPES
#define INCLUDE_TYPES 1
#else
#define INCLUDE_TYPES 0
#endif

char* Message_getString(Message* msg) {
    LindaValue v;

    xmlDocPtr doc = xmlNewDoc(NULL);
    xmlNodePtr root = xmlNewDocNode(doc, NULL, (xmlChar*)"linda", NULL);
    xmlDocSetRootElement(doc, root);

    if(msg->msgid) {
        xmlNodePtr msgid = xmlNewDocNode(doc, NULL, (xmlChar*)"msgid", NULL);
        xmlAddChild(root, msgid);
        char* count_str = (char*)malloc(snprintf(NULL, 0, "%i", msg->msgid->count) + 1);
        sprintf(count_str, "%i", msg->msgid->count);
        xmlNewProp(msgid, (xmlChar*)"source", (xmlChar*)msg->msgid->source);
        xmlNewProp(msgid, (xmlChar*)"dest", (xmlChar*)msg->msgid->dest);
        xmlNewProp(msgid, (xmlChar*)"count", (xmlChar*)count_str);
        free(count_str);
    }

    switch(msg->type) {
    case DONE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"done");
        /*xmlAddChild(root, action);*/
        }
        break;
    case DONT_KNOW:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"dont_know");
        /*xmlAddChild(root, action);*/
        break;
    case RESULT_STRING:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_string");
        v = Linda_string(msg->string);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
    case RESULT_INT:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_int");
        v = Linda_int(msg->i);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
    case RESULT_TUPLE:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_tuple");
        Minimal_serialiseXML(doc, root, msg->tuple, INCLUDE_TYPES);
        break;
    case UNBLOCK:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"unblock");
        break;
    case OUT:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"out");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->out.ts));
        Minimal_serialiseXML(doc, root, msg->out.t, INCLUDE_TYPES);
        break;
        }
    case IN:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"in");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->in.ts));

        Minimal_serialiseXML(doc, root, msg->in.t, INCLUDE_TYPES);

        xmlNodePtr tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->in.tid);
        break;
        }
    case RD:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"rd");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->rd.ts));

        Minimal_serialiseXML(doc, root, msg->rd.t, INCLUDE_TYPES);

        xmlNodePtr tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->rd.tid);
        break;
        }
    case INP:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"inp");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->in.ts));

        Minimal_serialiseXML(doc, root, msg->in.t, INCLUDE_TYPES);

        xmlNodePtr tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->in.tid);
        break;
        }
    case RDP:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"rdp");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->rd.ts));

        Minimal_serialiseXML(doc, root, msg->rd.t, INCLUDE_TYPES);

        xmlNodePtr tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->rd.tid);
        break;
        }
    case COLLECT:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"collect");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts1));
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts2));

        Minimal_serialiseXML(doc, root, msg->collect.t, INCLUDE_TYPES);
        break;
        }
    case COPY_COLLECT:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"copy_collect");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts1));
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts2));

        Minimal_serialiseXML(doc, root, msg->collect.t, INCLUDE_TYPES);
        break;
        }
    case CREATE_TUPLESPACE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"create_tuplespace");
        v = Linda_string(msg->string);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
        }
    case ADD_REFERENCE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"add_reference");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ref.ts));
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
        }
    case DELETE_REFERENCE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"delete_reference");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ref.ts));
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
        }
    case MONITOR:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"monitor");
        break;
    case LIST_TS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"list_ts");
        break;
    case INSPECT:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"inspect");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ts));
        break;
        }
    case GET_ROUTES:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_routes");
        break;
    case REGISTER_PROCESS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_process");
        break;
    case REGISTER_THREAD:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_thread");
        v = Linda_string(Linda_process_id);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
#ifdef REGISTER_TYPES
    case REGISTER_TYPE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_type");
        xmlNodePtr e = xmlNewDocNode(doc, NULL, (xmlChar*)"element", NULL);
        xmlAddChild(root, e);
        Minimal_serialiseXML(doc, e, msg->typestruct.typeobj, INCLUDE_TYPES);
        break;
        }
    case UPDATE_TYPE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"update_type");
        v = Linda_int(msg->i);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        xmlNodePtr e = xmlNewDocNode(doc, NULL, (xmlChar*)"element", NULL);
        xmlAddChild(root, e);
        Minimal_serialiseXML(doc, e, msg->typestruct.typeobj, INCLUDE_TYPES);
        break;
        }
#endif
    case GET_NODE_ID:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_node_id");
        break;
    case MY_NAME_IS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"my_name_is");
        v = Linda_string(Linda_process_id);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
    case REGISTER_PARTITION:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_partition");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ref.ts));
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
        }
    case GET_PARTITIONS:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_partitions");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ref.ts));
        break;
        }
    case DELETED_PARTITION:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"deleted_partition");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ref.ts));
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
        }
    case GET_REQUESTS:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_requests");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->ref.ts));
        break;
        }
    case GET_NEIGHBOURS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_neighbours");
        break;
    case GET_CONNECTION_DETAILS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_connection_details");
        v = Linda_string(msg->string);
        Minimal_serialiseXML(doc, root, v, 0);
        Linda_delReference(v);
        break;
    case TUPLE_REQUEST:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"tuple_request");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->tuple_request.ts));

        Minimal_serialiseXML(doc, root, msg->tuple_request.t, 0);
        break;
        }
    case CANCEL_REQUEST:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"cancel_request");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->tuple_request.ts));

        Minimal_serialiseXML(doc, root, msg->tuple_request.t, 0);
        break;
        }
    case MULTIPLE_IN:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"multiple_in");
        xmlNodePtr ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->tuple_request.ts));

        Minimal_serialiseXML(doc, root, msg->tuple_request.t, 0);
        }
        break;
    default:
        fprintf(stderr, "Get String: Error, invalid message type.\n");
    }

    int size;
    xmlChar* buf;
    xmlDocDumpFormatMemory(doc, &buf, &size, 1);

    char* outbuf = (char*)malloc(size+1);
    memcpy(outbuf, buf, size);
    outbuf[size] = '\0';

    xmlFreeDoc(doc);
    xmlFree(buf);

    return outbuf;
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

Message* Message_result_tuple(LindaValue t) {
    if(!Linda_isTuple(t)) {
        fprintf(stderr, "PyLinda: Error, Message_result_tuple not passed a tuple.\n");
        return NULL;
    }
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RESULT_TUPLE;
    m->tuple = Linda_copy(t);
    return m;
}

Message* Message_out(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = OUT;
    m->out.ts = Linda_copy(ts);
    m->out.t = Linda_copy(t);
    return m;
}

Message* Message_in(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = IN;
    m->in.ts = Linda_copy(ts);
    m->in.t = Linda_copy(t);
    m->in.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->in.tid, tdata->thread_id);
    return m;
}

Message* Message_rd(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RD;
    m->rd.ts = Linda_copy(ts);
    m->rd.t = Linda_copy(t);
    m->rd.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->rd.tid, tdata->thread_id);
    return m;
}

Message* Message_inp(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = INP;
    m->in.ts = Linda_copy(ts);
    m->in.t = Linda_copy(t);
    m->in.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->in.tid, tdata->thread_id);
    return m;
}

Message* Message_rdp(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = RDP;
    m->rd.ts = Linda_copy(ts);
    m->rd.t = Linda_copy(t);
    m->rd.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->rd.tid, tdata->thread_id);
    return m;
}

Message* Message_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = COLLECT;
    m->collect.ts1 = Linda_copy(ts1);
    m->collect.ts2 = Linda_copy(ts2);
    m->collect.t = Linda_copy(t);
    return m;
}

Message* Message_copy_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = COPY_COLLECT;
    m->collect.ts1 = Linda_copy(ts1);
    m->collect.ts2 = Linda_copy(ts2);
    m->collect.t = Linda_copy(t);
    return m;
}

Message* Message_unblock() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = UNBLOCK;
    return m;
}

Message* Message_createTuplespace() {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = CREATE_TUPLESPACE;
    m->string = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->string, tdata->thread_id);
    return m;
}

Message* Message_addReference(LindaValue ts) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = ADD_REFERENCE;
    Linda_addReference(ts);
    m->ref.ts = ts;
    m->ref.tid = (char*)malloc(strlen(tdata->thread_id) + 1);
    strcpy(m->ref.tid, tdata->thread_id);
    return m;
}

Message* Message_addReference2(LindaValue ts, char* id) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = ADD_REFERENCE;
    Linda_addReference(ts);
    m->ref.ts = ts;
    m->ref.tid = (char*)malloc(strlen(id) + 1);
    strcpy(m->ref.tid, id);
    return m;
}

Message* Message_deleteReference(LindaValue ts) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = DELETE_REFERENCE;
    Linda_addReference(ts);
    m->ref.ts = ts;
    m->ref.tid = (char*)malloc(strlen(tdata->thread_id) + 1);
    strcpy(m->ref.tid, tdata->thread_id);
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

Message* Message_inspect(LindaValue ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = INSPECT;
    m->ts = Linda_copy(ts);
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
    m->string = (char*)malloc(strlen(Linda_process_id)+1);
    strcpy(m->string, Linda_process_id);
    return m;
}

Message* Message_register_type(LindaValue type) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = REGISTER_TYPE;
    Linda_addReference(type);
    m->typestruct.typeobj = type;
    return m;
}

Message* Message_update_type(int type_id, LindaValue type) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = UPDATE_TYPE;
    m->typestruct.type_id = type_id;
    Linda_addReference(type);
    m->typestruct.typeobj = type;
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

Message* Message_register_partition(LindaValue ts, char* ref) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = REGISTER_PARTITION;
    m->ref.ts = Linda_copy(m->ref.ts);
    m->ref.tid = (char*)malloc(strlen(ref)+1);
    strcpy(m->ref.tid, ref);
    return m;
}

Message* Message_get_partitions(LindaValue ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_PARTITIONS;
    m->ref.ts = Linda_copy(m->ts);
    return m;
}

Message* Message_deleted_partition(LindaValue ts, char* ref) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = DELETED_PARTITION;
    m->ref.ts = Linda_copy(m->ref.ts);
    m->ref.tid = (char*)malloc(strlen(ref)+1);
    strcpy(m->ref.tid, ref);
    return m;
}

Message* Message_get_requests(LindaValue ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = GET_REQUESTS;
    m->ref.ts = Linda_copy(m->ref.ts);
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

Message* Message_tuple_request(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = TUPLE_REQUEST;
    m->tuple_request.ts = Linda_copy(m->tuple_request.ts);
    m->tuple_request.t = Linda_copy(t);
    return m;
}

Message* Message_cancel_request(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = CANCEL_REQUEST;
    m->tuple_request.ts = Linda_copy(m->tuple_request.ts);
    m->tuple_request.t = Linda_copy(t);
    return m;
}

Message* Message_multiple_in(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = MULTIPLE_IN;
    m->tuple_request.ts = Linda_copy(m->tuple_request.ts);
    m->tuple_request.t = Linda_copy(t);
    return m;
}

void Message_free(Message* msg) {
    if(msg != NULL) {
    if(msg->msgid != NULL) {
        free(msg->msgid->source); free(msg->msgid->dest); free(msg->msgid);
    }
    switch(msg->type) {
    case DONE:
    case DONT_KNOW:
        break;
    case RESULT_STRING:
        free(msg->string);
        break;
    case RESULT_INT:
        break;
    case RESULT_TUPLE:
        Linda_delReference(msg->tuple);
        break;
    case UNBLOCK:
        break;
    case OUT:
        Linda_delReference(msg->out.ts);
        Linda_delReference(msg->out.t);
        break;
    case RD:
    case RDP:
        free(msg->rd.tid);
        Linda_delReference(msg->rd.ts);
        Linda_delReference(msg->rd.t);
        break;
    case IN:
    case INP:
        free(msg->in.tid);
        Linda_delReference(msg->in.ts);
        Linda_delReference(msg->in.t);
        break;
    case COLLECT:
    case COPY_COLLECT:
        Linda_delReference(msg->collect.ts1);
        Linda_delReference(msg->collect.ts2);
        Linda_delReference(msg->collect.t);
        break;
    case CREATE_TUPLESPACE:
        free(msg->string);
        break;
    case ADD_REFERENCE:
        Linda_delReference(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case DELETE_REFERENCE:
        Linda_delReference(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case MONITOR:
    case LIST_TS:
    case GET_ROUTES:
        break;
    case REGISTER_PROCESS:
        break;
    case REGISTER_THREAD:
        free(msg->string);
        break;
    case REGISTER_TYPE:
        Linda_delReference(msg->typestruct.typeobj);
        break;
    case UPDATE_TYPE:
        Linda_delReference(msg->typestruct.typeobj);
        break;
    case MY_NAME_IS:
        free(msg->string);
        break;
    case GET_NODE_ID:
        break;
    case REGISTER_PARTITION:
    case DELETED_PARTITION:
        Linda_delReference(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case GET_PARTITIONS:
    case GET_REQUESTS:
        Linda_delReference(msg->ref.ts);
        break;
    case INSPECT:
        Linda_delReference(msg->ts);
        break;
    case GET_NEIGHBOURS:
        break;
    case GET_CONNECTION_DETAILS:
        free(msg->string);
        break;
    case TUPLE_REQUEST:
    case CANCEL_REQUEST:
        Linda_delReference(msg->tuple_request.ts);
        Linda_delReference(msg->tuple_request.t);
        break;
    case MULTIPLE_IN:
        Linda_delReference(msg->tuple_request.ts);
        Linda_delReference(msg->tuple_request.t);
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
