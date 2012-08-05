/*************************************************************************
 * wraithe/src/bin/wraithe/token/dump.c
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

void TokenDump(Token *root) {
	Token *t = root;
	while (t) {
		switch (t->kind) {
		case tkText:
			printf("\ntoken:\ttkText %8d ===============================================\n%s\n", t->id, t->data);
			//fprintf(fp, "%s", t->data);
			break;
		case tkH1:
			printf("\ntoken:\ttkH1   %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkH2:
			printf("\ntoken:\ttkH2   %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkH3:
			printf("\ntoken:\ttkH3   %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkH4:
			printf("\ntoken:\ttkH4   %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkH5:
			printf("\ntoken:\ttkH5   %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkH6:
			printf("\ntoken:\ttkH6   %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkStart:
			printf("\ntoken:\ttkStart%8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkCode:
			printf("\ntoken:\ttkCode %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkEnd:
			printf("\ntoken:\ttkEnd  %8d ===============================================\n%s\n", t->id, t->data);
			break;
		case tkNode:
			printf("\ntoken:\ttkNode %8d vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n", t->id);
			TokenDump(t->node);
			printf("\n=====:\ttkNode %8d ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", t->id);
			break;
		}

		t = t->next;
	}
}
