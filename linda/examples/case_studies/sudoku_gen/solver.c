#include "linda.h"

int main(int argc, char* argv) {
    Linda_connect();

    LindaValue grid = Linda_type("grid :: ((Nil + int) * 9) * 9;");
    LindaValue functype = Linda_type("difficulty :: grid -> bool;");

    LindaValue template = Linda_tuple(2);
    Linda_setTuple(template, 0, Linda_string("difficulty_func"));
    Linda_setTuple(template, 1, functype);

    LindaValue tup = Linda_rd(Linda_uts, template);
    LindaValue difficulty_func = Linda_getTuple(tup, 0);
    Linda_addReference(difficulty_func);
    Linda_delReference(tup);

    Linda_setTuple(template, 0, Linda_string("work_ts"));
    Linda_setTuple(template, 1, Linda_tupleSpaceType);

    LindaValue tup = Linda_rd(Linda_uts, template);
    LindaValue work_ts = Linda_getTuple(tup, 0);
    Linda_addReference(work_ts);
    Linda_delReference(tup);

    Linda_delReference(template);
    template = Linda_tuple(1);
    Linda_setTuple(template, 0, grid);
    while(1) {
        LindaValue args;
        LindaValue r;

        tup = Linda_inp(work_ts, template);
        if(tup == NULL) {
            break;
        }

        args = Linda_tuple(1);
        Linda_addReference(Linda_getTuple(tup, 0));
        Linda_setTuple(args, 0, Linda_getTuple(tup, 0));

        r = Linda_callFunction(difficulty_func, args);
        Linda_delReference(args);
        if(Linda_isTrue(r)) {
            args = Linda_tuple(2);
            Linda_setTuple(template, 0, Linda_string("sudoku"));
            Linda_addReference(Linda_getTuple(tup, 0));
            Linda_setTuple(template, 1, Linda_getTuple(tup, 0));
            Linda_out(Linda_uts, args);
        }
        Linda_delReference(tup);
    }
}
