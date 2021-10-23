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

#include "gplot.h"

#include "chk.h"

#include <stdio.h>
#include <ncurses.h>

int gplot(const char* filename, int x_offset, int y_offset, int ign_space) {
	char fname[1024];
	snprintf(fname, sizeof fname, "%s%s", get_data_dir(), filename);

	int x = x_offset;
	int y = y_offset;

	FILE* fp = fopen(fname, "r");
	if (fp == NULL) {
		return 1;
	}

	int c;
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n') {
			y++;
			x = x_offset;
		} else {
			if (c == ' ' && ign_space == 1) {
			} else {
				mvaddch(y, x, c);
			}

			x++;
		}
	}

	fclose(fp);

	return 0;
}
