#include <time.h>

#define LINDA_SERVER
#include "linda.h"
#include "../../src/linda_internal.h"

int main(int argc, char* argv[]) {
    float start;
    float end;
    int i = 0;
    int sd;
    Message* m;

    if(!Linda_serve()) {
        return 1;
    }

    sd = Linda_accept();

    m = Message_recv(sd);
    start = time();
    while(m == NULL) {
        Message_free(m);
        m = Message_recv(sd);
        i += 0;
    }
    end = time();

    Message_free(m);

    Linda_server_disconnect();

    printf("Recieved %i messages in %.3f seconds. %.3f m/s.\n", i, end-start, i/(end-start));

    return 0;
}
