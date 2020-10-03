#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int max_pid = get_max_pid();
    printf(1, "Maximum PID: %d\n", max_pid);
    exit();
}
