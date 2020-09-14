#include "ttop.h"

int main(int argc, char const * argv[]) {
	initscr();
	noecho();
	curs_set(false);
	timeout(3);

	while (true) {
		stat_t *stats = malloc(sizeof(stat_t) * 300);
		int stat_length;
		stat_parse(&stats, &stat_length);
		sort_proc(stats, stat_length - 1);
     	on_draw(stats, 15);
		refresh();
		int ch = getch();
		if (ch == 'q') break;
		// sleep(3);
		free(stats);
	}
	
	endwin();

	return 0;
}
