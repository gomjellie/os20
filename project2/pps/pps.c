#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#include "proc.h"
#include "dev.h"

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
    
    * u      Display user-oriented format.
    
     */
    OPTION_NONE = 0,
    OPTION_A = 1, // a
    OPTION_U = 2, // u 
    OPTION_X = 4, // x
} option_t;

void render(const proc_t *proc, const cdev_t *dev, const int option);

int main(int argc, char *argv[]) {
    int options = OPTION_NONE;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            char *opt = argv[i];
            size_t opt_len = strlen(opt);

            for (int j = 0; j < opt_len; j++) {
                if (opt[j] == '-') break;
                switch (opt[j]) {
                    case 'a':
                    options |= OPTION_A; break;
                    case 'u':
                    options |= OPTION_U; break;
                    case 'x':
                    options |= OPTION_X; break;
                }
            }
        }
    }
    
    cdev_t *dev = cdev_new(128);
    proc_t *proc = proc_new(256);
    cdev_update(dev);
    proc_update(proc);
    
    initscr();
    render(proc, dev, options);
    getch();

    cdev_del(dev);
    proc_del(proc);
    endwin();
}

void render(const proc_t *proc, const cdev_t *dev, const int option) {
    int fd = open("/proc/self/fd/0", O_RDONLY);
    char self_tty[32], time_s[32];
    int line = 1;
    int x = 0;
    sprintf(self_tty, "%s", ttyname(fd));

    mvprintw(0,  x, "  PID");     x += 6;
    mvprintw(0,  x, "TTY");       x += 9;

    if (option & OPTION_A) {
        mvprintw(0, x, "STAT"); x += 7;
    }
    mvprintw(0,  x, "     TIME"); x += 10;
    if (option & OPTION_A) {
        mvprintw(0, x, "COMMAND"); x += 20;
    } else {
        mvprintw(0, x, "CMD"); x += 10;
    }
    
    for (int i = 0; i < proc->processes_length; i++) {
        stat_t stat = *proc->processes[i]->stat;
        const char *tty_name = cdev_find(dev, stat.tty_nr);
        x = 0;
        
        int condition_a = tty_name != NULL || strcmp(stat.tty, self_tty) == 0;
        int condition_none = strcmp(stat.tty, self_tty) == 0;
        
        // filter here
        if ((option & OPTION_A) && !condition_a) continue;
        if ((option == OPTION_NONE) && !condition_none) continue;

        int hour = stat.time / 3600;
        int minute = (stat.time - (3600 * hour)) / 60;
        int second = (stat.time - (3600 * hour) - (minute * 60));
        
        // render here
        sprintf(time_s, "%03d:%02d:%02d", hour, minute, second);
        mvprintw(line, x, "%5d", stat.pid); x += 6;
        mvprintw(line, x, "%5s", tty_name); x += 9;

        if (option & OPTION_A) {
            mvprintw(line, x, "%c", stat.state); x += 7;
        }
        mvprintw(line, x, "%8s", time_s);   x += 10;

        if ((option & OPTION_A)) {
            mvprintw(line, x, "%s\n", proc->processes[i]->cmdline->string); x += 20;
        } else {
            mvprintw(line, x, "%s\n", stat.command); x += 10;
        }
        
        ++line;
    }
}
