#include "ttop.h"

int main(int argc, char const * argv[]) {
	// TODO: fork로 자식만들고 자식은 계속 stats 3초마다 갱신하게, 부모는 drawing 계속하게...
	initscr();
	noecho();
	curs_set(false);

	while (true) {
		stat_t *stats = malloc(sizeof(stat_t) * 300);
		int stat_length;
		stat_parse(&stats, &stat_length);
		sort_proc(stats, stat_length - 1);
     	on_draw(stats, 15);
		refresh();
		int ch = getch();
		if (ch == 'q') return 0;
		sleep(3);
		free(stats);
	}
	
	endwin();

	return 0;
}
