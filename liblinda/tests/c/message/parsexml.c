#include "linda.h"
#include "../../src/linda_internal.h"

char* genXML();

int main(int argc, char* argv[]) {
    char* msg = genXML();

    printf(msg);
    printf("\n");

    Message* m = Message_parse(msg, strlen(msg), 1);

    free(msg);

    Message_shutdown();

    return 0;
}

char* genXML() {
    Message m;
    m.type = OUT;
    m.out.ts = "TestTS";
    m.out.t = Tuple_new(2);
    Tuple_set(m.out.t, 0, Value_bool(1));
    Tuple_set(m.out.t, 1, Value_int(100));

    char* msg = Message_getString(&m);

    Tuple_delete(m.out.t);
    return msg;
}
