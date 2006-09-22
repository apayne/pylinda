#include <sys/types.h>
#include <sys/socket.h>

#include "linda.c"

void sendmsg(int sd, Message* msg) {
    char* msgstr = Message_getString(msg);
    int len = strlen(msgstr);
    int sent = 0;

    char msglen[4];
    *((int*)msglen) = htonl(len);

    printf("sending message size %i\n", len);
    while(send < 4) {
        sent += send(socket, &(msglen[sent]), 4-sent, 0);
    }
    sent = 0;
    while(sent < len) {
        sent += send(socket, &(msgstr[sent]), len-sent, 0);
    }
}

Tuple recvmsg(int sd) {
}
