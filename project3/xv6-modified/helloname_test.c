#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        hello_name("noname");
        exit();
    }
    hello_name(argv[argc - 1]);
    exit();
}
