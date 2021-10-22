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

#include "chk.h"
#include "common.h"
#include "editor.h"
#include "frame.h"
#include "isready.h"
#include "loadmap.h"
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

char map[MAP_YSIZE][MAP_XSIZE];
char item_behind_player;
char current_map[1024];
int p_x;
int p_y;
int diamonds_left;
int lives;
long int score;
long int score_last_extralife;
int bombs;
int level;
int custom_map;
int need_refresh;
int option_sound;
int first_bomb;

void make_ready(void);
int mainloop(void);
void draw_map(void);
int update_map(void);
void create_map(char* mapname);
void player_died(void);
void explode(int x, int y, int len, int chr);
void explode_put(int y, int x, int chr);
int count_diamonds();
int count_monsters();
void got_diamond();
void got_money();
void got_bombs();
void got_extralife();
void level_done(int x, int y);
void draw_status(void);
void _beep(void);
void explode_bombs(void);
int do_the_monster_dance(void);

int main(int argc, char** argv) {
	int c;

	while ((c = getopt(argc, argv, "e:vh")) != -1) {
		switch (c) {
		case 'e':
			if (!optarg) {
				fprintf(stderr, "usage: phear -e <file>\n");
				return 1;
			}
			return editor_main(optarg);
			break;
		case 'v':
			puts(VERSION);
			return 0;
		case 'h':
			puts("phear [-e] [-v] [-h] [<file>]\n");
			return 0;
		default:
			return 1;
		}
	}


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

	if (argv[optind]) {
		snprintf(current_map, sizeof current_map - 1, "%s", argv[optind]);
		custom_map = 1;
	} else {
		current_map[0] = 0x00;
		custom_map = 0;
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

int mainloop(void) {
	erase();

	create_map(current_map);

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

	diamonds_left = count_diamonds();

	while (update_map() > 0) {
	}

	draw_map();
	draw_status();
	refresh();

	flushinp();

	int x_direction = 0;
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

			if (tolower(input) == 'b') {
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

			if (tolower(input) == 't') {
				explode_bombs();
				need_refresh = 1;
			}

			if (tolower(input) == 'q') {
				for (;;) {
					int rval = tolower(msgbox("Are you sure you want to quit? (Yes/No)"));
					if (rval == 'y' || rval == '\n' || rval == ' ') {
						curses_stop();
						exit(0);
					} else if (rval == 'n') {
						break;
					}
				}
			}

			if (tolower(input) == 'k') {
				map[p_y][p_x] = MAP_DIAMOND;
				player_died();
			}

			if (tolower(input) == 's') {
				if (option_sound == 0) {
					option_sound = 1;
					_beep();
				} else {
					option_sound = 0;
				}
			}

			if (tolower(input) == 'w') { /* stupid feature to make lh stfu */
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
			}

			// move player
			map[p_y][p_x] = item_behind_player;
			int player_moved = 1;
			int old_p_y = p_y;
			int old_p_x = p_x;

			if (input == KEY_UP || input == '8') {
				p_y--;
				x_direction = +0;
			}
			if (input == KEY_DOWN || input == '2') {
				p_y++;
				x_direction = +0;
			}
			if (input == KEY_LEFT || input == '4') {
				p_x--;
				x_direction = -1;
			}
			if (input == KEY_RIGHT || input == '6') {
				p_x++;
				x_direction = +1;
			}

			if (map[p_y][p_x] == MAP_WALL) {
				p_y = old_p_y;
				p_x = old_p_x;
			}

			if (map[p_y][p_x] == MAP_MONSTER) {
				player_died();
			}

			if (is_stonish(map[p_y][p_x]) && x_direction == -1 && map[p_y][p_x - 1] == MAP_EMPTY) {
				// try to push left
				if (map[p_y][p_x] == MAP_DIAMOND) {
					got_diamond();
				} else if (map[p_y][p_x] == MAP_MONEY) {
					got_money();
				} else if (map[p_y][p_x] == MAP_BOMBPK) {
					got_bombs();
				} else { // STONE or BOMB
					map[p_y][p_x - 1] = map[p_y][p_x];
				}
			} else if (is_stonish(map[p_y][p_x]) && x_direction == +1 && map[p_y][p_x + 1] == MAP_EMPTY) {
				// try to push right
				if (map[p_y][p_x] == MAP_DIAMOND) {
					got_diamond();
				} else if (map[p_y][p_x] == MAP_MONEY) {
					got_money();
				} else if (map[p_y][p_x] == MAP_BOMBPK) {
					got_bombs();
				} else {
					map[p_y][p_x + 1] = map[p_y][p_x];
				}
			} else if (map[p_y][p_x] == MAP_STONE || map[p_y][p_x] == MAP_BOMB) {
				// blocked
				player_moved = 0;
				p_y = old_p_y;
				p_x = old_p_x;
			} else if (map[p_y][p_x] == MAP_DIAMOND) {
				got_diamond();
			} else if (map[p_y][p_x] == MAP_MONEY) {
				got_money();
			} else if (map[p_y][p_x] == MAP_BOMBPK) {
				got_bombs();
			}

			if (player_moved) {
				item_behind_player = MAP_EMPTY;
			}
			map[p_y][p_x] = MAP_PLAYER;

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

void create_map(char* mapname) {
	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			map[y][x] = MAP_EMPTY;
		}
	}

	char mstr[64];
	if (mapname[0] == 0x00) {
		snprintf(mstr, sizeof mstr - 1, "%s/levels/%02d", get_data_dir(), level);
	} else {
		snprintf(mstr, sizeof mstr - 1, "%s", mapname);
	}

	if (load_map(mstr, map) == 1) {
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
				if (custom_map == 0) {
					current_map[0] = 0x00;
				}
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

int count_diamonds() {
	int num_diamonds = 0;

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			if (map[y][x] == MAP_DIAMOND) {
				num_diamonds++;
			}
		}
	}

	return num_diamonds;
}

int count_monsters() {
	int num_monsters = 0;

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			if (map[y][x] == MAP_MONSTER) {
				num_monsters++;
			}
		}
	}

	return num_monsters;
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

void explode_bombs(void) {
	int playerdied = 0;

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			if (map[y][x] == MAP_BOMB) {
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
