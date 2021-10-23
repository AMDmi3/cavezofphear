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

#ifndef MAP_MAP_H
#define MAP_MAP_H

#define MAP_XSIZE 80
#define MAP_YSIZE 23

#define MAP_EMPTY   0
#define MAP_DIRT    1
#define MAP_STONE   2
#define MAP_DIAMOND 3
#define MAP_WALL    4
#define MAP_MONEY   5
#define MAP_PLAYER  9
#define MAP_BOMB    6
#define MAP_BOMBPK  7
#define MAP_MONSTER 8

#define CHR_EMPTY   ' '
#define CHR_DIRT    '#' | COLOR_PAIR(COLOR_RED)    | A_NORMAL
#define CHR_STONE   'O' | COLOR_PAIR(COLOR_WHITE)  | A_NORMAL
#define CHR_WALL    ':' | COLOR_PAIR(COLOR_CYAN)   | A_NORMAL
#define CHR_DIAMOND '*' | COLOR_PAIR(COLOR_YELLOW) | A_BOLD
#define CHR_MONEY   '$' | COLOR_PAIR(COLOR_GREEN)  | A_NORMAL
#define CHR_PLAYER  'Z' | COLOR_PAIR(COLOR_WHITE)  | A_BOLD
#define CHR_BOMB    '@' | COLOR_PAIR(COLOR_BLUE)   | A_BOLD
#define CHR_BOMBPK  '%' | COLOR_PAIR(COLOR_BLUE)   | A_BOLD
#define CHR_MONSTER 'M' | COLOR_PAIR(COLOR_MAGENTA)| A_BOLD

extern char map[MAP_YSIZE][MAP_XSIZE];

void draw_map(void);

void clear_map(void);
int load_map(const char* path);
int save_map(const char* path);

int count_map_objects(int object_type);

#endif
