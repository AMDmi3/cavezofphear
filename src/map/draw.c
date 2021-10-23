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

void draw_map(void) {
	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			if (map[y][x] == MAP_EMPTY) {
				mvaddch(y + 1, x, CHR_EMPTY);
			}
			if (map[y][x] == MAP_DIRT) {
				mvaddch(y + 1, x, CHR_DIRT);
			}
			if (map[y][x] == MAP_WALL) {
				mvaddch(y + 1, x, CHR_WALL);
			}
			if (map[y][x] == MAP_PLAYER) {
				mvaddch(y + 1, x, CHR_PLAYER);
			}
			if (map[y][x] == MAP_MONSTER) {
				mvaddch(y + 1, x, CHR_MONSTER);
			}
			if (map[y][x] == MAP_STONE) {
				mvaddch(y + 1, x, CHR_STONE);
			}
			if (map[y][x] == MAP_DIAMOND) {
				mvaddch(y + 1, x, CHR_DIAMOND);
			}
			if (map[y][x] == MAP_MONEY) {
				mvaddch(y + 1, x, CHR_MONEY);
			}
			if (map[y][x] == MAP_BOMB) {
				mvaddch(y + 1, x, CHR_BOMB);
			}
			if (map[y][x] == MAP_BOMBPK) {
				mvaddch(y + 1, x, CHR_BOMBPK);
			}
		}
	}
}
