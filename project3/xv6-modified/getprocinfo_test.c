#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"
#include "processInfo.h"

int main(int argc, char *argv[])
{
    printf(1, "PID\t  PPID\t  SIZE\t  Number of Context Switch\n");
    for (int pid = 1; pid < NPROC; pid++) {
        struct processInfo pInfo;
        if (get_proc_info(pid, &pInfo) == -1) continue;

        printf(1, "%d\t  %d\t  %d\t  %d\n", pid, pInfo.ppid, pInfo.psize, pInfo.numberContextSwitches);
    }
    exit();
}
