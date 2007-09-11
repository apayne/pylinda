#include "linda.h"

int main(int argc, char* argv) {
    Linda_init();

    Linda_connect(Linda_port);

    Linda_finalise();
    return 0;
}
