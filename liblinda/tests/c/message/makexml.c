#include "linda.h"
#include "../../src/linda_internal.h"

int main(int argc, char* argv[]) {
    Message m;
    m.type = OUT;
    m.out.ts = "TestTS";
    m.out.t = Tuple_new(2);
    Tuple_set(m.out.t, 0, Value_bool(1));
    Tuple_set(m.out.t, 1, Value_int(100));

    char* msg = Message_getString(&m);
    printf("%s", msg);

    Tuple_delete(m.out.t);
    free(msg);

    return 0;
}
