#include "stat.h"
#include <stdlib.h>

void on_draw(stat_t stats[], int stats_len);

int main(int argc, char const * argv[]) {
    initscr();
    noecho();
    curs_set(false);
    timeout(3000);

    while (true) {
        stat_t *stats = malloc(sizeof(stat_t) * 300);
        int stat_length;
        stat_parse(stats, &stat_length);
        
        qsort(stats, stat_length, sizeof(stat_t), stat_cmp);
          on_draw(stats, stat_length);
        refresh();
        int ch = getch();
        if (ch == 'q') break;
        
        free(stats);
    }
    
    endwin();

    return 0;
}

/**
 * 드로잉 함수
 */
void on_draw(stat_t stats[], int stats_len) {
     int hour, minute, second;

     for (int i = 1; i < 70; i++) {
          mvprintw(0, i, "=");
     }

     mvprintw(1,  0, "     PID |");
     mvprintw(1, 10, "  PR |");
     mvprintw(1, 16, "   S |");
     mvprintw(1, 26, "    %%CPU |");
     mvprintw(1, 41, "    TIME |");
     mvprintw(1, 52, "COMMAND");

     for (int i = 0; i < 70; i++) {
          mvprintw(2, i, "=");
     }

     for (int i = 0; i < stats_len; i++) {
          hour = stats[i].time / 3600;
          minute = (stats[i].time - (3600 * hour)) / 60;
          second = (stats[i].time - (3600 * hour) - (minute * 60));

          mvprintw(i + 3,  0, "%8d |\t", stats[i].pid);
          mvprintw(i + 3, 10, "%4ld |\t", stats[i].priority);
          mvprintw(i + 3, 16, "%4c |\t", stats[i].state);
          mvprintw(i + 3, 26, "%6.2f %% |\t", stats[i].cpu_usage);
          mvprintw(i + 3, 41, "%02d:%02d:%02d |\t", hour, minute, second);
          mvprintw(i + 3, 52, "%s\t", stats[i].command);
     }
}
