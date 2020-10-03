#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int num_proc = get_num_proc();
    printf(1, "Total Number of Active Processes: %d\n", num_proc);
    exit();
}
