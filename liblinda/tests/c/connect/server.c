#include "linda.h"

int main(int argc, char* argv[]) {
    if(!Linda_serve()) {
        return 1;
    }

    Linda_accept();

    Linda_server_disconnect();

    return 0;
}
