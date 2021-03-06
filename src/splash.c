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

#include "splash.h"

#include "misc.h"
#include "map/map.h"  // XXX: untie from map, use window size instead
#include "gplot.h"

#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void splash(void) {
	attrset(COLOR_PAIR(COLOR_RED) | A_BOLD);
	gplot("htext", 20, 1, 1);

	attrset(COLOR_PAIR(COLOR_GREEN));
	gplot("tdesc", 42, 9, 1);

	attrset(COLOR_PAIR(COLOR_RED));
	mvprintw(4, MAP_XSIZE - strlen(VERSION), "%s", VERSION);

	attrset(COLOR_PAIR(COLOR_CYAN));
	for (int x = 0; x < MAP_XSIZE; x++) {
		mvaddch(MAP_YSIZE - 1, x, ':');
		mvaddch(0, x, ':');
	}

	attrset(COLOR_PAIR(COLOR_MAGENTA));

	mvprintw(MAP_YSIZE, 0, "CAVEZ of PHEAR Copyright 2003-2011 Tom Rune Flo <tom@x86.no>");
	mvprintw(MAP_YSIZE + 1, 0, "Distributed under the terms of the GPL license");

	refresh();

	attrset(COLOR_PAIR(COLOR_WHITE) | A_NORMAL);

	mysleep(100000);

	for (int x = -40; x < 0; x++) {
		gplot("spgraf", x, 1, 0);
		refresh();
		mysleep(7000);
	}

	attrset(COLOR_PAIR(COLOR_YELLOW));
	mvprintw(15, 42, "PRESS ANY KEY TO START!");

	attrset(A_NORMAL);

	refresh();

	flushinp();
	getch();

	return;
}

void gameover(void) {
	fade_dissolv();

	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	gplot("gover", 18, 2, 1);

	refresh();
}
