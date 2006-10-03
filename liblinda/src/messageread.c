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

#include "expat.h"

#include "linda.h"
#include "linda_internal.h"

unsigned char Linda_XML_init = 0;
XML_Parser p;

void StartElementHandler(void* userData, const XML_Char* name, const XML_Char** atts);
void EndElementHandler(void* userData, const XML_Char* name);
void CharacterDataHandler(void* userData, const XML_Char* s, int len);

struct buildmessage_t {
    char* text;
    int tuplecount;
    Tuplequeue tq;
    Message* m;
    unsigned char in_tuple;
};

typedef struct buildmessage_t buildmessage;

void initbuildmessage(buildmessage* bm) {
    bm->text = NULL;
    bm->tuplecount = 0;
    bm->m = malloc(sizeof(Message));
    memset(bm->m, 0, sizeof(Message));
    bm->tq = NULL;
    bm->in_tuple = 0;
}

Message* delbuildmessage(buildmessage* bm) {
    free(bm->text);
    if(bm->tq != NULL) {
        fprintf(stderr, "Tuplequeue not null!\n");
    }
    return bm->m;
}

Message* Message_recv(int s) {
    XML_Char* buf = NULL;
    int bytesrecv;
    int bytesread = 0;
    int msgsize;
    buildmessage bm;
    initbuildmessage(&bm);

    if(!Linda_XML_init) {
        p = XML_ParserCreate("UTF-8");
        Linda_XML_init = 1;
    }

    XML_SetElementHandler(p, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(p, CharacterDataHandler);

    buf = XML_GetBuffer(p, 1024);

    XML_SetUserData(p, (void*)&bm);
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
        bytesrecv = recv(s, buf, ((msgsize - bytesread) < 1024 ? (msgsize - bytesread): 1024), 0);
        if(bytesrecv <= 0) {
            free(delbuildmessage(&bm));
            return NULL;
        }
        bytesread += bytesrecv;

        if(!XML_ParseBuffer(p, bytesread, bytesread == msgsize)) {
            fprintf(stderr, "XML Parser Error!\n");
            exit(-1);
        }
    }

    XML_ParserReset(p, "UTF-8");
    return delbuildmessage(&bm);
}

