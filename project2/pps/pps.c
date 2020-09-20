#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "proc.h"

#define DEFAULT_FORMAT "%5s %s\t%8s %s\n"

typedef enum {
    /**
     * a
    
    Lift the BSD-style "only yourself" restriction, which is imposed upon the set of all processes when some BSD-style (without "-") options are used or when the ps personality setting is
    BSD-like.  The set of processes selected in this manner is in addition to the set of processes selected by other means.  An alternate description is that this option causes ps to list all
    processes with a terminal (tty), or to list all processes when used together with the x option.
    
     *  x
    Lift the BSD-style "must have a tty" restriction, which is imposed upon the set of all processes when some BSD-style (without "-") options are used or when the ps personality setting is
    BSD-like.  The set of processes selected in this manner is in addition to the set of processes selected by other means.  An alternate description is that this option causes ps to list all
    processes owned by you (same EUID as ps), or to list all processes when used together with the a option.
     * U userlist
    
    Select by effective user ID (EUID) or name.  This selects the processes whose effective user name or ID is in userlist.  The effective user ID describes the user whose file access
    permissions are used by the process (see geteuid(2)).  Identical to -u and --user.
    
    * u      Display user-oriented format.
    
     */
    OPTION_NONE = 0,
    OPTION_A = 1, // A
    OPTION_U = 2, // U 
    OPTION_X = 4, // X
} option_t;

void render(const proc_t *proc, const int option);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // 옵션 인자가 들어온경우 처리
    }

    proc_t *proc = proc_new(256);
    proc_update(proc);
    
    render(proc, OPTION_NONE);
}

void render(const proc_t *proc, const int option) {
    int fd = open("/proc/self/fd/0", O_RDONLY);
    char self_tty[32], time_s[32];
    sprintf(self_tty, "%s", ttyname(fd));

    printf("  PID TTY          TIME CMD\n");
    for (int i = 0; i < proc->processes_length; i++) {
        stat_t stat = *proc->processes[i]->stat;
        if (strcmp(stat.tty, self_tty) == 0) {
            int hour = stat.time / 3600;
            int minute = (stat.time - (3600 * hour)) / 60;
            int second = (stat.time - (3600 * hour) - (minute * 60));
            
            sprintf(time_s, "%02d:%02d:%02d", hour, minute, second);
            printf("%5d %5s    %8s %s\n", stat.pid, stat.tty, time_s, stat.command);
        }
    }
}
