/*************************************************************************
 * wraithe/src/bin/wraithe/html/new.c
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
#include "../local.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************************************************************************
 * (<h1 .*> | <h2 .*>) title (</h1> | </h2>) text?
 */
static void SplitHTag(const char *data, const char *endOfData, const char **tag, const char **title, const char **close, const char **text) {
	*tag = data;
	while (data < endOfData && *data != '>') {
		data++;
	}
	if (data < endOfData) {
		data++;
	}

	*title = data;
	while (data < endOfData && *data != '<') {
		data++;
	}

	*close = data;
	while (data < endOfData && *data != '>') {
		data++;
	}
	if (data < endOfData) {
		data++;
	}

	*text = data;
}

/*************************************************************************
 */
BODY *BODYNew(const char *tag, const char *text, const char *close) {
	int   idx;
	BODY *p = (BODY *)malloc(sizeof(*p));
	if (!p) {
		perror("newBODY");
		exit(2);
	}
	memset(p, 0, sizeof(*p));
	p->tag  = StrNDup(tag , text  - tag );
	p->text = StrNDup(text, close - text);

	return p;
}

/*************************************************************************
 * <h1> title </h1> text?
 */
H1 *H1New(const char *data, const char *endOfData) {
	int idx;
	H1  *p = (H1 *)malloc(sizeof(*p));
	if (!p) {
		perror("newH1");
		exit(2);
	}
	memset(p, 0, sizeof(*p));

	SplitHTag(data, endOfData, &(p->tag), &(p->title), &(p->close), &(p->text));

	p->tag   = StrNDup(p->tag  , p->title  - p->tag  );
	p->title = StrNDup(p->title, p->close  - p->title);
	p->close = StrNDup(p->close, p->text   - p->close);
	p->text  = StrNDup(p->text , endOfData - p->text );

	return p;
}

/*************************************************************************
 */
H2 *H2New(const char *data, const char *endOfData) {
	H2 *p = (H2 *)malloc(sizeof(*p));
	if (!p) {
		perror("newH2");
		exit(2);
	}
	memset(p, 0, sizeof(*p));

	SplitHTag(data, endOfData, &(p->tag), &(p->title), &(p->close), &(p->text));

	p->tag   = StrNDup(p->tag  , p->title  - p->tag  );
	p->title = StrNDup(p->title, p->close  - p->title);
	p->close = StrNDup(p->close, p->text   - p->close);
	p->text  = StrNDup(p->text , endOfData - p->text );

	return p;
}

/*************************************************************************
 */
HEAD *HEADNew(const char *data, const char *endOfData) {
	HEAD *p = (HEAD *)malloc(sizeof(*p));
	if (!p) {
		perror("newHEAD");
		exit(2);
	}
	memset(p, 0, sizeof(*p));
	p->data = StrNDup(data, endOfData - data);
	return p;
}

/*************************************************************************
 */
HTML *HTMLNew(void) {
	HTML *p = (HTML *)malloc(sizeof(*p));
	if (!p) {
		perror("newHTML");
		exit(2);
	}
	memset(p, 0, sizeof(*p));
	p->head = 0;
	p->body = 0;
	return p;
}
