#include <iostream>

#include <linda.h>

int main(int argc, char* argv[]) {
    if(!Linda::connect()) {
        std::cerr << "Error. Cannot connect to server." << std::endl;
        return -1;
    }

    Linda::Tuple t(1);
    t.set(0, 100);

    Linda::uts.out(t);

    Linda::disconnect();

    return 0;
}
