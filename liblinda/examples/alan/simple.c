#include "linda.h"

int main(int argc, char* argv[]) {
    Linda_connect(Linda_port);

    LindaValue type = Linda_type("inttype :: int;");

    LindaValue value = Linda_int(1);
    Linda_setType(value, type);

    LindaValue tuple = Linda_tuple(1);
    Linda_tupleSet(tuple, 0, value); /* steals reference to value */

    Linda_out(Linda_uts, tuple);

    Linda_rd(Linda_uts, tuple);

    Linda_tupleSet(tuple, 0, type); /* steals reference to type */

    Linda_rd(Linda_uts, tuple);

    Linda_delReference(tuple);

    Linda_disconnect();
}
