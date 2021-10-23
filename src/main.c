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
#include "game.h"

#include <stdio.h>
#include <getopt.h>

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

	return game_main(argv[optind]);
}
