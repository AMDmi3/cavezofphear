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

#include "game.h"

#include "chk.h"
#include "common.h"
#include "editor.h"
#include "frame.h"
#include "isready.h"
#include "map/map.h"
#include "misc.h"
#include "splash.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

#define UPDATE_DELAY 2000000

#define EX_C CHR_DIAMOND
#define EX_DELAY 50000

char item_behind_player;
const char* custom_map = NULL;
int p_x;
int p_y;
int diamonds_left;
int lives;
long int score;
long int score_last_extralife;
int bombs;
int level;
int need_refresh;
int option_sound;
int first_bomb;

void make_ready(void);
int mainloop(void);
int update_map(void);
void create_map(void);
void player_died(void);
void explode(int x, int y, int len, int chr);
void explode_put(int y, int x, int chr);
void got_diamond();
void got_money();
void got_bombs();
void got_extralife();
void level_done(int x, int y);
void draw_status(void);
void _beep(void);
void try_explode_bombs(void);
int do_the_monster_dance(void);

int game_main(const char* custom_map_path) {
	chk_all();

	curses_start();

	if (COLS < 80 || LINES < 25) {
		bail("error: your terminal size must be at least 80x25");
	}

	signal(SIGINT, sigint_handler);
	signal(SIGWINCH, sigwinch_handler);

	srand(time(0));

	make_ready();

	option_sound = 1;

	custom_map = custom_map_path;
	if (!custom_map) {
		splash();
	}

	mainloop();

	return EXIT_SUCCESS;
}

void make_ready(void) {
	level = 1;
	lives = 3;
	score = 0;
	score_last_extralife = 0;
	bombs = 0;
	first_bomb = 1;
	need_refresh = 0;
}

int is_stonish(char type) {
	// checks item for stone-like behavior, e.g. an item
	// which may fall down and roll down stonish slopes
	return type == MAP_STONE ||
		   type == MAP_DIAMOND ||
		   type == MAP_MONEY ||
		   type == MAP_BOMB ||
		   type == MAP_BOMBPK;
}

void move_player(int dx, int dy) {
	const int new_p_x = p_x + dx;
	const int new_p_y = p_y + dy;

	switch (map[new_p_y][new_p_x]) {
	case MAP_MONSTER:
		player_died();
		break;
	case MAP_DIAMOND:
		got_diamond();
		break;
	case MAP_MONEY:
		got_money();
		break;
	case MAP_BOMBPK:
		got_bombs();
		break;
	case MAP_STONE:
	case MAP_BOMB:
		if (dx == -1 && map[new_p_y][new_p_x - 1] == MAP_EMPTY) {
			// push left
			map[new_p_y][new_p_x - 1] = map[new_p_y][new_p_x];
		} else if (dx == +1 && map[new_p_y][new_p_x + 1] == MAP_EMPTY) {
			// push right
			map[new_p_y][new_p_x + 1] = map[new_p_y][new_p_x];
		} else {
			return;  // blocked
		}
		break;
	case MAP_WALL:
		return;  // blocked
	case MAP_EMPTY:  // move freely
	case MAP_DIRT:   // move and
		break;
	case MAP_PLAYER:
		// XXX: assert(false);
		break;
	}

	// update old location
	map[p_y][p_x] = item_behind_player;
	item_behind_player = MAP_EMPTY;

	// update player pos and new location
	p_x = new_p_x;
	p_y = new_p_y;
	map[p_y][p_x] = MAP_PLAYER;
}

void try_place_bomb() {
	if (bombs > 0 && item_behind_player != MAP_BOMB) {
		bombs--;

		if (map[p_y + 1][p_x] == MAP_EMPTY) {
			// bomb falls down
			map[p_y + 1][p_x] = MAP_BOMB;
		} else if (is_stonish(map[p_y + 1][p_x]) && map[p_y + 1][p_x + 1] == MAP_EMPTY && map[p_y][p_x + 1] == MAP_EMPTY) {
			// bomb rolls right
			map[p_y + 1][p_x + 1] = MAP_BOMB;
		} else if (is_stonish(map[p_y + 1][p_x]) && map[p_y + 1][p_x - 1] == MAP_EMPTY && map[p_y][p_x - 1] == MAP_EMPTY) {
			// bomb rolls left
			map[p_y + 1][p_x - 1] = MAP_BOMB;
		} else {
			// bomb is placed at player location
			item_behind_player = MAP_BOMB;
		}
		need_refresh = 1;
	}
}

