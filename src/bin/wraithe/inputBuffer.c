/*************************************************************************
 * wraithe/src/bin/wraithe/inputBuffer.c
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
#include <sys/stat.h>


/*************************************************************************
 */
char *InputBuffer(const char *fileName) {
	struct stat statBuf;
	if (stat(fileName, &statBuf) != 0) {
		perror(fileName);
		exit(2);
	}

	FILE *fp = fopen(fileName, "r");
	if (!fp) {
		perror(fileName);
		exit(2);
	}

	char *data = (char *)malloc(sizeof(char) * (statBuf.st_size + 1));
	if (!data) {
		perror("InputBuffer_data");
		exit(2);
	}

	if (statBuf.st_size && fread(data, statBuf.st_size, 1, fp) != 1) {
		perror(fileName);
		exit(2);
	}

	fclose(fp);

	data[statBuf.st_size] = 0;

	return TranslatePHP(TranslateSmartQuotes(data));
}
