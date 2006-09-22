#include "linda.h"
#include "linda_internal.h"

Linda_tuplespace Linda_createTuplespace() {
    Linda_tuplespace ts;
    Message* m = Message_createTuplespace();
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    if(m->type != RESULT_STRING) {
        return NULL;
    }
    ts = (char*)malloc(strlen(m->string)+1);
    strcpy(ts, m->string);
    Message_free(m);
    return ts;
}

void Linda_deleteTuplespace(const Linda_tuplespace ts) {
    if(strcmp(ts, "UTS") == 0) { return; }
    if(Linda_active_connections == 0) { return; } /* We've been disconnected to ignore this message. */
    Message* m = Message_deleteReference(ts);
    Message_send(Linda_sd, NULL, m);
    Message_free(m);
    m = Message_recv(Linda_sd);
    Message_free(m);
}
