#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"

int main(int argc, char *argv[])
{
    int prio = 0;
    if (set_prio(20) < 0) {
        exit();
    }
    prio = get_prio();
    printf(1, "prio: %d, expected value = 20\n", prio);

    exit();
}