Message* Message_parse(char* text, int len, unsigned char final) {
    buildmessage bm;
    initbuildmessage(&bm);

    if(!Linda_XML_init) {
        p = XML_ParserCreate("UTF-8");
        XML_SetElementHandler(p, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(p, CharacterDataHandler);
        Linda_XML_init = 1;
    }

    XML_SetUserData(p, (void*)&bm);
    XML_Parse(p, text, len, final);

    return delbuildmessage(&bm);
}

void Message_shutdown() {
    if(Linda_XML_init) {
        XML_ParserFree(p);
        Linda_XML_init = 0;
    }
}

void StartElementHandler(void* userData, const XML_Char* name, const XML_Char** attrs) {
    buildmessage* bm = (buildmessage*)userData;

    if(strcmp(name, "msg") == 0) {
    } else if(strcmp(name, "action") == 0) {
    } else if(strcmp(name, "body") == 0) {
    } else if(strcmp(name, "ts") == 0) {
        const XML_Char** attrptr = attrs;
        while(*(attrptr) != NULL) {
            if(strcmp(*attrptr, "id") == 0) {
                char* c = malloc(strlen(*(attrptr+1)) + 1);
                strcpy(c, *(attrptr+1));
                if(bm->in_tuple > 0) {
                    Tuple_add(Tuplequeue_top(bm->tq), Value_tsref(c));
                } else {
                    switch(bm->m->type) {
                    case OUT:
                        bm->m->out.ts = c;
                        break;
                    case IN:
                    case INP:
                        bm->m->in.ts = c;
                        break;
                    case RD:
                    case RDP:
                        bm->m->rd.ts = c;
                        break;
                    case COLLECT:
                    case COPY_COLLECT:
                        if(bm->m->collect.ts1 == NULL) {
                            bm->m->collect.ts1 = c;
                        } else {
                            bm->m->collect.ts2 = c;
                        }
                        break;
                    case ADD_REFERENCE:
                    case DELETE_REFERENCE:
                        bm->m->ref.ts = c;
                        break;
                    case INSPECT:
                    case REGISTER_THREAD:
                    case GET_PARTITIONS:
                    case GET_REQUESTS:
                        bm->m->ts = c;
                        break;
                    case REGISTER_PARTITION:
                    case DELETED_PARTITION:
                        bm->m->ref.ts = c;
                        break;
                    case TUPLE_REQUEST:
                    case CANCEL_REQUEST:
                    case MULTIPLE_IN:
                        bm->m->tuple_request.ts = c;
                        break;
                    default:
                        fprintf(stderr, "Got unexpected TSID.\n");
                        free(c);
                    }
                }
            }
            attrptr = attrptr + 2;
        }
    } else if(strcmp(name, "tid") == 0) {
        const XML_Char** attrptr = attrs;
        while(*(attrptr) != NULL) {
            if(strcmp(*attrptr, "id") == 0) {
                char* c = malloc(strlen(*(attrptr+1)) + 1);
                strcpy(c, *(attrptr+1));
                switch(bm->m->type) {
                case IN:
                case INP:
                    bm->m->in.tid = c;
                    break;
                case RD:
                case RDP:
                    bm->m->rd.tid = c;
                    break;
                default:
                    fprintf(stderr, "Got unexpected TID.\n");
                    free(c);
                }
            }
            attrptr = attrptr + 2;
        }
    } else if(strcmp(name, "tuple") == 0) {
        bm->tq = Tuplequeue_push(bm->tq);
        bm->in_tuple += 1;
    } else if(strcmp(name, "msgid") == 0) {
        const XML_Char** attrptr = attrs;
        bm->m->msgid = (MsgID*)malloc(sizeof(MsgID));
        while(*(attrptr) != NULL) {
            if(strcmp(*attrptr, "dest") == 0) {
                char* c = malloc(strlen(*(attrptr+1)) + 1);
                strcpy(c, *(attrptr+1));
                bm->m->msgid->dest = c;
            } else if(strcmp(*attrptr, "source") == 0) {
                char* c = malloc(strlen(*(attrptr+1)) + 1);
                strcpy(c, *(attrptr+1));
                bm->m->msgid->source = c;
            } else if(strcmp(*attrptr, "count") == 0) {
                bm->m->msgid->count = atoi(*(attrptr+1));
            }
            attrptr = attrptr + 2;
        }
    } else if(strcmp(name, "true") == 0 || strcmp(name, "false") == 0 || strcmp(name, "int") == 0 || strcmp(name, "string") == 0) {
    } else {
        fprintf(stderr, "Unknown message open tag '%s'. Ignoring.\n", name);
    }
}

void EndElementHandler(void* userData, const XML_Char* name) {
    buildmessage* bm = (buildmessage*)userData;

    if(strcmp(name, "msg") == 0) {
    } else if(strcmp(name, "action") == 0) {
        if(bm->text == NULL) {
            fprintf(stderr, "No message type specified.\n");
            exit(-1);
        } else if(strcmp(bm->text, "done") == 0) {
            bm->m->type = DONE;
        } else if(strcmp(bm->text, "dont_know") == 0) {
            bm->m->type = DONT_KNOW;
        } else if(strcmp(bm->text, "result_string") == 0) {
            bm->m->type = RESULT_STRING;
        } else if(strcmp(bm->text, "result_int") == 0) {
            bm->m->type = RESULT_INT;
        } else if(strcmp(bm->text, "result_tuple") == 0) {
            bm->m->type = RESULT_TUPLE;
        } else if(strcmp(bm->text, "out") == 0) {
            bm->m->type = OUT;
            bm->m->out.ts = NULL;
            bm->m->out.t = NULL;
        } else if(strcmp(bm->text, "in") == 0) {
            bm->m->type = IN;
            bm->m->in.ts = NULL;
            bm->m->in.t = NULL;
        } else if(strcmp(bm->text, "rd") == 0) {
            bm->m->type = RD;
            bm->m->rd.ts = NULL;
            bm->m->rd.t = NULL;
        } else if(strcmp(bm->text, "inp") == 0) {
            bm->m->type = INP;
            bm->m->in.ts = NULL;
            bm->m->in.t = NULL;
        } else if(strcmp(bm->text, "rdp") == 0) {
            bm->m->type = RDP;
            bm->m->rd.ts = NULL;
            bm->m->rd.t = NULL;
        } else if(strcmp(bm->text, "collect") == 0) {
            bm->m->type = COLLECT;
            bm->m->collect.ts1 = NULL;
            bm->m->collect.ts2 = NULL;
            bm->m->collect.t = NULL;
        } else if(strcmp(bm->text, "copy_collect") == 0) {
            bm->m->type = COPY_COLLECT;
            bm->m->collect.ts1 = NULL;
            bm->m->collect.ts2 = NULL;
            bm->m->collect.t = NULL;
        } else if(strcmp(bm->text, "create_tuplespace") == 0) {
            bm->m->type = CREATE_TUPLESPACE;
        } else if(strcmp(bm->text, "add_reference") == 0) {
            bm->m->type = ADD_REFERENCE;
        } else if(strcmp(bm->text, "delete_reference") == 0) {
            bm->m->type = DELETE_REFERENCE;
        } else if(strcmp(bm->text, "monitor") == 0) {
            bm->m->type = MONITOR;
        } else if(strcmp(bm->text, "list_ts") == 0) {
            bm->m->type = LIST_TS;
        } else if(strcmp(bm->text, "inspect") == 0) {
            bm->m->type = INSPECT;
        } else if(strcmp(bm->text, "get_routes") == 0) {
            bm->m->type = GET_ROUTES;
        } else if(strcmp(bm->text, "register_process") == 0) {
            bm->m->type = REGISTER_PROCESS;
        } else if(strcmp(bm->text, "register_thread") == 0) {
            bm->m->type = REGISTER_THREAD;
        } else if(strcmp(bm->text, "my_name_is") == 0) {
            bm->m->type = MY_NAME_IS;
        } else if(strcmp(bm->text, "get_node_id") == 0) {
            bm->m->type = GET_NODE_ID;
        } else if(strcmp(bm->text, "register_partition") == 0) {
            bm->m->type = REGISTER_PARTITION;
        } else if(strcmp(bm->text, "get_partitions") == 0) {
            bm->m->type = GET_PARTITIONS;
        } else if(strcmp(bm->text, "deleted_partition") == 0) {
            bm->m->type = DELETED_PARTITION;
        } else if(strcmp(bm->text, "get_neighbours") == 0) {
            bm->m->type = GET_NEIGHBOURS;
        } else if(strcmp(bm->text, "get_connection_details") == 0) {
            bm->m->type = GET_CONNECTION_DETAILS;
        } else if(strcmp(bm->text, "get_requests") == 0) {
            bm->m->type = GET_REQUESTS;
        } else if(strcmp(bm->text, "tuple_request") == 0) {
            bm->m->type = TUPLE_REQUEST;
        } else if(strcmp(bm->text, "cancel_request") == 0) {
            bm->m->type = CANCEL_REQUEST;
        } else if(strcmp(bm->text, "multiple_in") == 0) {
            bm->m->type = MULTIPLE_IN;
        } else {
            fprintf(stderr, "Unknown message type '%s'.\n", bm->text);
            exit(-1);
        }

        free(bm->text); bm->text = NULL;
    } else if(strcmp(name, "body") == 0) {
    } else if(strcmp(name, "ts") == 0) {
    } else if(strcmp(name, "msgid") == 0) {
    } else if(strcmp(name, "tid") == 0) {
    } else if(strcmp(name, "tuple") == 0) {
        if(bm->in_tuple > 0) {
            Tuple t = Tuple_copy(Tuplequeue_top(bm->tq));
            bm->tq = Tuplequeue_pop(bm->tq);
            bm->in_tuple--;
            if(bm->in_tuple == 0) {
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
                    Tuple_free(t);
                }
            } else {
                Tuple_add(Tuplequeue_top(bm->tq), Value_tuple(t));
                Tuple_free(t);
            }
        }
    } else if(strcmp(name, "true") == 0) {
        Tuple_add(Tuplequeue_top(bm->tq), Value_bool(1));
    } else if(strcmp(name, "false") == 0) {
        Tuple_add(Tuplequeue_top(bm->tq), Value_bool(0));
    } else if(strcmp(name, "int") == 0) {
        switch(bm->m->type) {
        case RESULT_INT:
            bm->m->i = atoi(bm->text);
            break;
        default:
            Tuple_add(Tuplequeue_top(bm->tq), Value_int(atoi(bm->text)));
            free(bm->text); bm->text = NULL;
        }
    } else if(strcmp(name, "string") == 0) {
        switch(bm->m->type) {
        case RESULT_STRING:
        case CREATE_TUPLESPACE:
        case REGISTER_THREAD:
        case MY_NAME_IS:
        case GET_CONNECTION_DETAILS:
            bm->m->string = bm->text; bm->text = NULL;
            break;
        case ADD_REFERENCE:
        case DELETE_REFERENCE:
        case REGISTER_PARTITION:
        case DELETED_PARTITION:
            bm->m->ref.tid = bm->text; bm->text = NULL;
            break;
        default:
            Tuple_add(Tuplequeue_top(bm->tq), Value_string(bm->text));
            free(bm->text); bm->text = NULL;
        }
    } else {
        fprintf(stderr, "Unknown message close tag '%s'. Ignoring.\n", name);
    }
}

void CharacterDataHandler(void* userData, const XML_Char* s, int len) {
    buildmessage* bm = (buildmessage*)userData;

    if(bm->text != NULL) {
        int curlen = strlen(bm->text);
        char* c = malloc(curlen + len + 1);
        strncpy(c, bm->text, curlen);
        strncpy(&(c[curlen]), s, len);
        c[curlen + len] = '\0';
        free(bm->text);
        bm->text = c;
    } else {
        char* c = malloc(len+1);
        strncpy(c, s, len);
        c[len] = '\0';
        bm->text = c;
    }
}
