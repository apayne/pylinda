#include "linda.h"

int main(int argc, char* argv[]) {
    Tuple t = Tuple_new(1);

    Tuple_set(t, 0, Value_int(1));

    Tuple_clear(t);

    Tuple_set(t, 0, Value_string("Hello World!"));

    Tuple_clear(t);

    Tuple_delete(t);

    return 0;
}
