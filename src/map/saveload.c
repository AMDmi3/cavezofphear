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

#include <stdio.h>

void clear_map(void) {
	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			map[y][x] = MAP_EMPTY;
		}
	}
}

int load_map(const char* filename) {
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		return 0;
	}

	clear_map();

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			int c = fgetc(fp);
			if (c == EOF) {
				fclose(fp);
				return 0;
			}

			map[y][x] = c;
		}
	}

	fclose(fp);

	return 1;
}

int save_map(const char* filename) {
	FILE* fp = fopen(filename, "w");
	if (fp == NULL) {
		return 0;
	}

	for (int y = 0; y < MAP_YSIZE; y++) {
		for (int x = 0; x < MAP_XSIZE; x++) {
			fputc(map[y][x], fp);
		}
	}

	fclose(fp);

	return 1;
}
