#include "linda.h"
#include "../../../src/linda_internal.h"

int main(int argc, char* argv[]) {
    if(!Linda_serve()) {
        return 1;
    }

    int sd = Linda_accept();

    Message* m = Message_recv(sd);
    Message_free(m);
    m = Message_done();
    Message_send(sd, m);
    Message_free(m);

    Linda_server_disconnect();

    return 0;
}