void commit_suicide() {
	map[p_y][p_x] = MAP_DIAMOND;
	player_died();
}

void handle_input(int key) {
	switch (tolower(key)) {
	case 'b':
		try_place_bomb();
		break;

	case 't':
		try_explode_bombs();
		break;

	case 'q':
		for (;;) {
			int rval = tolower(msgbox("Are you sure you want to quit? (Yes/No)"));
			if (rval == 'y' || rval == '\n' || rval == ' ') {
				curses_stop();
				exit(0);
			} else if (rval == 'n') {
				break;
			}
		}
		break;

	case 'k':
		commit_suicide();
		break;

	case 's':
		if (option_sound == 0) {
			option_sound = 1;
			_beep();
		} else {
			option_sound = 0;
		}
		break;

	case 'w':
		for (int i = 0; i < 3; i++) {
			attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
			mvaddch(p_y - 1, p_x, '+');
			mvaddch(p_y + 3, p_x, '+');
			mvaddch(p_y + 1, p_x + 2, '+');
			mvaddch(p_y + 1, p_x - 2, '+');
			attrset(A_NORMAL);
			refresh();
			mysleep(90000);
			draw_map();
			refresh();
			mysleep(50000);
		}
		erase();
		draw_map();
		draw_status();
		refresh();
		break;

	case KEY_UP:
	case '8':
		move_player(0, -1);
		break;

	case KEY_DOWN:
	case '2':
		move_player(0, 1);
		break;

	case KEY_LEFT:
	case '4':
		move_player(-1, 0);
		break;

	case KEY_RIGHT:
	case '6':
		move_player(1, 0);
		break;
	}
}

int mainloop(void) {
	erase();

	create_map();

	p_x = 1;
	p_y = 1;

	int num_players = 0;
	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			if (map[y][x] == MAP_PLAYER) {
				if (!num_players++) {  // only consider first player location
					p_x = x;
					p_y = y;
				}
				map[p_y][p_x] = MAP_EMPTY;
			}
		}
	}

	map[p_y][p_x] = MAP_PLAYER;
	item_behind_player = MAP_EMPTY;

	diamonds_left = count_map_objects(MAP_DIAMOND);

	while (update_map() > 0) {
	}

	draw_map();
	draw_status();
	refresh();

	flushinp();

	long last_tick_time = 0;
	int tick = 100;
	int ticks_per_second = 100;
	int mcount = 0;
	int mloop_delay = 10000;
	while (1) {
		tick++;
		if (time(NULL) > last_tick_time) {
			last_tick_time = time(NULL);
			ticks_per_second = tick;
			if (ticks_per_second < 50) {
				mloop_delay -= 1000;
			}
			if (ticks_per_second > 50) {
				mloop_delay += 1000;
			}
			tick = 0;
		}

		if (need_refresh == 1) {
			draw_map();
			draw_status();
			refresh();
			need_refresh = 0;
		}

		if (diamonds_left <= 0) {
			level_done(p_x, p_y);
		}

		mcount++;
		if (mcount == 9) {
			mcount = 0;
			do_the_monster_dance();
			need_refresh = 1;
			update_map();
		}

		if (isready(0)) {
			int input = getch();
			flushinp();

			handle_input(input);

			int update_delay = UPDATE_DELAY;

			for (;;) { // XXX: this is lame, fix it using ticks?
				draw_map();
				draw_status();
				refresh();

				for (int i = 0; i < update_delay; i++) {
					continue;
				}

				if (update_map() == 0) {
					break;
				}

				if (update_delay > 0) {
					update_delay = update_delay - (UPDATE_DELAY / 50);
				}

				if (isready(0)) {
					break;
				}
			}
		}

		mysleep(mloop_delay);
	}

	curses_stop();
	return EXIT_SUCCESS;
}

