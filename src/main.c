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


static struct option longopts[] = {
	{ "edit", no_argument, NULL, 'e' },
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ NULL, 0, NULL, 0 },
};


void usage(const char* progname, FILE* fd) {
	(void)progname;
	fprintf(
		fd,
		/* 70 chars                                                         */
		"Usage: %1$s [--version] [--help] [MAPFILE]\n"
		"   or: %1$s --edit MAPFILE\n"
		"\n"
		"You are trapped in the CAVEZ of PHEAR, your mission is to escape\n"
		"through all the caves and make it out alive. To escape through a\n"
		"cave you will have to find all the diamonds located in it. Once\n"
		"you've found all the diamonds, their powers combined will help you\n"
		"get to the next cave, one step closer to freedom.\n"
		"\n"
		"Options:\n"
		"  -h, --help     display this help\n"
		"  -v, --version  display program version\n"
		"  -e, --edit     run level editor on the specified file\n"
		"\n"
		/* 70 chars                                                         */
		, progname
	);
}


int main(int argc, char** argv) {
	int edit_mode = 0;
	int c;

	while ((c = getopt_long(argc, argv, "evh", longopts, NULL)) != -1) {
		switch (c) {
		case 'e':
			edit_mode = 1;
			break;
		case 'v':
			printf("%s\n", VERSION);
			return 0;
		case 'h':
			usage(argv[0], stdout);
			return 0;
		default:
			usage(argv[0], stderr);
			return 1;
		}
	}

	if (edit_mode) {
		if (optind == argc) {
			usage(argv[0], stderr);
			return 1;
		}
		return editor_main(argv[optind]);
	} else {
		return game_main(argv[optind]);
	}
}
