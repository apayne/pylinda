#include "linda.h"

int main(int argc, char* argv[]) {
    if(!Linda_connect()) {
        printf("Failed to connect to server.\n");
        return 1;
    }

    Linda_disconnect();

    return 0;
}
