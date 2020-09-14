#include "ttop.h"

int main(int argc, char const * argv[]) {
	initscr();

	while(true) {
		stat_t *stats = malloc(sizeof(stat_t) * 300);
		int stat_length;
		stat_parse(&stats, &stat_length);
		sort_proc(stats, stat_length - 1);
     	on_draw(stats, 15);
		refresh();
		sleep(3);
		free(stats);
	}
	
	getch();
	endwin();

	return 0;
}
