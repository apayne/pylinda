#include "linda.h"

#define FORCE_C
#include "linda.h"

namespace Linda {

TupleSpace uts("UTS");

bool connect(int port) {
    return Linda_connect(port);
}

void disconnect() {
    Linda_disconnect();
}

}

