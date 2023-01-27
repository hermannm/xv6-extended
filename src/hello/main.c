#include "../../lib/user/user.h"
#include "greetings.h"

int main(int argc, char** argv)
{
    if (argc == 1) {
        print_hello_world();
    } else {
        greet_user(argv[1]);
    }

    exit(0);
}
