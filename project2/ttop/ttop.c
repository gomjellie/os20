#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <sys/sysinfo.h>

#include "stat.h"

typedef struct banner {
    char uptime[32];
    char current_time[16];
    char load_avg[16];
    char ram_total[16];
    char ram_free[16];
    char ram_used[16];
    char swap_total[16];
    char swap_free[16];
    char swap_used[16];

    state_count_t state_count;
} banner_t;

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

banner_t *banner_new();
void banner_update(banner_t *this, const stat_t stats[], const int stats_len);

view_t *view_new();
void view_scroll(view_t *this, direction_t to);
void on_draw(const view_t *ttop_view, const stat_t stats[], const int stats_len, const banner_t *banner);

int main(int argc, char const * argv[]) {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(false);
    start_color();
    timeout(3000);

    stat_t *stats = malloc(sizeof(stat_t) * 256);
    view_t *ttop_view = view_new();
    banner_t *banner = banner_new();

    while (true) {
        int stats_length;
        getmaxyx(stdscr, ttop_view->height, ttop_view->width);
        stats_update(stats, &stats_length);
        banner_update(banner, stats, stats_length);

        qsort(stats, stats_length, sizeof(stat_t), stat_cmp);
        clear();
        on_draw(ttop_view, stats, stats_length, banner);
        int ch = getch();
        switch (ch) {
            case 'q':
            case 'Q':
            goto QUIT;
            case KEY_UP:
            view_scroll(ttop_view, UP);
            break;
            case KEY_DOWN:
            view_scroll(ttop_view, DOWN);
            break;
        }
    }

QUIT:
    endwin();
    free(stats);
    free(ttop_view);
    free(banner);

    return 0;
}

/**
 * 드로잉 함수
 */
void on_draw(const view_t *ttop_view, const stat_t stats[], const int stats_len, const banner_t *banner) {
//   PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
    const banner_t bnr = *banner; // banner
    const view_t view = *ttop_view;
    mvprintw(0,  0, "ttop - %s up %s,  0 users,  load average: %s", bnr.current_time, bnr.uptime, bnr.load_avg);
    mvprintw(1,  0, "Tasks:  %d total,   %d running,  %d sleeping,   %d stopped,   %d zombie",
                    stats_len, bnr.state_count.running, bnr.state_count.sleeping, bnr.state_count.stopped, bnr.state_count.zombie);
    mvprintw(2,  0, "%%Cpu(s): 12.3 us,  5.2 sy,  0.0 ni, 82.1 id,  0.0 wa,  0.5 hi,  0.0 si,  0.0 st");
    mvprintw(3,  0, "MiB Mem :  %s total,  %s free,  %s used,    224.0 buff/cache", bnr.ram_total, bnr.ram_free, bnr.ram_used);
    mvprintw(4,  0, "MiB Swap:  %s total,  %s free,  %s used.  19562.7 avail Mem", bnr.swap_total, bnr.swap_free, bnr.swap_used);
    
    attron(COLOR_PAIR(view.label_color_schema));
    int coord_y = view.body_top - 1;
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
    attroff(COLOR_PAIR(view.label_color_schema));
    use_default_colors();
    
    for (int i = view.scroll; i < stats_len && view.body_top + i - view.scroll < view.height; i++) {
        stat_t stat = stats[i];
        int hour = stat.time / 3600;
        int minute = (stat.time - (3600 * hour)) / 60;
        int second = (stat.time - (3600 * hour) - (minute * 60));

        coord_y = view.body_top + i - view.scroll;
        mvprintw(coord_y,  0, "%6d\t", stat.pid);
        mvprintw(coord_y,  6, "%4ld\t", stat.priority);
        mvprintw(coord_y, 10, "%4ld\t", stat.nice);
        mvprintw(coord_y, 19, "%5lu\t", (unsigned long)(stat.vsize / 1024UL));
        mvprintw(coord_y, 23, "%7ld\t", stat.rss);
        mvprintw(coord_y, 29, "%7d\t", stat.shared);
        mvprintw(coord_y, 37, "%1c\t", stat.state);
        mvprintw(coord_y, 40, "%2.2f\t", stat.cpu_usage);
        mvprintw(coord_y, 46, "%2.2f\t", stat.mem_usage);
        mvprintw(coord_y, 55, "%3d:%02d:%02d\t", hour, minute, second);
        mvprintw(coord_y, 65, "%s\t", stat.command);


        // struct passwd *p = getpwuid((uid_t)stat.pid);

        // if (p != NULL) {
        //     mvprintw(coord_y, 80, "%s", p->pw_name);
        //     // mvprintw(coord_y, 80, "%s", g->gr_name);
        // }
    }
}

view_t *view_new() {
    view_t *this = malloc(sizeof(view_t));
    
    this->height = this->width = this->scroll = 0;
    this->body_top = 7;
    this->label_color_schema = 1;
    this->body_color_schema = 2;
    init_pair(this->label_color_schema, COLOR_BLACK, COLOR_WHITE);
    init_pair(this->body_color_schema, COLOR_WHITE, COLOR_BLACK);

    return this;
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

banner_t *banner_new() {
    banner_t *this = malloc(sizeof(banner_t));
    this->state_count.zombie = 0;
    this->state_count.running = 0;
    this->state_count.sleeping = 0;
    this->state_count.stopped = 0;
    
    return this;
}

void banner_update(banner_t *this, const stat_t stats[], const int stats_len) {
    struct sysinfo info;
    sysinfo(&info);

    int days = info.uptime / 86400;
    int hours = (info.uptime / 3600) - (days * 24);
    int mins = (info.uptime / 60) - (days * 1440) - (hours * 60);
    int unit = 1024;
    float loads[3];

    FILE *loadavg_file = fopen("/proc/loadavg", "r");

    if (!loadavg_file)
        return;

    fscanf(loadavg_file, "%f %f %f", &loads[0], &loads[1], &loads[2]);
    fclose(loadavg_file);

    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    sprintf(this->current_time, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    sprintf(this->uptime, "%d days, %02d:%02d", days, hours, mins);

    sprintf(this->load_avg, "%.2f %.2f %.2f", loads[0], loads[1], loads[2]);

    sprintf(this->ram_total, "%ld", info.totalram / unit);
    sprintf(this->ram_used, "%ld", (info.totalram - info.freeram) / unit);
    sprintf(this->ram_free, "%ld", info.freeram / unit);

    sprintf(this->swap_total, "%ld", info.totalswap / unit);
    sprintf(this->swap_used, "%ld", (info.totalswap - info.freeswap) / unit);
    sprintf(this->swap_free, "%ld", info.freeswap / unit);

    stats_count_state(stats, stats_len, &this->state_count);
}
