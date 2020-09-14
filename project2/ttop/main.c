#include "ttop.h"

int main(int argc, char const * argv[]) {
	initscr();

	while(true) {
		stat_t *stats = malloc(sizeof(stat_t) * 300);
		stat_parse(&stats);
		refresh();
		sleep(3);
		free(stats);
	}
	
	getch();
	endwin();

	return 0;
}
