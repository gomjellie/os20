#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "stat.h"

// typedef struct state {
//     int pid;
//     int idx;
//     char stat;
// } state_t;

// int pid2idx[24000];

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // 옵션 인자가 들어온경우 처리
    } 

    printf("Your tty name is : %s\n", ttyname(0)); return 0;
    stat_t stats[256];
    int stats_length;
    stats_update(stats, &stats_length);

    // for (int i = 0; i < stats_length; i++) {
    //     int pid = stats[i].pid;
    //     pid2idx[pid] = i;
    // }

    for (int i = 0; i < stats_length; i++) {
        stat_t stat = stats[i];
        if (stat.state == 'R') {
            int hour = stat.time / 3600;
            int minute = (stat.time - (3600 * hour)) / 60;
            int second = (stat.time - (3600 * hour) - (minute * 60));
            
            // printf(" %5d  %d    %c   %3d:%02d:%02d %s\n", stat.pid, stat.tty_nr, stat.state, hour, minute, second, stat.command);
        }
    }
}
