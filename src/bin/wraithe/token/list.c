/*************************************************************************
 * wraithe/src/bin/wraithe/token/list.c
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
#include <ctype.h>

/*************************************************************************
 */
typedef struct ioi ioi;
struct ioi {
	ioi  *prev;
	char *data;
	ioi  *next;
};


/*************************************************************************
 */
static ioi   *IOIList(char *buffer);
static ioi   *IOINew(char *data);
static char  *IOIString(ioi *i);


/*************************************************************************
 */
static ioi *IOIList(char *buffer) {
	const char *match = 0;
	ioi  *root  = 0;
	ioi  *tail  = 0;

	// ignore everything up to the first match
	match = 0;
	while (*buffer && !match) {
		match = MatchPattern("<?php", buffer);
		match = match ? match : MatchPattern("<h1", buffer);
		match = match ? match : MatchPattern("<h2", buffer);
		match = match ? match : MatchPattern("<h3", buffer);
		match = match ? match : MatchPattern("<h4", buffer);
		match = match ? match : MatchPattern("<h5", buffer);
		match = match ? match : MatchPattern("<h6", buffer);
		if (!match) {
			buffer++;
		}
	}
	if (!*buffer) {
		return 0;
	}

	while (*buffer) {
		if (!root) {
			root = tail = IOINew(buffer);
		} else {
			tail->next = IOINew(buffer);
			tail = tail->next;
		}

		// php is special since it could contain headers in it
		match = MatchPattern("<?php ", buffer);
		if (match) {
			match = 0;
			while (*buffer && !match) {
				match = MatchPattern(" ?>", buffer++);
			}
			if (!*buffer) {
				printf("\nerror:\tinput missing ?>\n\n");
				return 0;
			}
			buffer = (char *)match;
		} else {
			buffer++;
			match = 0;
			while (*buffer && !match) {
				match = MatchPattern("<?php", buffer);
				match = match ? match : MatchPattern("<h1", buffer);
				match = match ? match : MatchPattern("<h2", buffer);
				match = match ? match : MatchPattern("<h3", buffer);
				match = match ? match : MatchPattern("<h4", buffer);
				match = match ? match : MatchPattern("<h5", buffer);
				match = match ? match : MatchPattern("<h6", buffer);
				match = match ? match : MatchPattern("</body>", buffer);
				if (!match) {
					buffer++;
				}
			}
		}
	}

	ioi *i = root;
	while (i) {
		i->data = IOIString(i);
		i = i->next;
	}

	return root;
}


/*************************************************************************
 */
static ioi *IOINew(char *data) {
	ioi *i = (ioi *)malloc(sizeof(*i));
	if (!i) {
		perror("IOINew");
		exit(2);
	}
	i->prev = i->next = 0;
	i->data = data;
	return i;
}


/*************************************************************************
 */
static char *IOIString(ioi *i) {
	char *soString = i->data;
	char *eoString = i->data;
	if (i->next) {
		eoString = i->next->data;
	} else {
		while (*eoString) {
			eoString++;
		}
	}
	int len = eoString - soString;
	char *s = (char *)malloc(sizeof(char) * (len + 1));
	if (!s) {
		perror("IOIString");
		exit(2);
	}
	memcpy(s, soString, len);
	s[len] = 0;
	return s;
}


/*************************************************************************
 */
Token *TokenList(char *buffer, const char *separator) {
	Token *root = 0, *tail = 0, *t;

	ioi *items = IOIList(buffer);
	while (items) {
		printf("item ==========================================================\n");
		printf("%s\n", items->data);
		if (MatchPattern("</body>", items->data)) {
			break;
		}

		int kind = tkText;
		if (MatchPattern("<h1", items->data)) {
			kind = tkH1;
		} else if (MatchPattern("<h2", items->data)) {
			kind = tkH2;
		} else if (MatchPattern("<h3", items->data)) {
			kind = tkH3;
		} else if (MatchPattern("<h4", items->data)) {
			kind = tkH4;
		} else if (MatchPattern("<h5", items->data)) {
			kind = tkH5;
		} else if (MatchPattern("<h6", items->data)) {
			kind = tkH6;
		}

		if (!root) {
			root = tail = TokenNew(kind, items->data);
		} else {
			tail->next = TokenNew(kind, items->data);
			tail->next->prev = tail;
			tail = tail->next;
		}
		items = items->next;
	}

	Token *posts = TokenSeparator(root, "<h1");
	printf("\n\n\n\n");
	TokenDump(posts);
	printf("\n\n\n\n");
	t = posts;
	while (t) {
		t->node = TokenSeparator(t->node, "<h2");
		t = t->next;
	}
	printf("\n\n\n\n");
	TokenDump(posts);
	printf("\n\n\n\n");

	return posts;

	// break it up by separator
	posts = TokenNew(tkNode, 0);
	printf(" node:\tadding %s\n", root->data);
	posts->node  = root;
	tail         = posts;
	t            = root ? root->next : 0;
	while (t) {
		if (MatchPattern(separator, t->data)) {
			printf(" node:\tadding %s\n", t->data);
			tail->next = TokenNew(tkNode, 0);
			tail->next->prev = tail;
			tail = tail->next;
			tail->node = t;
			if (t->prev) {
				t->prev->next = 0;
			}
			t->prev = 0;
		}
		t = t->next;
	}

	return posts;
}
