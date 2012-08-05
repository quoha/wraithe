/*************************************************************************
 * wraithe/src/bin/wraithe/translate/php.c
 *
 * Copyright (c) 2012 Michael D Henderson, all rights reserved
 *
 * This file is part of wraithe
 *************************************************************************/
#include "../local.h"

/*************************************************************************
 */
char *TranslatePHP(char *buffer) {
	char *from = buffer;
	while (*from && !MatchPattern("<!-- php ", from)) {
		from++;
	}

	char *to = from;

	while (*from) {
		const char *match = MatchPattern("<!-- php ", from);
		if (!match) {
			*(to++) = *(from++);
		} else {
			*(to++) = '<';
			*(to++) = '?';
			*(to++) = 'p';
			*(to++) = 'h';
			*(to++) = 'p';
			*(to++) = ' ';
			from = (char *)match;
			while (*from) {
				match = MatchPattern(" php -->", from);
				if (!match) {
					*(to++) = *(from++);
				} else {
					*(to++) = ' ';
					*(to++) = '?';
					*(to++) = '>';
					from = (char *)match;
					break;
				}
			}
		}
	}

	*to = 0;

	return buffer;
}
