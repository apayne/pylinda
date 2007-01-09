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
#include <string.h>
#include <arpa/inet.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "linda.h"
#include "linda_internal.h"

struct buildmessage_t {
    char* text;
    int tuplecount;
    Tuplequeue tq;
    Message* m;
    unsigned char in_tuple;
};

typedef struct buildmessage_t buildmessage;

void BuildMessage(buildmessage* bm, xmlDocPtr doc, xmlNodePtr node);

void initbuildmessage(buildmessage* bm) {
    bm->text = NULL;
    bm->tuplecount = 0;
    bm->m = malloc(sizeof(Message));
    memset(bm->m, 0, sizeof(Message));
    bm->tq = NULL;
}

Message* delbuildmessage(buildmessage* bm) {
    free(bm->text);
    if(bm->tq != NULL) {
        fprintf(stderr, "Tuplequeue not null!\n");
    }
    return bm->m;
}

Message* Message_recv(int s) {
    char* buf = NULL;
    int bytesrecv;
    int bytesread = 0;
    int msgsize;
    buildmessage bm;
    initbuildmessage(&bm);

    bytesread = 0;
    while(bytesread < 4) {
        bytesrecv = recv(s, &(((char*)&msgsize)[bytesread]), 4-bytesread, 0);
        if(bytesrecv <= 0) {
            free(delbuildmessage(&bm));
            return NULL;
        }
        bytesread += bytesrecv;
    }
    msgsize = ntohl(msgsize);
    bytesread = 0;
    while(bytesread < msgsize) {
        int buf_size = (102400 < (msgsize - bytesread) ? 102400 : (msgsize - bytesread));
        char* tmp = (char*)malloc(bytesread + buf_size);
        memcpy(tmp, buf, bytesread);
        free(buf);
        buf = tmp;
        bytesrecv = recv(s, &(buf[bytesread]), buf_size, 0);
        if(bytesrecv <= 0) {
            free(buf);
            free(delbuildmessage(&bm));
            return NULL;
        }
        bytesread += bytesrecv;
    }

#ifdef DEBUG
    int i;
    for(i = 0; i < bytesrecv; i++) {
        printf("%c", buf[i]);
    }
    printf("\n---\n");
#endif

    xmlDocPtr doc = xmlReadMemory(buf, bytesread, NULL, NULL, 0);
    free(buf);
    if(doc == NULL) {
        int i;
        for(i = 0; i < bytesrecv; i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
        fprintf(stderr, "XML Parser Error!\n");
        exit(-1);
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);

    BuildMessage(&bm, doc, root);

    xmlFreeDoc(doc);

    return delbuildmessage(&bm);
}

void BuildMessage(buildmessage* bm, xmlDocPtr doc, xmlNodePtr node) {
    if(strcmp((char*)node->name, "linda") == 0) {
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                BuildMessage(bm, doc, cur_node);
            }
            cur_node = cur_node->next;
        }
        return;
    } else if(strcmp((char*)node->name, "action") == 0) {
        xmlNode* cur_node = node->children;
        char* text = NULL;
        while(cur_node) {
            if(cur_node->type == XML_TEXT_NODE) {
                text = (char*)cur_node->content;
            }
            cur_node = cur_node->next;
        }
        if(text == NULL) {
            fprintf(stderr, "No message type specified.\n");
            exit(-1);
        } else if(strcmp(text, "done") == 0) {
            bm->m->type = DONE;
        } else if(strcmp(text, "dont_know") == 0) {
            bm->m->type = DONT_KNOW;
        } else if(strcmp(text, "result_string") == 0) {
            bm->m->type = RESULT_STRING;
        } else if(strcmp(text, "result_int") == 0) {
            bm->m->type = RESULT_INT;
        } else if(strcmp(text, "result_tuple") == 0) {
            bm->m->type = RESULT_TUPLE;
        } else if(strcmp(text, "out") == 0) {
            bm->m->type = OUT;
            bm->m->out.ts = NULL;
            bm->m->out.t = NULL;
        } else if(strcmp(text, "in") == 0) {
            bm->m->type = IN;
            bm->m->in.ts = NULL;
            bm->m->in.t = NULL;
        } else if(strcmp(text, "rd") == 0) {
            bm->m->type = RD;
            bm->m->rd.ts = NULL;
            bm->m->rd.t = NULL;
        } else if(strcmp(text, "inp") == 0) {
            bm->m->type = INP;
            bm->m->in.ts = NULL;
            bm->m->in.t = NULL;
        } else if(strcmp(text, "rdp") == 0) {
            bm->m->type = RDP;
            bm->m->rd.ts = NULL;
            bm->m->rd.t = NULL;
        } else if(strcmp(text, "collect") == 0) {
            bm->m->type = COLLECT;
            bm->m->collect.ts1 = NULL;
            bm->m->collect.ts2 = NULL;
            bm->m->collect.t = NULL;
        } else if(strcmp(text, "copy_collect") == 0) {
            bm->m->type = COPY_COLLECT;
            bm->m->collect.ts1 = NULL;
            bm->m->collect.ts2 = NULL;
            bm->m->collect.t = NULL;
        } else if(strcmp(text, "unblock") == 0) {
            bm->m->type = UNBLOCK;
        } else if(strcmp(text, "create_tuplespace") == 0) {
            bm->m->type = CREATE_TUPLESPACE;
        } else if(strcmp(text, "add_reference") == 0) {
            bm->m->type = ADD_REFERENCE;
        } else if(strcmp(text, "delete_reference") == 0) {
            bm->m->type = DELETE_REFERENCE;
        } else if(strcmp(text, "monitor") == 0) {
            bm->m->type = MONITOR;
        } else if(strcmp(text, "list_ts") == 0) {
            bm->m->type = LIST_TS;
        } else if(strcmp(text, "inspect") == 0) {
            bm->m->type = INSPECT;
        } else if(strcmp(text, "get_routes") == 0) {
            bm->m->type = GET_ROUTES;
        } else if(strcmp(text, "register_process") == 0) {
            bm->m->type = REGISTER_PROCESS;
        } else if(strcmp(text, "register_thread") == 0) {
            bm->m->type = REGISTER_THREAD;
        } else if(strcmp(text, "register_type") == 0) {
            bm->m->type = REGISTER_TYPE;
        } else if(strcmp(text, "update_type") == 0) {
            bm->m->type = UPDATE_TYPE;
        } else if(strcmp(text, "my_name_is") == 0) {
            bm->m->type = MY_NAME_IS;
        } else if(strcmp(text, "get_node_id") == 0) {
            bm->m->type = GET_NODE_ID;
        } else if(strcmp(text, "register_partition") == 0) {
            bm->m->type = REGISTER_PARTITION;
        } else if(strcmp(text, "get_partitions") == 0) {
            bm->m->type = GET_PARTITIONS;
        } else if(strcmp(text, "deleted_partition") == 0) {
            bm->m->type = DELETED_PARTITION;
        } else if(strcmp(text, "get_neighbours") == 0) {
            bm->m->type = GET_NEIGHBOURS;
        } else if(strcmp(text, "get_connection_details") == 0) {
            bm->m->type = GET_CONNECTION_DETAILS;
        } else if(strcmp(text, "get_requests") == 0) {
            bm->m->type = GET_REQUESTS;
        } else if(strcmp(text, "tuple_request") == 0) {
            bm->m->type = TUPLE_REQUEST;
        } else if(strcmp(text, "cancel_request") == 0) {
            bm->m->type = CANCEL_REQUEST;
        } else if(strcmp(text, "multiple_in") == 0) {
            bm->m->type = MULTIPLE_IN;
        } else {
            fprintf(stderr, "Unknown message type '%s'.\n", text);
            exit(-1);
        }
    } else if(strcmp((char*)node->name, "body") == 0) {
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                BuildMessage(bm, doc, cur_node);
            }
            cur_node = cur_node->next;
        }
        return;
    } else if(strcmp((char*)node->name, "ts") == 0) {
        char* val = (char*)xmlGetProp(node, (xmlChar*)"id");
        switch(bm->m->type) {
        case OUT:
            bm->m->out.ts = Minimal_tupleSpace(val);
            break;
        case IN:
        case INP:
            bm->m->in.ts = Minimal_tupleSpace(val);
            break;
        case RD:
        case RDP:
            bm->m->rd.ts = Minimal_tupleSpace(val);
            break;
        case COLLECT:
        case COPY_COLLECT:
            if(bm->m->collect.ts1 == NULL) {
                bm->m->collect.ts1 = Minimal_tupleSpace(val);
            } else {
                bm->m->collect.ts2 = Minimal_tupleSpace(val);
            }
            break;
        case ADD_REFERENCE:
        case DELETE_REFERENCE:
            bm->m->ref.ts = Minimal_tupleSpace(val);
            break;
        case INSPECT:
        case REGISTER_THREAD:
        case GET_PARTITIONS:
        case GET_REQUESTS:
            bm->m->ts = Minimal_tupleSpace(val);
            break;
        case REGISTER_PARTITION:
        case DELETED_PARTITION:
            bm->m->ref.ts = Minimal_tupleSpace(val);
            break;
        case TUPLE_REQUEST:
        case CANCEL_REQUEST:
        case MULTIPLE_IN:
            bm->m->tuple_request.ts = Minimal_tupleSpace(val);
            break;
        default:
            fprintf(stderr, "Got unexpected TSID.\n");
        }
        free(val);
    } else if(strcmp((char*)node->name, "tid") == 0) {
        char* val = (char*)xmlGetProp(node, (xmlChar*)"id");
        switch(bm->m->type) {
        case IN:
        case INP:
            bm->m->in.tid = val;
            break;
        case RD:
        case RDP:
            bm->m->rd.tid = val;
            break;
        default:
            fprintf(stderr, "Got unexpected TID.\n");
        }
    } else if(strcmp((char*)node->name, "tuple") == 0) {
        LindaValue t = Minimal_xmlToValue(node);

        switch(bm->m->type) {
        case RESULT_TUPLE:
            bm->m->tuple = t;
            break;
        case OUT:
            bm->m->out.t = t;
            break;
        case IN:
        case INP:
            bm->m->in.t = t;
            break;
        case RD:
        case RDP:
            bm->m->rd.t = t;
            break;
        case COLLECT:
        case COPY_COLLECT:
            bm->m->collect.t = t;
            break;
        case TUPLE_REQUEST:
        case CANCEL_REQUEST:
        case MULTIPLE_IN:
            bm->m->tuple_request.t = t;
            break;
        default:
            fprintf(stderr, "Discarding tuple due to invalid message type.\n");
            Linda_delReference(t);
        }
    } else if(strcmp((char*)node->name, "integer") == 0) {
        LindaValue i = Minimal_xmlToValue(node);
        switch(bm->m->type) {
        case RESULT_INT:
            bm->m->i = Linda_getInt(i);
            break;
        case UPDATE_TYPE:
            bm->m->typestruct.type_id = Linda_getInt(i);
            break;
        default:
            fprintf(stderr, "Discarding int due to invalid message type.\n");
        }
        Linda_delReference(i);
    } else if(strcmp((char*)node->name, "string") == 0) {
        LindaValue sv = Minimal_xmlToValue(node);
        char* s = malloc(strlen(sv->string)+1);
        strcpy(s, sv->string);
        Linda_delReference(sv);

        switch(bm->m->type) {
        case RESULT_STRING:
        case CREATE_TUPLESPACE:
        case REGISTER_THREAD:
        case MY_NAME_IS:
        case GET_CONNECTION_DETAILS:
            bm->m->string = s;
            break;
        case ADD_REFERENCE:
        case DELETE_REFERENCE:
        case REGISTER_PARTITION:
        case DELETED_PARTITION:
            bm->m->ref.tid = s;
            break;
        default:
            fprintf(stderr, "Discarding string due to invalid message type.\n");
        }
    } else if(strcmp((char*)node->name, "element") == 0) {
        LindaValue t = Minimal_xmlToValue(node);

        switch(bm->m->type) {
        case REGISTER_TYPE:
        case UPDATE_TYPE:
            bm->m->typestruct.typeobj = t;
            break;
        default:
            fprintf(stderr, "Discarding element due to invalid message type.\n");
        }
    } else if(strcmp((char*)node->name, "msgid") == 0) {
        bm->m->msgid = (MsgID*)malloc(sizeof(MsgID));

        bm->m->msgid->dest = malloc(strlen((char*)xmlGetProp(node, (xmlChar*)"dest")) + 1);
        strcpy(bm->m->msgid->dest, (char*)xmlGetProp(node, (xmlChar*)"dest"));

        bm->m->msgid->source = malloc(strlen((char*)xmlGetProp(node, (xmlChar*)"source")) + 1);
        strcpy(bm->m->msgid->source, (char*)xmlGetProp(node, (xmlChar*)"source"));

        bm->m->msgid->count = atoi((char*)xmlGetProp(node, (xmlChar*)"count"));
    } else {
        fprintf(stderr, "Unknown message tag '%s'. Ignoring.\n", node->name);
    }
}

