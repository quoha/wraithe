/*************************************************************************
 * wraithe/src/bin/wraithe/local.h
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
#ifndef wraithe_src_bin_wraithe_local_H
#define wraithe_src_bin_wraithe_local_H

#include <stdio.h>
#include <sqlite3.h>

#define MAXH1      1024
#define MAXH2      1024

typedef struct HTML HTML;
typedef struct HEAD HEAD;
typedef struct BODY BODY;
typedef struct H1   H1;
typedef struct H2   H2;

struct HTML {
	HEAD *head;
	BODY *body;
};

struct HEAD {
	char *data;
	char *endOfData;
};

struct BODY {
	char *tag;
	char *text;
	char *close;
	H1   *h1[MAXH1 + 1];
};

struct H1 {
	const char *tag;
	const char *title;
	const char *close;
	const char *text;
	H2   *h2[MAXH2 + 1];
};

struct H2 {
	const char *tag;
	const char *title;
	const char *close;
	const char *text;
};

BODY       *BODYNew(const char *tag, const char *text, const char *close);
const char *CRC(const char *text);
H1         *H1New(const char *data, const char *endOfData);
H2         *H2New(const char *data, const char *endOfData);
HEAD       *HEADNew(const char *data, const char *endOfData);
void        HTMLDump(HTML *html, FILE *fp);
HTML       *HTMLNew(void);
HTML       *HTMLTree(char *buffer);
char       *InputBuffer(const char *fileName);
const char *MatchPattern(const char *pattern, const char *src);
int         OptIs(const char *cmdLineParameter, const char *option);
int         SetOpt(char *s, char **opt, char **val);
char       *StrNDup(const char *src, int len);
char       *TranslatePHP(char *buffer);
char       *TranslateSmartQuotes(char *buffer);
int         PostArticle(sqlite3 *db, const char *type, H1 *h1);

#endif
