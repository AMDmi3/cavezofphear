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

#include <stdio.h>
#include <stdlib.h>

const char* get_data_dir() {
	if (!chk_file(DATADIR "/data/", "spgraf")) {
		return DATADIR "/data/";
	}
	return NULL;
}

void chk_all(void) {
	const char* dir = get_data_dir();
	int errors = 0;

	if (dir == NULL) {
		fprintf(stderr, "Unable to find data, aborting.\n");
		exit(1);
	}

	if (chk_file(dir, "gover") == 1) {
		errors++;
	}
	if (chk_file(dir, "htext") == 1) {
		errors++;
	}
	if (chk_file(dir, "spgraf") == 1) {
		errors++;
	}
	if (chk_file(dir, "tdesc") == 1) {
		errors++;
	}
	if (chk_file(dir, "levels/01") == 1) {
		errors++;
	}

	if (errors > 0) {
		fprintf(stderr, "%d or more required data files are missing, aborting.\n", errors);
		exit(1);
	}
	//LOLDEBIAN
}

int chk_file(const char* dir, const char* filename) {
	char fname[1024];

	snprintf(fname, sizeof fname, "%s/%s", dir, filename);

	FILE* fp = fopen(fname, "r");
	if (fp == NULL) {
		return 1;
	}

	fclose(fp);
	return 0;
}
