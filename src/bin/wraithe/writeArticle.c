/*************************************************************************
 * wraithe/src/bin/wraithe/writeArticle.c
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*************************************************************************
 */
static char *TitleToPath(const char *path, const char *h1) {
	static char buffer[1024];

	if (strlen(path) + strlen(h1) + 8 > 1023) {
		printf("\nerror:\tmaximum path length exceeded\n");
		printf("\t%-18s == '%s'\n", "path", path);
		printf("\t%-18s == '%s'\n", "h1 title", h1);
		exit(2);
	}

	char *p = buffer;
	while (*path) {
		*(p++) = *(path++);
	}

	while (*h1) {
		if (isalnum(*h1)) {
			*(p++) = tolower(*(h1++));
		} else {
			*(p++) = '-';
			h1++;
		}
	}

	*(p++) = '.';
	*(p++) = 'p';
	*(p++) = 'h';
	*(p++) = 'p';
	*(p++) = 0;

	return buffer;
}


/*************************************************************************
 */
int WriteArticle(const char *path, const char *preLib, const char *preText, H1 *h1, const char *postLib) {
	char *file = TitleToPath(path, h1->title);
	printf("write:\t%s\n", file);

	MakePath(file);

	FILE *fp = fopen(file, "w");
	if (!fp) {
		perror(file);
		exit(2);
	}

	if (preLib && *preLib) {
		fprintf(fp, "%s", preLib);
	}
	fprintf(fp, "%s", preText);
	fprintf(fp, "%s%s%s\n%s\n", h1->tag, h1->title, h1->close, h1->text);
	if (postLib && *postLib) {
		fprintf(fp, "%s", postLib);
	}

	fclose(fp);

	return 0;
}
