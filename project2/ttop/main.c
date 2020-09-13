#include "ttop.h"

int main(int argc, char const * argv[]) {

	int row, col;

	initscr();
	getmaxyx(stdscr, row, col);

	while(true) {

		read_all_proc(row, col);
		refresh();
		sleep(1);
	}
	
	getch();
	endwin();

	return 0;
}
