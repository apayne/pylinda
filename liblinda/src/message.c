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
#include <unistd.h>

#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include "linda_internal.h"

#ifdef WIN32
#define snprintf _snprintf
#include "winsock2.h"
#else
#include <arpa/inet.h>
#endif

#ifdef TYPES
#define INCLUDE_TYPES 1
#else
#define INCLUDE_TYPES 0
#endif

char* Message_getString(Message* msg) {
    LindaValue v;
    int size;
    xmlChar* buf;
    char* outbuf;

    xmlDocPtr doc = xmlNewDoc(NULL);
    xmlNodePtr root = xmlNewDocNode(doc, NULL, (xmlChar*)"linda", NULL);
    xmlDocSetRootElement(doc, root);

    if(msg->msgid) {
        char* count_str;
        xmlNodePtr msgid = xmlNewDocNode(doc, NULL, (xmlChar*)"msgid", NULL);
        xmlAddChild(root, msgid);
        count_str = (char*)malloc(snprintf(NULL, 0, "%i", msg->msgid->count) + 1);
        sprintf(count_str, "%i", msg->msgid->count);
        xmlNewProp(msgid, (xmlChar*)"source", (xmlChar*)msg->msgid->source);
        xmlNewProp(msgid, (xmlChar*)"dest", (xmlChar*)msg->msgid->dest);
        xmlNewProp(msgid, (xmlChar*)"count", (xmlChar*)count_str);
        free(count_str);
    }

    switch(msg->type) {
    case L_DONE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"done");
        /*xmlAddChild(root, action);*/
        }
        break;
    case L_DONT_KNOW:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"dont_know");
        /*xmlAddChild(root, action);*/
        break;
    case L_RESULT_STRING:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_string");
        v = Linda_string(msg->string);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
    case L_RESULT_INT:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_int");
        v = Linda_int(msg->i);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
    case L_RESULT_TUPLE:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_tuple");
        Minimal_serialiseXML(doc, root, msg->tuple, INCLUDE_TYPES, 0);
        break;
    case L_UNBLOCK:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"unblock");
        break;
    case L_OUT:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"out");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->out.ts));
        Minimal_serialiseXML(doc, root, msg->out.t, INCLUDE_TYPES, 0);
        break;
        }
    case L_IN:
        {
        xmlNodePtr tid;
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"in");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->in.ts));

        Minimal_serialiseXML(doc, root, msg->in.t, INCLUDE_TYPES, 0);

        tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->in.tid);
        break;
        }
    case L_RD:
        {
        xmlNodePtr tid;
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"rd");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->rd.ts));

        Minimal_serialiseXML(doc, root, msg->rd.t, INCLUDE_TYPES, 0);

        tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->rd.tid);
        break;
        }
    case L_INP:
        {
        xmlNodePtr tid;
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"inp");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->in.ts));

        Minimal_serialiseXML(doc, root, msg->in.t, INCLUDE_TYPES, 0);

        tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->in.tid);
        break;
        }
    case L_RDP:
        {
        xmlNodePtr ts;
        xmlNodePtr tid;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"rdp");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->rd.ts));

        Minimal_serialiseXML(doc, root, msg->rd.t, INCLUDE_TYPES, 0);

        tid = xmlNewDocNode(doc, NULL, (xmlChar*)"tid", NULL);
        xmlAddChild(root, tid);
        xmlNewProp(tid, (xmlChar*)"id", (xmlChar*)msg->rd.tid);
        break;
        }
    case L_COLLECT:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"collect");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts1));
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts2));

        Minimal_serialiseXML(doc, root, msg->collect.t, INCLUDE_TYPES, 0);
        break;
        }
    case L_COPY_COLLECT:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"copy_collect");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts1));
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->collect.ts2));

        Minimal_serialiseXML(doc, root, msg->collect.t, INCLUDE_TYPES, 0);
        break;
        }
    case L_CREATE_TUPLESPACE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"create_tuplespace");
        v = Linda_string(msg->string);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_ADD_REFERENCE:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"add_reference");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ref.ts);
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_DELETE_REFERENCE:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"delete_reference");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ref.ts);
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_MONITOR:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"monitor");
        break;
    case L_LIST_TS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"list_ts");
        break;
    case L_INSPECT:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"inspect");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ts);
        break;
        }
    case L_GET_ROUTES:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_routes");
        break;
    case L_REGISTER_PROCESS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_process");
        break;
    case L_REGISTER_THREAD:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_thread");
        v = Linda_string(Linda_process_id);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
    case L_REGISTER_TYPE:
        {
        xmlNodePtr e;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_type");
        e = xmlNewDocNode(doc, NULL, (xmlChar*)"element", NULL);
        xmlAddChild(root, e);
        Minimal_serialiseXML(doc, e, msg->typestruct.typeobj, INCLUDE_TYPES, 0);
        v = Linda_string(Linda_process_id);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_UPDATE_TYPE:
        {
        xmlNodePtr e;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"update_type");
        v = Linda_string(msg->typestruct.type_id);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        e = xmlNewDocNode(doc, NULL, (xmlChar*)"element", NULL);
        xmlAddChild(root, e);
        Minimal_serialiseXML(doc, e, msg->typestruct.typeobj, INCLUDE_TYPES, 1);
        v = Linda_string(Linda_process_id);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_RESULT_TYPE:
        {
        xmlNodePtr e;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_type");
        e = xmlNewDocNode(doc, NULL, (xmlChar*)"element", NULL);
        xmlAddChild(root, e);
        Minimal_serialiseXML(doc, e, msg->typestruct.typeobj, INCLUDE_TYPES, 0);
        break;
        }
    case L_REQUEST_TYPE:
        {
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"result_type");
        v = Linda_string(msg->typestruct.type_id);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_GET_NODE_ID:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_node_id");
        break;
    case L_MY_NAME_IS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"my_name_is");
        v = Linda_string(Linda_process_id);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
    case L_REGISTER_PARTITION:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"register_partition");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ref.ts);
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_GET_PARTITIONS:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_partitions");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ref.ts);
        break;
        }
    case L_DELETED_PARTITION:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"deleted_partition");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ref.ts);
        v = Linda_string(msg->ref.tid);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
        }
    case L_GET_REQUESTS:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_requests");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)msg->ref.ts);
        break;
        }
    case L_GET_NEIGHBOURS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_neighbours");
        break;
    case L_GET_CONNECTION_DETAILS:
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"get_connection_details");
        v = Linda_string(msg->string);
        Minimal_serialiseXML(doc, root, v, 0, 0);
        Linda_delReference(v);
        break;
    case L_TUPLE_REQUEST:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"tuple_request");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->tuple_request.ts));

        Minimal_serialiseXML(doc, root, msg->tuple_request.t, 0, 0);
        break;
        }
    case L_CANCEL_REQUEST:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"cancel_request");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->tuple_request.ts));

        Minimal_serialiseXML(doc, root, msg->tuple_request.t, 0, 0);
        break;
        }
    case L_MULTIPLE_IN:
        {
        xmlNodePtr ts;
        xmlNewTextChild(root, NULL, (xmlChar*)"action", (xmlChar*)"multiple_in");
        ts = xmlNewDocNode(doc, NULL, (xmlChar*)"ts", NULL);
        xmlAddChild(root, ts);
        xmlNewProp(ts, (xmlChar*)"id", (xmlChar*)Minimal_getTupleSpace(msg->tuple_request.ts));

        Minimal_serialiseXML(doc, root, msg->tuple_request.t, 0, 0);
        }
        break;
    }

    xmlDocDumpFormatMemory(doc, &buf, &size, 1);

    outbuf = (char*)malloc(size+1);
    memcpy(outbuf, buf, size);
    outbuf[size] = '\0';

    xmlFreeDoc(doc);
    xmlFree(buf);

    return outbuf;
}

