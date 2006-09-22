#include "linda.h"

char* return_yes_no(unsigned char t);

int main(int argc, char* argv[]) {
    Value v;

    printf("Setting value to an integer...\n");
    Value_set_integer(&v, 100);
    printf("Is this a string?...   %s\n", return_yes_no(Value_is_string(&v)));
    printf("Is this an integer?... %s\n", return_yes_no(Value_is_integer(&v)));
    printf("Get integer value... %i\n", Value_get_integer(&v));

    return 0;
}

char* return_yes_no(unsigned char t) {
    if(t) {
        return "yes";
    } else {
        return "no";
    }
}
