#include "linda.h"

int main(int argc, char* argv[]) {
    int i;

    Linda_connect(Linda_port);

    LindaValue t = Linda_type("tc :: string + int;");

    LindaValue template = Linda_tuple(2);
    Linda_tupleSet(template, 0, Linda_string("py"));
    Linda_addReference(t);
    Linda_tupleSet(template, 1, t);

    for(i=0; i<4; i++) {
        LindaValue tup = Linda_in(Linda_uts, template);
        LindaValue v = Linda_tupleGet(tup, 1);

        if(Linda_getSumPos(v) == 1) {
            printf("int %i", Linda_getInt(v));
        } else {
            printf("string %s", Linda_getString(v));
        }
    }
    Linda_delReference(template);

    LindaValue tup = Linda_tuple(2);
    Linda_tupleSet(tup, 0, Linda_string("py"));

    LindaValue v = Linda_int(1);
    Linda_setType(v, t); Linda_setSumPos(v, 1);
    Linda_tupleSet(tup, 1, v);

    Linda_out(Linda_uts, tup);

    v = Linda_string("A");
    Linda_setType(v, t); Linda_setSumPos(v, 0);
    Linda_tupleSet(tup, 1, v);

    Linda_out(Linda_uts, tup);

    v = Linda_int(2);
    Linda_setType(v, t); Linda_setSumPos(v, 1);
    Linda_tupleSet(tup, 1, v);

    Linda_out(Linda_uts, tup);

    v = Linda_string("B");
    Linda_setType(v, t); Linda_setSumPos(v, 0);
    Linda_tupleSet(tup, 1, v);

    Linda_delReference(tup);
    Linda_delReference(t);

    Linda_disconnect();

    return 0;
}