Message* Message_done() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_DONE;
    return m;
}

Message* Message_dont_know() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_DONT_KNOW;
    return m;
}

Message* Message_result_string(char* text) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_RESULT_STRING;
    m->string = (char*)malloc(strlen(text)+1);
    strcpy(m->string, text);
    return m;
}

Message* Message_result_int(int i) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_RESULT_INT;
    m->i = i;
    return m;
}

Message* Message_result_tuple(LindaValue t) {
    Message* m;
    if(!Linda_isTuple(t)) {
        fprintf(stderr, "PyLinda: Error, Message_result_tuple not passed a tuple.\n");
        return NULL;
    }
    m = (Message*)malloc(sizeof(Message));
    m->type = L_RESULT_TUPLE;
    Linda_addReference(t);
    m->tuple = t;
    return m;
}

Message* Message_result_type(LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_RESULT_TYPE;
    Linda_addReference(t);
    m->typestruct.typeobj = t;
    return m;
}

Message* Message_out(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_OUT;
    Linda_addReference(ts);
    m->out.ts = ts;
    Linda_addReference(t);
    m->out.t = t;
    return m;
}

Message* Message_in(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_IN;
    Linda_addReference(ts);
    m->in.ts = ts;
    Linda_addReference(t);
    m->in.t = t;
    m->in.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->in.tid, tdata->thread_id);
    return m;
}

