#include "linda.h"

int main(int argc, char* argv[]) {
    int i;
    LindaValue t;
    LindaValue t2;

    Linda_init();

    Linda_connect(Linda_port);

    t = Linda_tuple(1);
    Linda_tupleSet(t, 0, Linda_int(0));

    Linda_out(Linda_uts, t);

    for(i=0; i<2; i++) {
        t2 = Linda_in(Linda_uts, t);
        Linda_delReference(t);

        Linda_out(Linda_uts, t2);
        t = t2;
    }
    Linda_delReference(t);

    Linda_disconnect();
}
