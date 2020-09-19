#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "stat.h"

#define DEFAULT_FORMAT "%5s %s\t%8s %s\n"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // 옵션 인자가 들어온경우 처리
    } 

    stat_t stats[256];
    int stats_length;
    stats_update(stats, &stats_length);

    int fd = open("/proc/self/fd/0", O_RDONLY);
    char self_tty[32], time_s[32];
    sprintf(self_tty, "%s", ttyname(fd));

    printf("  PID TTY          TIME CMD\n");
    for (int i = 0; i < stats_length; i++) {
        stat_t stat = stats[i];
        if (strcmp(stat.tty, self_tty) == 0) {
            int hour = stat.time / 3600;
            int minute = (stat.time - (3600 * hour)) / 60;
            int second = (stat.time - (3600 * hour) - (minute * 60));
            
            sprintf(time_s, "%02d:%02d:%02d", hour, minute, second);
            printf("%5d %s    %8s %s\n", stat.pid, stat.tty + 5, time_s, stat.command);
        }
    }
}
