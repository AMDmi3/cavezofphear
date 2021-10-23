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

#include "editor.h"

#include "common.h"
#include "frame.h"
#include "map/map.h"
#include "misc.h"

#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

extern char map[MAP_YSIZE][MAP_XSIZE];
int lock;
int last_obj;

void editor_draw_status(void);

int editor_main(const char* file) {
	if (!file) {
		fprintf(stderr, "usage: phear -e <file>\n");
		exit(1);
	}

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			map[y][x] = 0;
		}
	}

	curses_start();
	curs_set(1);

	if (COLS < 80 || LINES < 25) {
		bail("error: your terminal size must be at least 80x25");
	}

	signal(SIGINT, sigint_handler);
	signal(SIGWINCH, sigwinch_handler);

	FILE* fp = fopen(file, "r");
	if (fp != NULL) {
		fclose(fp);
		load_map(file);
	}

	for (int x = 0; x < MAP_XSIZE; x++) {
		map[0][x] = MAP_WALL;
	}
	for (int x = 0; x < MAP_XSIZE; x++) {
		map[MAP_YSIZE - 1][x] = MAP_WALL;
	}
	for (int y = 0; y < MAP_YSIZE; y++) {
		map[y][0] = MAP_WALL;
	}
	for (int y = 0; y < MAP_YSIZE; y++) {
		map[y][MAP_XSIZE - 1] = MAP_WALL;
	}
	for (int y = 0; y < MAP_YSIZE; y++) {
		map[y][1] = MAP_WALL;
	}
	for (int y = 0; y < MAP_YSIZE; y++) {
		map[y][MAP_XSIZE - 2] = MAP_WALL;
	}

	int x = 2;
	int y = 2;

	lock = 0;

	last_obj = 0;

	editor_draw_status();

	while (1) {
		if (count_map_objects(MAP_PLAYER) == 0) {
			map[1][1] = MAP_PLAYER;
		}

		draw_map();
		refresh();

		int input = mvgetch(y, x);

		if (input == KEY_UP) {
			y--;
		}
		if (input == KEY_DOWN) {
			y++;
		}
		if (input == KEY_LEFT) {
			x--;
		}
		if (input == KEY_RIGHT) {
			x++;
		}

		if (y < 2) {
			y = MAP_YSIZE - 1;
		}
		if (y > MAP_YSIZE - 1) {
			y = 2;
		}
		if (x < 2) {
			x = MAP_XSIZE - 3;
		}
		if (x > MAP_XSIZE - 3) {
			x = 2;
		}

		if (input == '\n' || input == ' ' || input == KEY_ENTER) {
			if (last_obj != MAP_PLAYER) {
				map[y - 1][x] = last_obj;
			}
		}

		if (lock == 1) {
			if (input == KEY_RIGHT || input == KEY_LEFT || input == KEY_UP || input == KEY_DOWN) {
				if (last_obj != MAP_PLAYER) {
					map[y - 1][x] = last_obj;
				}
			}
		}

		if (input == KEY_DC || input == 0x7f) {
			map[y - 1][x] = MAP_EMPTY;
		}

		if (input == '0' || input == '|') {
			map[y - 1][x] = MAP_EMPTY;
			last_obj = MAP_EMPTY;
		}
		if (input == '1') {
			map[y - 1][x] = MAP_DIRT;
			last_obj = MAP_DIRT;
		}
		if (input == '2') {
			map[y - 1][x] = MAP_STONE;
			last_obj = MAP_STONE;
		}
		if (input == '3') {
			map[y - 1][x] = MAP_DIAMOND;
			last_obj = MAP_DIAMOND;
		}
		if (input == '4') {
			map[y - 1][x] = MAP_WALL;
			last_obj = MAP_WALL;
		}
		if (input == '5') {
			map[y - 1][x] = MAP_MONEY;
			last_obj = MAP_MONEY;
		}
		if (input == '6') {
			map[y - 1][x] = MAP_BOMBPK;
			last_obj = MAP_BOMBPK;
		}
		if (input == '7') {
			map[y - 1][x] = MAP_MONSTER;
			last_obj = MAP_MONSTER;
		}
		if (input == '9') {
			/* last_obj = MAP_PLAYER; */
			for (int yy = 0; yy < MAP_YSIZE; yy++) {
				for (int xx = 0; xx < MAP_XSIZE; xx++) {
					if (map[yy][xx] == MAP_PLAYER) {
						map[yy][xx] = MAP_EMPTY;
					}
				}
			}
			map[y - 1][x] = MAP_PLAYER;
			map[1][1] = MAP_WALL;
		}

		if (tolower(input) == 's') {
			beep();
			curs_set(0);
			if (save_map(file) == 1) {
				msgbox("ERROR: Unable to open file for writing!");
			} else {
				msgbox("Saved successfully!");
			}
			curs_set(1);
		}

		/* if(tolower(input) == 'x') {
		 *   beep();
		 *   if(save_map(file) != 1) {
		 *     curses_stop();
		 *     exit(0);
		 *   }
		 * }  */

		if (tolower(input) == 'q') {
			curs_set(0);

			for (;;) {
				int rval = tolower(msgbox("Are you sure you want to quit? (Yes/No)"));
				if (rval == 'y' || rval == '\n' || rval == ' ') {
					curses_stop();
					exit(0);
				} else if (rval == 'n') {
					curs_set(1);
					break;
				}
			}
		}


		if (tolower(input) == 'l') {
			if (lock == 1) {
				lock = 0;
			} else if (lock == 0) {
				lock = 1;
			}
		}

		editor_draw_status();
	}

	return EXIT_SUCCESS;
}

