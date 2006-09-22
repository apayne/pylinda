#include <sys/time.h>

#include "linda.h"
#include "../../src/linda_internal.h"

extern int Linda_sd;

int main(int argc, char* argv[]) {
    int i;
    timeval start;
    timeval end;
    if(!Linda_connect()) {
        printf("Failed to connect to server.\n");
        return 1;
    }

    gettimeofday(&start);
    for(i=0; i < 10000; i++) {
        Message m;
        m.type = OUT;
        m.out.ts = "MYTSID";
        m.out.t = Tuple_new(1);
        Tuple_set(m.out.t, 0, Value_int(100));

        Message_send(Linda_sd, &m);
    }
    gettimeofday(&end);

    Linda_disconnect();

    printf("Sent %i messages in %.3f seconds. %.3f m/s.\n", i, (end.tv_sec+end.tv_usec/1000.0f)-(start.tv_sec+start.tv_usec/1000.0f), i/((end.tv_sec+end.tv_usec/1000.0f)-(start.tv_sec+start.tv_usec/1000.0f)));

    return 0;
}