Message* Message_rd(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_RD;
    Linda_addReference(ts);
    m->rd.ts = ts;
    Linda_addReference(t);
    m->rd.t = t;
    m->rd.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->rd.tid, tdata->thread_id);
    return m;
}

Message* Message_inp(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_INP;
    Linda_addReference(ts);
    m->in.ts = ts;
    Linda_addReference(t);
    m->in.t = t;
    m->in.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->in.tid, tdata->thread_id);
    return m;
}

Message* Message_rdp(LindaValue ts, LindaValue t) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_RDP;
    Linda_addReference(ts);
    m->rd.ts = ts;
    Linda_addReference(t);
    m->rd.t = t;
    m->rd.tid = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->rd.tid, tdata->thread_id);
    return m;
}

Message* Message_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_COLLECT;
    Linda_addReference(ts1);
    Linda_addReference(ts2);
    m->collect.ts1 = ts1;
    m->collect.ts2 = ts2;
    Linda_addReference(t);
    m->collect.t = t;
    return m;
}

Message* Message_copy_collect(LindaValue ts1, LindaValue ts2, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_COPY_COLLECT;
    Linda_addReference(ts1);
    Linda_addReference(ts2);
    m->collect.ts1 = ts1;
    m->collect.ts2 = ts2;
    Linda_addReference(t);
    m->collect.t = t;
    return m;
}

Message* Message_unblock() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_UNBLOCK;
    return m;
}

Message* Message_createTuplespace() {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_CREATE_TUPLESPACE;
    m->string = (char*)malloc(strlen(tdata->thread_id)+1);
    strcpy(m->string, tdata->thread_id);
    return m;
}

Message* Message_addReference(LindaValue ts) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_ADD_REFERENCE;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    m->ref.tid = (char*)malloc(strlen(tdata->thread_id) + 1);
    strcpy(m->ref.tid, tdata->thread_id);
    return m;
}

Message* Message_addReference2(LindaValue ts, char* id) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_ADD_REFERENCE;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    m->ref.tid = (char*)malloc(strlen(id) + 1);
    strcpy(m->ref.tid, id);
    return m;
}

Message* Message_deleteReference(LindaValue ts) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_DELETE_REFERENCE;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    m->ref.tid = (char*)malloc(strlen(tdata->thread_id) + 1);
    strcpy(m->ref.tid, tdata->thread_id);
    return m;
}

Message* Message_monitor() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_MONITOR;
    return m;
}

Message* Message_list_ts() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_LIST_TS;
    return m;
}

Message* Message_inspect(char* ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_INSPECT;
    m->string = (char*)malloc(strlen(ts) + 1);
    strcpy(m->string, ts);
    return m;
}

Message* Message_get_routes() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_GET_ROUTES;
    return m;
}

Message* Message_register_process() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_REGISTER_PROCESS;
    return m;
}

