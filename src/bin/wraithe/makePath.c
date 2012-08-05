/*************************************************************************
 * wraithe/src/bin/wraithe/makePath.c
 *
 * Copyright (c) 2012 Michael D Henderson, all rights reserved
 *
 * This file is part of wraithe (http://github.com/quoha/wraithe).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************/
#include "local.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/*************************************************************************
 */
int MakePath(char *path) {
	char *sl = strchr(path, '/');
	while (sl) {
		*sl = 0;
		if (*path) {
			// does path exist?
			struct stat statbuf;
			if (stat(path, &statbuf) != 0) {
				// then create it
				printf(" info:\tmkdir %s\n", path);
				if (mkdir(path, 0755) != 0) {
					perror(path);
					exit(2);
				}
			}
		}
		*sl = '/';
		sl = strchr(sl + 1, '/');
	}
	return 0;
}
