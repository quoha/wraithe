/*************************************************************************
 * wraithe/src/bin/wraithe/translate/smartQuotes.c
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

/*************************************************************************
 */
char *TranslateSmartQuotes(char *buffer) {
	unsigned char *p  = (unsigned char *)buffer;
	while (*p) {
		if (p[0] == 0xE2 && p[1] == 0x80 && (p[2] == 0x98 || p[2] == 0x99 || p[2] == 0x9C || p[2] == 0x9D)) {
			break;
		}
		p++;
	}

	unsigned char *to = p;
	while (*p) {
		if (p[0] == 0xE2 && p[1] == 0x80 && (p[2] == 0x98 || p[2] == 0x99 || p[2] == 0x9C || p[2] == 0x9D)) {
			*(to++) = (p[2] == 0x9C || p[2] == 0x9D) ? '"' : '\'';
			p += 3;
		} else {
			*(to++) = *(p++);
		}
	}

	*to = 0;

	return buffer;
}