int update_map(void) {
	int changes = 0;

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			// falling things crush monsters
			if (is_stonish(map[y][x]) && map[y + 1][x] == MAP_MONSTER) {
				map[y + 1][x] = MAP_EMPTY;
				_beep();
				return 1;
			}

			// things fall down
			if (is_stonish(map[y][x]) && map[y + 1][x] == MAP_EMPTY) {
				map[y + 1][x] = map[y][x];
				map[y][x] = MAP_EMPTY;

				if (is_stonish(map[y + 1][x]) && map[y + 2][x] == MAP_PLAYER) {
					player_died();
				}

				return 1;
			}

			if (is_stonish(map[y][x]) && is_stonish(map[y + 1][x]) && map[y + 2][x] != MAP_EMPTY) {
				// things roll left
				if (map[y][x - 1] == MAP_EMPTY && map[y + 1][x - 1] == MAP_EMPTY) {
					map[y + 1][x - 1] = map[y][x];
					map[y][x] = MAP_EMPTY;

					if (is_stonish(map[y + 1][x - 1]) && map[y + 2][x - 1] == MAP_PLAYER) {
						player_died();
					}

					return 1;
				}

				// things roll right
				if (map[y][x + 1] == MAP_EMPTY && map[y + 1][x + 1] == MAP_EMPTY) {
					map[y + 1][x + 1] = map[y][x];
					map[y][x] = MAP_EMPTY;

					if (is_stonish(map[y + 1][x + 1]) && map[y + 2][x + 1] == MAP_PLAYER) {
						player_died();
					}

					return 1;
				}
			}
		}
	}

	return changes;
}

void create_map(void) {
	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			map[y][x] = MAP_EMPTY;
		}
	}

	char mstr[1024];
	if (custom_map) {
		snprintf(mstr, sizeof mstr, "%s", custom_map);
	} else {
		snprintf(mstr, sizeof mstr, "%s/levels/%02d", get_data_dir(), level);
	}

	if (!load_map(mstr)) {
		bail("error: load_map failed");
	}

	while (update_map() != 0) {
	}
}

void player_died(void) {
	lives--;
	bombs = 0;

	update_map();
	draw_map();
	draw_status();
	refresh();

	_beep();
	mysleep(90000);
	explode(p_x, p_y, 4, EX_C);

	while (update_map() > 0) {
		_beep();
		draw_map();
		refresh();
	}

	_beep();

	sleep(1);

	if (lives <= 0) {
		gameover();
		sleep(2);

		for (;;) {
			int rval = tolower(msgbox("Game over! Play again? (Y/N)"));

			if (rval == 'y' || rval == '\n' || rval == ' ') {
				make_ready();
				mainloop();
			} else if (rval == 'n') {
				curses_stop();
				exit(0);
			}
		}
	} else {
		sleep(1);
		mainloop();
	}
}

void explode(int x, int y, int len, int chr) {
	explode_put(y, x, chr);
	map[y][x] = MAP_EMPTY;

	refresh();

	for (int offset = 0; offset < len; offset++) {
		explode_put(y + offset, x, chr);
		explode_put(y - offset, x, chr);
		explode_put(y, x + offset * 2, chr);
		explode_put(y, x - offset * 2, chr);

		explode_put(y + offset, x + offset, chr);
		explode_put(y - offset, x + offset, chr);
		explode_put(y - offset, x - offset, chr);
		explode_put(y + offset, x - offset, chr);

		refresh();
		mysleep(EX_DELAY);
	}
}

void explode_put(int y, int x, int chr) {
	if ((x > 1 && MAP_XSIZE - 2 > x) && (y > 1 && MAP_YSIZE - 1> y)) {
		mvaddch(y, x, chr);
		map[y][x] = MAP_DIAMOND;
	}
}

void got_diamond() {
	_beep();
	diamonds_left--;
	score += POINTS_DIAMOND;
	if (score >= score_last_extralife + 1000) {
		got_extralife();
	}
}

void got_money() {
	_beep();
	score += POINTS_MONEY;
	if (score >= score_last_extralife + 1000) {
		got_extralife();
	}
}

void got_bombs() {
	//int rval;
	_beep();
	bombs += 3;
	if (bombs > 99) {
		bombs = 99;
	}
	if (first_bomb == 1) {
		first_bomb = 0;
		//for (;;) {
		//  rval = msgbox("Got the bombs! Press 'b' to place bombs, 't' to detonate! Press ENTER");
		//  refresh();
		//  if (rval == '\n')
		//    break;
		//}

		need_refresh = 1;
	}
}

void got_extralife() {
	if (lives < 99) {
		for (int i = 0; i < 6; i++) {
			_beep();
			mysleep(1);
		}

		lives++;
		score_last_extralife = score;
		draw_status();
	}
}

void level_done(int x, int y) {
	// int i;
	mysleep(100000);
	mvaddch(y + 1, x, 'z');
	refresh();
	mysleep(200000);
	mvaddch(y + 1, x, '.');
	refresh();
	mysleep(200000);

	map[y][x] = MAP_EMPTY;

	/*for(i = 6; i < 12; i += 2) {
	 * explode(MAP_XSIZE / 2, MAP_YSIZE / 2, i, EX_C);
	 * while(update_map() > 0) {
	 *   _beep();
	 *   draw_map();
	 *   refresh();
	 * }
	 *}
	 */

	fade_dissolv();
	mysleep(30000);

	level++;
	mainloop();
}