void editor_draw_status(void) {
	attrset(COLOR_PAIR(COLOR_GREEN));
	mvprintw(0, calc_center(strlen("CAVEZ of PHEAR " VERSION " EDITOR")), "CAVEZ of PHEAR " VERSION " EDITOR");

	attrset(COLOR_PAIR(COLOR_MAGENTA));
	mvprintw(24, 0, "0 EMPTY  1 #  2 O  3 *  4 :  5 $  6 @  7 M  9 Z     l LOCK ON/OFF s SAVE  q QUIT");
/* mvaddch(24, 13, CHR_DIRT);
 * mvaddch(24, 18, CHR_STONE);
 * mvaddch(24, 23, CHR_DIAMOND);
 * mvaddch(24, 28, CHR_WALL);
 * mvaddch(24, 33, CHR_MONEY);
 * mvaddch(24, 38, CHR_PLAYER); */

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_NORMAL);
	mvprintw(0, 60, "OBJECT:");

	switch (last_obj) {
	case MAP_EMPTY:
		mvaddch(0, 68, CHR_EMPTY);
		attrset(COLOR_PAIR(COLOR_WHITE) | A_NORMAL);
		mvprintw(24, 2, "EMPTY");
		mvaddch(24, 0, '0' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_DIRT:
		mvaddch(0, 68, CHR_DIRT);
		mvaddch(24, 11, CHR_DIRT);
		mvaddch(24, 9, '1' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_WALL:
		mvaddch(0, 68, CHR_WALL);
		mvaddch(24, 26, CHR_WALL);
		mvaddch(24, 24, '4' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_STONE:
		mvaddch(0, 68, CHR_STONE);
		mvaddch(24, 16, CHR_STONE);
		mvaddch(24, 14, '2' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_DIAMOND:
		mvaddch(0, 68, CHR_DIAMOND);
		mvaddch(24, 21, CHR_DIAMOND);
		mvaddch(24, 19, '3' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_MONEY:
		mvaddch(0, 68, CHR_MONEY);
		mvaddch(24, 31, CHR_MONEY);
		mvaddch(24, 29, '5' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_BOMBPK:
		mvaddch(0, 68, CHR_BOMBPK);
		mvaddch(24, 36, CHR_BOMBPK);
		mvaddch(24, 34, '6' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_MONSTER:
		mvaddch(0, 68, CHR_MONSTER);
		mvaddch(24, 41, CHR_MONSTER);
		mvaddch(24, 39, '7' | COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		break;

	case MAP_PLAYER:
		exit(0);
		mvaddch(24, 36, CHR_PLAYER);
		break;
	}


	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_NORMAL);
	mvprintw(0, 71, "LOCK:");

	if (lock == 0) {
		attrset(COLOR_PAIR(COLOR_WHITE) | A_NORMAL);
		mvprintw(0, 77, "OFF");
		mvprintw(24, 62, "OFF");
	}
	if (lock == 1) {
		attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
		mvprintw(0, 77, "ON ");
		mvprintw(24, 59, "ON");
	}

	attrset(COLOR_PAIR(COLOR_MAGENTA));
	mvprintw(0, 0, "*: %d $: %d SCORE: %d   ", count_map_objects(MAP_DIAMOND),
	         count_map_objects(MAP_MONEY), (count_map_objects(MAP_DIAMOND) * POINTS_DIAMOND) +
	         (count_map_objects(MAP_MONEY) * POINTS_MONEY));

	attrset(A_NORMAL);
}
