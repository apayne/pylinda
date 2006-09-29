#include "stdio.h"

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
    Tuple tuple;
    Message* m;
    unsigned char in_tuple;
};

typedef struct buildmessage_t buildmessage;

void initbuildmessage(buildmessage* bm) {
    bm->text = NULL;
    bm->tuplecount = 0;
    bm->tuple = NULL;
    bm->m = malloc(sizeof(Message));
    memset(bm->m, 0, sizeof(Message));
    bm->in_tuple = 0;
}

Message* delbuildmessage(buildmessage* bm) {
    free(bm->text);
    Tuple_free(bm->tuple);
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
        bytesrecv = recv(s, &(((char*)&msgsize)[bytesread]), 4-bytesread, NULL);
        if(bytesrecv <= 0) {
            free(delbuildmessage(&bm));
            return NULL;
        }
        bytesread += bytesrecv;
    }
    msgsize = ntohl(msgsize);
    bytesread = 0;
    while(bytesread < msgsize) {
        int i;
        bytesrecv = recv(s, buf, ((msgsize - bytesread) < 1024 ? (msgsize - bytesread): 1024), 0);
        if(bytesrecv <= 0) {
            free(delbuildmessage(&bm));
            return NULL;
        }
        bytesread += bytesrecv;

        for(i=0; i<bytesread; i++) {
            printf("%c", buf[i]);
        }
        printf("\n");

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
                if(bm->in_tuple) {
                    Tuple_add(bm->tuple, Value_tsref(c));
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
        bm->tuple = Tuple_new(0);
        bm->in_tuple = 1;
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
        printf("Unknown message open tag '%s'. Ignoring.\n", name);
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
        switch(bm->m->type) {
        case RESULT_TUPLE:
            bm->m->tuple = bm->tuple;
            bm->tuple = NULL;
            break;
        case OUT:
            bm->m->out.t = bm->tuple;
            bm->tuple = NULL;
            break;
        case IN:
        case INP:
            bm->m->in.t = bm->tuple;
            bm->tuple = NULL;
            break;
        case RD:
        case RDP:
            bm->m->rd.t = bm->tuple;
            bm->tuple = NULL;
            break;
        case TUPLE_REQUEST:
            bm->m->tuple_request.t = bm->tuple;
            bm->tuple = NULL;
            break;
        default:
            fprintf(stderr, "Discarding tuple due to invalid message type.\n");
            Tuple_free(bm->tuple); bm->tuple = NULL;
        }
        bm->in_tuple = 0;
    } else if(strcmp(name, "true") == 0) {
        Tuple_add(bm->tuple, Value_bool(1));
    } else if(strcmp(name, "false") == 0) {
        Tuple_add(bm->tuple, Value_bool(0));
    } else if(strcmp(name, "int") == 0) {
        switch(bm->m->type) {
        case RESULT_INT:
            bm->m->i = atoi(bm->text);
            break;
        default:
            Tuple_add(bm->tuple, Value_int(atoi(bm->text)));
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
            Tuple_add(bm->tuple, Value_string(bm->text));
            free(bm->text); bm->text = NULL;
        }
    } else {
        printf("Unknown message close tag '%s'. Ignoring.\n", name);
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