Message* Message_register_thread() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_REGISTER_THREAD;
    m->string = (char*)malloc(strlen(Linda_process_id)+1);
    strcpy(m->string, Linda_process_id);
    return m;
}

Message* Message_register_type(LindaValue type) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_REGISTER_TYPE;
    Linda_addReference(type);
    m->typestruct.typeobj = type;
    m->typestruct.pid = NULL;
    return m;
}

Message* Message_update_type(char* type_id, LindaValue type) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_UPDATE_TYPE;
    m->typestruct.type_id = malloc(strlen(type_id) + 1);
    strcpy(m->typestruct.type_id, type_id);
    Linda_addReference(type);
    m->typestruct.typeobj = type;
    m->typestruct.pid = NULL;
    return m;
}

Message* Message_request_type(char* type_id) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_REQUEST_TYPE;
    m->typestruct.type_id = malloc(strlen(type_id) + 1);
    strcpy(m->typestruct.type_id, type_id);
    return m;
}

Message* Message_my_name_is(char* name) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_MY_NAME_IS;
    m->string = (char*)malloc(strlen(name)+1);
    strcpy(m->string, name);
    return m;
}

Message* Message_get_node_id() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_GET_NODE_ID;
    return m;
}

Message* Message_register_partition(LindaValue ts, char* ref) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_REGISTER_PARTITION;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    m->ref.tid = (char*)malloc(strlen(ref)+1);
    strcpy(m->ref.tid, ref);
    return m;
}

Message* Message_get_partitions(LindaValue ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_GET_PARTITIONS;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    return m;
}

Message* Message_deleted_partition(LindaValue ts, char* ref) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_DELETED_PARTITION;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    m->ref.tid = (char*)malloc(strlen(ref)+1);
    strcpy(m->ref.tid, ref);
    return m;
}

Message* Message_get_requests(LindaValue ts) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_GET_REQUESTS;
    m->ref.ts = malloc(strlen(Minimal_getTupleSpace(ts)) + 1);
    strcpy(m->ref.ts, Minimal_getTupleSpace(ts));
    return m;
}

Message* Message_get_neighbours() {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_GET_NEIGHBOURS;
    return m;
}

Message* Message_get_connection_details(char* id) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_GET_CONNECTION_DETAILS;
    m->string = (char*)malloc(strlen(id)+1);
    strcpy(m->string, id);
    return m;
}

Message* Message_tuple_request(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_TUPLE_REQUEST;
    Linda_addReference(ts);
    m->tuple_request.ts = ts;
    Linda_addReference(t);
    m->tuple_request.t = t;
    return m;
}

Message* Message_cancel_request(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_CANCEL_REQUEST;
    Linda_addReference(ts);
    m->tuple_request.ts = ts;
    Linda_addReference(t);
    m->tuple_request.t = t;
    return m;
}

Message* Message_multiple_in(LindaValue ts, LindaValue t) {
    Message* m = (Message*)malloc(sizeof(Message));
    m->type = L_MULTIPLE_IN;
    Linda_addReference(ts);
    m->tuple_request.ts = ts;
    Linda_addReference(t);
    m->tuple_request.t = t;
    return m;
}

