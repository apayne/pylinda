#include "linda.h"

int main(int argc, char* argv[]) {
    LindaValue tup;
    LindaValue r;
    LindaValue t1;
    LindaValue v1;
    LindaValue t2;
    LindaValue v2;

    Linda_connect(Linda_port);

    t1 = Linda_type("t1 :: int + string;");
    t2 = Linda_type("t2 :: string + int;");

    v1 = Linda_sum(Linda_int(1), 0);
    Linda_setType(v1, t1);

    tup = Linda_tuple(1);
    Linda_tupleSet(tup, 0, v1);
    Linda_out(Linda_uts, tup);

    Linda_tupleSet(tup, 0, t2);
    r = Linda_in(Linda_uts, tup);

    printf("Sum pos: %i -> %i\n", Linda_getSumPos(v1), Linda_getSumPos(Linda_tupleGet(r, 0)));
}
