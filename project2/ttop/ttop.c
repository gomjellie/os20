#include "stat.h"
#include <stdlib.h>

typedef struct view {
    int height;
    int width;
    int scroll; // y 축 스크롤정도를 나타냄, 위 아래키를 통해서 조정 가능
} view_t;

void screen_init(view_t *this) {
    this->height = this->width = this->scroll = 0;
}

void on_draw(const stat_t stats[], const int stats_len, const view_t ttop_view);

int main(int argc, char const * argv[]) {
    initscr();
    noecho();
    curs_set(false);
    timeout(3000);

    stat_t *stats = malloc(sizeof(stat_t) * 256);
    view_t ttop_view;
    screen_init(&ttop_view);

    while (true) {
        int stats_length;
        getmaxyx(stdscr, ttop_view.height, ttop_view.width);
        stats_update(stats, &stats_length);

        qsort(stats, stats_length, sizeof(stat_t), stat_cmp);
        on_draw(stats, stats_length, ttop_view);
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
void on_draw(const stat_t stats[], const int stats_len, const view_t ttop_view) {
//   PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND     
    mvprintw(0,  0, "   PID\t");
    mvprintw(0,  5, "  PR\t");
    mvprintw(0, 10, "  NI\t");
    mvprintw(0, 15, " S\t");
    mvprintw(0, 20, "  %%CPU\t");
    mvprintw(0, 28, "   TIME+\t");
    mvprintw(0, 52, "COMMAND");

    for (int i = 0; i < stats_len; i++) {
        int hour = stats[i].time / 3600;
        int minute = (stats[i].time - (3600 * hour)) / 60;
        int second = (stats[i].time - (3600 * hour) - (minute * 60));

        mvprintw(i + 1,  0, "%6d\t", stats[i].pid);
        mvprintw(i + 1,  5, "%4ld\t", stats[i].priority);
        mvprintw(i + 1, 10, "%3ld\t", stats[i].nice);
        mvprintw(i + 1, 15, "%2c\t", stats[i].state);
        mvprintw(i + 1, 20, "%6.2f\t", stats[i].cpu_usage);
        mvprintw(i + 1, 28, "%02d:%02d:%02d\t", hour, minute, second);
        mvprintw(i + 1, 52, "%s\t", stats[i].command);
    }
}
