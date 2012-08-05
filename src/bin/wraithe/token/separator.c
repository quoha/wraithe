/*************************************************************************
 * wraithe/src/bin/wraithe/token/separator.c
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

/*************************************************************************
 */
Token *TokenSeparator(Token *list, const char *separator) {
	Token *root = 0, *tail = 0, *t;

	if (list) {
		// break it up by separator
		root = tail = TokenNew(tkNode, 0);
		root->node  = list;
		printf(" node:\tadding %s\n", root->node->data);

		t = root->node ? root->node->next : 0;
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
	}

	return root;
}
