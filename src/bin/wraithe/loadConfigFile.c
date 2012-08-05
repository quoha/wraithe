/*************************************************************************
 * wraithe/src/bin/wraithe/loadConfigFile.c
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
#include <ctype.h>

int LoadConfigFile(const char *configFile) {
	configFile = FixPath(configFile);

	// load the file into memory
	char *text = InputBuffer(configFile);
	if (!text) {
		free((void *)configFile);
		return 0;
	}

	while (*text) {
		// skip whitespace
		if (*text && isspace(*text)) {
			*(text++) = 0;
			continue;
		}

		// skip comments
		if (text[0] == '-' && text[1] == '-') {
			while (*text && *text != '\n') {
				*(text++) = 0;
			}
			continue;
		}

		char *name = text;
		while (*text && !isspace(*text)) {
			text++;
		}
		while (*text && isspace(*text)) {
			*(text++) = 0;
		}

		char *value     = text;
		char *lastSpace = 0;
		while (*text && *text != '\n') {
			if (!isspace(*text)) {
				lastSpace = 0;
			} else if (!lastSpace) {
				lastSpace = text;
			}
			text++;
		}
		if (lastSpace) {
			*lastSpace = 0;
		} else if (*text) {
			*(text++) = 0;
		}

		printf("  env:\t%-18s == '%s'\n", name, value);

		if (*value) {
			setenv(name, value, 1);
		} else {
			unsetenv(name);
		}
	}

	free((void *)configFile);
	return 1;
}
