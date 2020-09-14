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
    attron(COLOR_PAIR(ttop_view.label_color_schema));
    mvprintw(0,  0, "   PID\t");
    mvprintw(0,  6, "  PR\t");
    mvprintw(0, 10, "  NI\t");
    mvprintw(0, 15, " S\t");
    mvprintw(0, 20, "  %%CPU\t");
    mvprintw(0, 30, "  %%MEM\t");
    mvprintw(0, 40, "   TIME+\t");
    mvprintw(0, 50, "COMMAND");
    attroff(COLOR_PAIR(ttop_view.label_color_schema));
    
    for (int i = ttop_view.scroll; i < stats_len && ttop_view.body_top + i - ttop_view.scroll < ttop_view.height; i++) {
        int hour = stats[i].time / 3600;
        int minute = (stats[i].time - (3600 * hour)) / 60;
        int second = (stats[i].time - (3600 * hour) - (minute * 60));

        mvprintw(ttop_view.body_top + i - ttop_view.scroll,  0, "%6d\t", stats[i].pid);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll,  6, "%4ld\t", stats[i].priority);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll, 10, "%3ld\t", stats[i].nice);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll, 15, "%2c\t", stats[i].state);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll, 20, "%6.2f\t", stats[i].cpu_usage);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll, 30, "%6.2f\t", stats[i].mem_usage);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll, 40, "%02d:%02d:%02d\t", hour, minute, second);
        mvprintw(ttop_view.body_top + i - ttop_view.scroll, 50, "%s\t", stats[i].command);
    }
}

void view_init(view_t *this) {
    this->height = this->width = this->scroll = 0;
    this->body_top = 1;
    this->label_color_schema = 1;
    this->body_color_schema = 2;
    init_pair(this->label_color_schema, COLOR_BLACK, COLOR_WHITE);
    init_pair(this->body_color_schema, COLOR_WHITE, COLOR_BLACK);
}

void view_scroll(view_t *this, direction_t to) {
    if (to == DOWN) {
        if (this->scroll < this->height)
            this->scroll ++;
    }
    if (to == UP) {
        if (0 < this->scroll)
            this->scroll --;
    }
}
