/*************************************************************************
 * wraithe/src/bin/incfile/translate.c
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

#include <stdlib.h>
#include <string.h>


/*************************************************************************
 *  ( text | include )*
 */
int Translate(int id, const char *text) {
	const char *p = text;

	while (*p) {
		if Accept(p, "<!-- include ") {
		} else {
		}
		p++;
	}

	while (*p && *p != '<' && *p != '&' && *p != '>') {
		p++;
	}

	if (!*p) {
		return 1;
	}

	char *newText = strdup("Translate(text);");
	if (newText) {
			const char *update =
				"\tupdate articles\n"
				"\t   set text = ?\n"
				"\t where id   = ?\n";

			// make the update

			free(newText);
		}
	}

	return 1;
}
