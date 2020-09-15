#include <ncurses.h>
#include <stdlib.h>

#include "stat.h"

typedef struct view {
    int height;
    int width;
    int scroll; // y 축 스크롤 위치를 나타냄, 화살표 방향키를 통해서 조정 가능
    int body_top; // process들이 표시되는 body 시작 y좌표
    int label_color_schema; // PID USER PR ... 라벨쪽 색상
    int body_color_schema;
} view_t;

typedef enum direction {
    UP, DOWN
} direction_t;

void view_init(view_t *this);
void view_scroll(view_t *this, direction_t to);
void on_draw(const stat_t stats[], const int stats_len, const view_t ttop_view);

int main(int argc, char const * argv[]) {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(false);
    start_color();
    timeout(3000);

    stat_t *stats = malloc(sizeof(stat_t) * 256);
    view_t ttop_view;
    view_init(&ttop_view);

    while (true) {
        int stats_length;
        getmaxyx(stdscr, ttop_view.height, ttop_view.width);
        stats_update(stats, &stats_length);

        qsort(stats, stats_length, sizeof(stat_t), stat_cmp);
        clear();
        on_draw(stats, stats_length, ttop_view);
        int ch = getch();
        switch (ch) {
            case 'q':
            case 'Q':
            goto QUIT;
            case KEY_UP:
            view_scroll(&ttop_view, UP);
            break;
            case KEY_DOWN:
            view_scroll(&ttop_view, DOWN);
            break;
        }
    }

QUIT:
    endwin();
    free(stats);

    return 0;
}

/**
 * 드로잉 함수
 */
void on_draw(const stat_t stats[], const int stats_len, const view_t ttop_view) {
//   PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND     
    mvprintw(0,  0, "ttop - 04:56:35 up  8:03,  0 users,  load average: 0.52, 0.58, 0.59");
    mvprintw(1,  0, "Tasks:  %d total,   1 running,  11 sleeping,   0 stopped,   0 zombie", stats_len);
    mvprintw(2,  0, "%%Cpu(s): 12.3 us,  5.2 sy,  0.0 ni, 82.1 id,  0.0 wa,  0.5 hi,  0.0 si,  0.0 st");
    mvprintw(3,  0, "MiB Mem :  30822.0 total,  19477.0 free,  11121.0 used,    224.0 buff/cache");
    mvprintw(4,  0, "MiB Swap:  29639.6 total,  29639.6 free,      0.0 used.  19562.7 avail Mem");
    
    attron(COLOR_PAIR(ttop_view.label_color_schema));
    int coord_y = ttop_view.body_top - 1;
    mvprintw(coord_y,  0, "   PID");
    mvprintw(coord_y,  6, "  PR");
    mvprintw(coord_y, 10, "  NI\t\t");
    mvprintw(coord_y, 19, "VIRT");
    mvprintw(coord_y, 23, "   RES");
    mvprintw(coord_y, 29, "    SHR\t\t");
    mvprintw(coord_y, 37, "S\t");
    mvprintw(coord_y, 40, "%%CPU\t");
    mvprintw(coord_y, 46, "%%MEM\t");
    mvprintw(coord_y, 55, "     TIME\t\t");
    mvprintw(coord_y, 65, "COMMAND\t");
    attroff(COLOR_PAIR(ttop_view.label_color_schema));
    use_default_colors();
    
    for (int i = ttop_view.scroll; i < stats_len && ttop_view.body_top + i - ttop_view.scroll < ttop_view.height; i++) {
        int hour = stats[i].time / 3600;
        int minute = (stats[i].time - (3600 * hour)) / 60;
        int second = (stats[i].time - (3600 * hour) - (minute * 60));

        coord_y = ttop_view.body_top + i - ttop_view.scroll;
        mvprintw(coord_y,  0, "%6d\t", stats[i].pid);
        mvprintw(coord_y,  6, "%4ld\t", stats[i].priority);
        mvprintw(coord_y, 10, "%4ld\t", stats[i].nice);
        mvprintw(coord_y, 19, "%5lu\t", (unsigned long)(stats[i].vsize / 1024UL));
        mvprintw(coord_y, 23, "%7ld\t", stats[i].rss);
        mvprintw(coord_y, 29, "%7d\t", stats[i].shared);
        mvprintw(coord_y, 37, "%1c\t", stats[i].state);
        mvprintw(coord_y, 40, "%2.2f\t", stats[i].cpu_usage);
        mvprintw(coord_y, 46, "%2.2f\t", stats[i].mem_usage);
        mvprintw(coord_y, 55, "%3d:%02d:%02d\t", hour, minute, second);
        mvprintw(coord_y, 65, "%s\t", stats[i].command);
    }
}

void view_init(view_t *this) {
    this->height = this->width = this->scroll = 0;
    this->body_top = 7;
    this->label_color_schema = 1;
    this->body_color_schema = 2;
    init_pair(this->label_color_schema, COLOR_BLACK, COLOR_WHITE);
    init_pair(this->body_color_schema, COLOR_WHITE, COLOR_BLACK);
}

void view_scroll(view_t *this, direction_t to) {
    if (to == DOWN) {
        this->scroll ++;
    }
    if (to == UP) {
        if (0 < this->scroll)
            this->scroll --;
    }
}
