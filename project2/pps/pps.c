#include <ncurses.h>
#include <stdlib.h>

#include "stat.h"

// typedef struct state {
//     int pid;
//     int idx;
//     char stat;
// } state_t;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // 옵션 인자가 들어온경우 처리
    } 

    stat_t stats[256];
    int stats_length;
    stats_update(stats, &stats_length);

    for (int i = 0; i < stats_length; i++) {
        if (stats[i].state != 'S') {
            int hour = stats[i].time / 3600;
            int minute = (stats[i].time - (3600 * hour)) / 60;
            int second = (stats[i].time - (3600 * hour) - (minute * 60));
            
            printf(" %5d          %3d:%02d:%02d %s\n", stats[i].pid, hour, minute, second, stats[i].command);
        }
    }
}