void Message_free(Message* msg) {
    if(msg != NULL) {
    if(msg->msgid != NULL) {
        free(msg->msgid->source); free(msg->msgid->dest); free(msg->msgid);
    }
    switch(msg->type) {
    case L_DONE:
    case L_DONT_KNOW:
        break;
    case L_RESULT_STRING:
        free(msg->string);
        break;
    case L_RESULT_INT:
        break;
    case L_RESULT_TUPLE:
        Linda_delReference(msg->tuple);
        break;
    case L_RESULT_TYPE:
        Linda_delReference(msg->typestruct.typeobj);
        break;
    case L_UNBLOCK:
        break;
    case L_OUT:
        Linda_delReference(msg->out.ts);
        Linda_delReference(msg->out.t);
        break;
    case L_RD:
    case L_RDP:
        free(msg->rd.tid);
        Linda_delReference(msg->rd.ts);
        Linda_delReference(msg->rd.t);
        break;
    case L_IN:
    case L_INP:
        free(msg->in.tid);
        Linda_delReference(msg->in.ts);
        Linda_delReference(msg->in.t);
        break;
    case L_COLLECT:
    case L_COPY_COLLECT:
        Linda_delReference(msg->collect.ts1);
        Linda_delReference(msg->collect.ts2);
        Linda_delReference(msg->collect.t);
        break;
    case L_CREATE_TUPLESPACE:
        free(msg->string);
        break;
    case L_ADD_REFERENCE:
    case L_DELETE_REFERENCE:
        free(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case L_MONITOR:
    case L_LIST_TS:
    case L_GET_ROUTES:
        break;
    case L_REGISTER_PROCESS:
        break;
    case L_REGISTER_THREAD:
        free(msg->string);
        break;
    case L_REGISTER_TYPE:
    case L_UPDATE_TYPE:
        Linda_delReference(msg->typestruct.typeobj);
        if(msg->typestruct.pid != NULL) {
            free(msg->typestruct.type_id);
        }
        if(msg->typestruct.pid != NULL) {
            free(msg->typestruct.pid);
        }
        break;
    case L_REQUEST_TYPE:
        free(msg->typestruct.type_id);
        break;
    case L_MY_NAME_IS:
        free(msg->string);
        break;
    case L_GET_NODE_ID:
        break;
    case L_REGISTER_PARTITION:
    case L_DELETED_PARTITION:
        free(msg->ref.ts);
        free(msg->ref.tid);
        break;
    case L_GET_PARTITIONS:
    case L_GET_REQUESTS:
        free(msg->ref.ts);
        break;
    case L_INSPECT:
        free(msg->string);
        break;
    case L_GET_NEIGHBOURS:
        break;
    case L_GET_CONNECTION_DETAILS:
        free(msg->string);
        break;
    case L_TUPLE_REQUEST:
    case L_CANCEL_REQUEST:
        Linda_delReference(msg->tuple_request.ts);
        Linda_delReference(msg->tuple_request.t);
        break;
    case L_MULTIPLE_IN:
        Linda_delReference(msg->tuple_request.ts);
        Linda_delReference(msg->tuple_request.t);
        break;
    }
    free(msg);
    }
}

#ifdef NETWORK
FILE* fp = NULL;
unsigned long total_sent = 0;
#endif

void Message_send(int sd, MsgID* msgid, Message* msg) {
    char* msgstr;
    int len;
    int sent;
    char msglen[4];
    msg->msgid = msgid;
    msgstr = Message_getString(msg);
    len = strlen(msgstr);
    sent = 0;

#ifdef NETWORK
    if(fp == NULL) {
        if(Linda_is_server) {
            int size = snprintf(NULL, 0, "%i-server-network.txt", getpid());
            char* filename = malloc(size+1);
            sprintf(filename, "%i-server-network.txt", getpid());
            fp = fopen(filename, "w");
            free(filename);
        } else {
            int size = snprintf(NULL, 0, "%i-network.txt", getpid());
            char* filename = malloc(size+1);
            sprintf(filename, "%i-network.txt", getpid());
            fp = fopen(filename, "w");
            free(filename);
        }
    }
    fseek(fp, 0, SEEK_SET);
    total_sent += len;
    fprintf(fp, "%li\n", total_sent);
#endif
#ifdef MEMORY
    char* filename;
    if(Linda_is_server) {
        int size = snprintf(NULL, 0, "./update_memory.py `./ps_mem.py %i` %i-server-memory.txt", getpid(), getpid());
        filename = malloc(size+1);
        sprintf(filename, "./update_memory.py `./ps_mem.py %i` %i-server-memory.txt", getpid(), getpid());
    } else {
        int size = snprintf(NULL, 0, "./update_memory.py `./ps_mem.py %i` %i-memory.txt", getpid(), getpid());
        filename = malloc(size+1);
        sprintf(filename, "./update_memory.py `./ps_mem.py %i` %i-memory.txt", getpid(), getpid());
    }
    system(filename);
#endif

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
