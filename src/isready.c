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

#include "isready.h"

#include <sys/select.h>
#include <stddef.h>

int isready(int fd) {
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	struct timeval tv;
	tv.tv_sec = tv.tv_usec = 0;

	if (select(fd + 1, &fds, NULL, NULL, &tv) < 0) {
		return -1;
	}

	return FD_ISSET(fd, &fds) ? 1 : 0;
}