void draw_status(void) {
	attrset(COLOR_PAIR(COLOR_GREEN));
	mvprintw(0, calc_center(strlen("CAVEZ of PHEAR (" VERSION ")")), "CAVEZ of PHEAR (" VERSION ")");

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_BOLD);
	mvprintw(0, 0, "DIAMONDS LEFT:");
	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	mvprintw(0, 15, "%02d", diamonds_left);

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_BOLD);
	mvprintw(0, 71, "LIVES:");
	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	mvprintw(0, 78, "%02d", lives);

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_BOLD);
	mvprintw(24, 0, "SCORE:");
	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	mvprintw(24, 7, "%d", score);

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_BOLD);
	mvprintw(24, 36, "BOMBS:");
	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	mvprintw(24, 43, "%02d", bombs);

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_BOLD);
	mvprintw(24, 71, "LEVEL:");
	attrset(COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	mvprintw(24, 78, "%02d", level);

	attrset(A_NORMAL);
}

void _beep(void) {
	if (option_sound == 1) {
		beep();
	}
}

void try_explode_bombs(void) {
	int playerdied = 0;

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			if (map[y][x] == MAP_BOMB) {
				need_refresh = 1;
				_beep();
				for (int by = y - 1; by < y + 2; by++) {
					for (int bx = x - 1; bx < x + 2; bx++) {
						if (map[by][bx] == MAP_PLAYER) {
							playerdied = 1;
						}

						if (map[by][bx] != MAP_WALL && map[by][bx] != MAP_DIAMOND) {
							map[by][bx] = MAP_EMPTY;
							mvaddch(by + 1, bx, '+');
							refresh();
						}
					}
				}
			}
		}
	}

	mysleep(20000);

	if (playerdied == 1) {
		player_died();
	}

	return;
}

int do_the_monster_dance(void) {
	const int d = rand() % 3;

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			int moved = 0;
			if (map[y][x] == MAP_MONSTER) {
				if (d == 0 || d == 1) {
					if (p_y > y && (map[y + 1][x] == MAP_EMPTY || map[y + 1][x] == MAP_PLAYER)) {
						map[y][x] = MAP_EMPTY;
						map[y + 1][x] = MAP_MONSTER;
						y += 2;
						moved++;
					} else if (p_y < y && (map[y - 1][x] == MAP_EMPTY || map[y - 1][x] == MAP_PLAYER)) {
						map[y][x] = MAP_EMPTY;
						map[y - 1][x] = MAP_MONSTER;
						y += 2;
						moved++;
					}
				}
//        else {
				if (moved == 0) {
					if (p_x < x && (map[y][x - 1] == MAP_EMPTY || map[y][x - 1] == MAP_PLAYER)) {
						map[y][x] = MAP_EMPTY;
						map[y][x - 1] = MAP_MONSTER;
						x += 2;
						moved++;
					} else if (p_x > x && (map[y][x + 1] == MAP_EMPTY || map[y][x + 1] == MAP_PLAYER)) {
						map[y][x] = MAP_EMPTY;
						map[y][x + 1] = MAP_MONSTER;
						x += 2;
						moved++;
					}
				}

				if (moved == 0) {
					switch (rand() % 4) {
					case 0:
						if (map[y][x - 1] == MAP_EMPTY || map[y][x - 1] == MAP_PLAYER) {
							map[y][x] = MAP_EMPTY;
							map[y][x - 1] = MAP_MONSTER;
						}
						break;

					case 1:
						if (map[y][x + 1] == MAP_EMPTY || map[y][x + 1] == MAP_PLAYER) {
							map[y][x] = MAP_EMPTY;
							map[y][x + 1] = MAP_MONSTER;
						}
						break;
					case 2:
						if (map[y - 1][x] == MAP_EMPTY || map[y - 1][x] == MAP_PLAYER) {
							map[y][x] = MAP_EMPTY;
							map[y - 1][x] = MAP_MONSTER;
						}
						break;
					case 3:
						if (map[y + 1][x] == MAP_EMPTY || map[y + 1][x] == MAP_PLAYER) {
							map[y][x] = MAP_EMPTY;
							map[y + 1][x] = MAP_MONSTER;
						}
						break;
					default:
						exit(1);
					}
				}

				if (map[p_y][p_x] == MAP_MONSTER) {
					player_died();
				}
			}
		}
	}

	return 0;
}
