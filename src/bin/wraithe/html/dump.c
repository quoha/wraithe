/*************************************************************************
 * wraithe/src/bin/wraithe/html/dump.c
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
void HTMLDump(HTML *html, FILE *fp) {
	fprintf(fp, "\n");
	fprintf(fp, " html:\t*************************************************************************\n");
	if (html) {
		if (html->head) {
			fprintf(fp, " html:\thead\n");
			fprintf(fp, "\t\t%s\n", html->head->data);
		}
		if (html->body) {
			fprintf(fp, " html:\t%s\n%s\n", html->body->tag, html->body->text);
			int idH1;
			for (idH1 = 0; idH1 < MAXH1 && html->body->h1[idH1]; idH1++) {
				H1 *h1 = html->body->h1[idH1];
				fprintf(fp, " html:\t    %s%s%s\n", h1->tag, h1->title, h1->close);
				int idH2;
				for (idH2 = 0; idH2 < MAXH2 && h1->h2[idH2]; idH2++) {
					H2 *h2 = h1->h2[idH2];
					fprintf(fp, " html:        %s%s%s\n", h2->tag, h2->title, h2->close);
				}
			}
		}
	}
	fprintf(fp, "\n");
}
