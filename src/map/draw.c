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

#include "map.h"

#include <ncurses.h>

int get_map_symbol(int object_type) {
	switch (object_type) {
	case MAP_DIRT:
		return '#' | COLOR_PAIR(COLOR_RED)    | A_NORMAL;
	case MAP_STONE:
		return 'O' | COLOR_PAIR(COLOR_WHITE)  | A_NORMAL;
	case MAP_WALL:
		return ':' | COLOR_PAIR(COLOR_CYAN)   | A_NORMAL;
	case MAP_DIAMOND:
		return '*' | COLOR_PAIR(COLOR_YELLOW) | A_BOLD;
	case MAP_MONEY:
		return '$' | COLOR_PAIR(COLOR_GREEN)  | A_NORMAL;
	case MAP_PLAYER:
		return 'Z' | COLOR_PAIR(COLOR_WHITE)  | A_BOLD;
	case MAP_BOMB:
		return '@' | COLOR_PAIR(COLOR_BLUE)   | A_BOLD;
	case MAP_BOMBPK:
		return '%' | COLOR_PAIR(COLOR_BLUE)   | A_BOLD;
	case MAP_MONSTER:
		return 'M' | COLOR_PAIR(COLOR_MAGENTA)| A_BOLD;
	case MAP_EMPTY:
	default:
		return ' ';
	}
}

void draw_map(void) {
	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			mvaddch(y + 1, x, get_map_symbol(map[y][x]));
		}
	}
}
