/*
    Copyright 2003-2011 Tom Rune Flo

    This file is part of CAVEZ OF PHEAR

    CAVEZ OF PHEAR is free software: you can redistribute it and/or modify it under the terms of the GNU
    General Public License as published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CAVEZ OF PHEAR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along with CAVEZ OF PHEAR. If not,
    see http://www.gnu.org/licenses/.
 */

#include "misc.h"
#include "common.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int calc_center(int slen) {
	return (MAP_XSIZE / 2) - (slen / 2);
}

int msgbox(const char* message) {
	const int len = strlen(message);

	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);

	for (int x = -5; x < len + 5; x++) {
		mvaddch(MAP_YSIZE / 2 - 2, (MAP_XSIZE / 2) - (len / 2) + x, '+');
		mvaddch(MAP_YSIZE / 2 - 1, (MAP_XSIZE / 2) - (len / 2) + x, '+');
		mvaddch(MAP_YSIZE / 2 + 0, (MAP_XSIZE / 2) - (len / 2) + x, '+');
		mvaddch(MAP_YSIZE / 2 + 1, (MAP_XSIZE / 2) - (len / 2) + x, '+');
		mvaddch(MAP_YSIZE / 2 + 2, (MAP_XSIZE / 2) - (len / 2) + x, '+');
	}

	for (int x = -3; x < len + 3; x++) {
		mvaddch(MAP_YSIZE / 2 - 1, (MAP_XSIZE / 2) - (len / 2) + x, ' ');
		mvaddch(MAP_YSIZE / 2 + 0, (MAP_XSIZE / 2) - (len / 2) + x, ' ');
		mvaddch(MAP_YSIZE / 2 + 1, (MAP_XSIZE / 2) - (len / 2) + x, ' ');
	}

	attrset(COLOR_PAIR(COLOR_WHITE) | A_NORMAL);

	mvprintw(MAP_YSIZE / 2 + 0, (MAP_XSIZE / 2) - (len / 2), "%s", message);

	attrset(A_NORMAL);

	return wait_for_input();
}

int wait_for_input(void) {
	flushinp();
	return getch();
}

void fade_dissolv(void) {
	for (int i = 0; i < 1000 * 10; i++) {
		mvaddch(rand() % (MAP_YSIZE + 2), rand() % MAP_XSIZE, ' ');
		refresh();
	}

	erase();
	refresh();
}

void mysleep(long nsecs) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = nsecs * 1000;
	nanosleep(&ts, NULL);
	return;
}
