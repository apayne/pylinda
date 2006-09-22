#include "linda.h"

#define FORCE_C
#include "linda.h"

namespace Linda {

TupleSpace uts("UTS");

bool connect() {
    return Linda_connect();
}

void disconnect() {
    Linda_disconnect();
}

}

