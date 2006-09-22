#include <stdio.h>

#include "linda_c.h"
#include "linda_internal.h"

void Linda_scanTuple(Tuple t, char* ref) {
    int i;
    Message* m;
    for(i=0; i<Tuple_get_size(t); i++) {
        Value* v = Tuple_get(t, i);
        switch(v->type) {
        case NIL:
        case BOOLEAN:
        case INTEGER:
        case FLOAT:
        case STRING:
        case TYPE:
            break;
        case TSREF:
            m = Message_addReference2(v->tsid, ref);
            Message_send(Linda_sd, NULL, m);
            Message_free(m);
            m = Message_recv(Linda_sd);
            Message_free(m);
        default:
            fprintf(stderr, "Unknown value found when scanning tuple.\n");
        }
    }
}
