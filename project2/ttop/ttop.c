#include "stat.h"
#include <stdlib.h>

typedef struct screen {
    int height;
    int width;
    int scroll; // y 축 스크롤정도를 나타냄, 위 아래키를 통해서 조정 가능
} screen_t;

void screen_init(screen_t *this) {
    this->height = this->width = this->scroll = 0;
}

void on_draw(const stat_t stats[], const int stats_len, const screen_t ttop_screen);

int main(int argc, char const * argv[]) {
    initscr();
    noecho();
    curs_set(false);
    timeout(3000);

    stat_t *stats = malloc(sizeof(stat_t) * 256);

    while (true) {
        int stats_length;
        screen_t ttop_screen;
        getmaxyx(stdscr, ttop_screen.height, ttop_screen.width);
        stats_update(stats, &stats_length);

        qsort(stats, stats_length, sizeof(stat_t), stat_cmp);
        on_draw(stats, stats_length, ttop_screen);
        refresh();
        int ch = getch();
        if (ch == 'q') break;

    }
    
    endwin();
    free(stats);

    return 0;
}

/**
 * 드로잉 함수
 */
void on_draw(const stat_t stats[], const int stats_len, const screen_t ttop_screen) {
    for (int i = 1; i < 70; i++)
        mvprintw(0, i, "=");
    mvprintw(1,  0, "    PID |");
    mvprintw(1, 10, "  PR |");
    mvprintw(1, 16, "   S |");
    mvprintw(1, 26, "    %%CPU |");
    mvprintw(1, 41, "    TIME |");
    mvprintw(1, 52, "COMMAND");

    for (int i = 0; i < 70; i++) {
        mvprintw(2, i, "=");
    }

    for (int i = 0; i < stats_len; i++) {
        int hour = stats[i].time / 3600;
        int minute = (stats[i].time - (3600 * hour)) / 60;
        int second = (stats[i].time - (3600 * hour) - (minute * 60));

        mvprintw(i + 3,  0, "%8d |\t", stats[i].pid);
        mvprintw(i + 3, 10, "%4ld |\t", stats[i].priority);
        mvprintw(i + 3, 16, "%4c |\t", stats[i].state);
        mvprintw(i + 3, 26, "%6.2f %% |\t", stats[i].cpu_usage);
        mvprintw(i + 3, 41, "%02d:%02d:%02d |\t", hour, minute, second);
        mvprintw(i + 3, 52, "%s\t", stats[i].command);
    }
}
