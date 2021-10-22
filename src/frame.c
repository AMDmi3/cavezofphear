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


#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

void curses_start(void);
void curses_stop(void);
void bail(char* message);
void sigint_handler();
void sigwinch_handler();

extern int need_refresh;

void curses_start(void) {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	start_color();

	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);

	/* erase(); */
	refresh();

	flushinp();
}


void curses_stop(void) {
	erase();
	refresh();
	echo();
	nocbreak();
	endwin();
}


void bail(char* message) {
	curses_stop();
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}


void sigint_handler() {
	curses_stop();
	bail("got SIGINT, cleaning up");
}


void sigwinch_handler() {
	/*
	   curses_stop();
	   bail("got SIGWINCH, cleaning up");
	 */

	need_refresh = 1;

	endwin();
	curses_start();
	return;
}

