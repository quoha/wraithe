/*************************************************************************
 * wraithe/src/bin/wraithe/html/tree.c
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

/*************************************************************************
 * bodyBlock := ( .* <body .*> text? h1block* </body> )
 * text      := ( !( <h1> | </body> )* )
 * h1block   := ( <h1> text? )
 */


#define MAXOBJ (16 * 1024)
static char *obj[MAXOBJ + 1];

static const char *inputBuffer;
static HTML       *html;
static H1         *h1;

/*************************************************************************
 */
static int Accept(const char *tag);
static int Expect(const char *tag);
static int BodyBlock(void);
static int BodyClose(void);
static int H1Block(void);
static int Text(void);

/*************************************************************************
 */
static int Accept(const char *tag) {
	const char *match = MatchPattern(tag, inputBuffer);
	if (!match) {
		return 0;
	}
	return 1;
}

/*************************************************************************
 */
static int Expect(const char *tag) {
	const char *match = MatchPattern(tag, inputBuffer);
	if (!match) {
		return 0;
	}
	inputBuffer = match;
	return 1;
}

/*************************************************************************
 * bodyBlock := ( .* <body .*> text? h1block* </body> )
 */
static int BodyBlock() {
	// .*
	while (*inputBuffer && !Accept("<body")) {
		inputBuffer++;
	}

	// <body
	const char *tag = inputBuffer;
	if (!Expect("<body")) {
		printf("\nerror:\tparser did not find <body> tag\n\n");
		return 0;
	}

	// .*>
	while (*inputBuffer && *inputBuffer != '>') {
		inputBuffer++;
	}
	if (!*inputBuffer) {
		printf("\nerror:\tparser did not find close '>' on body tag\n\n");
		return 0;
	}
	inputBuffer++;
	const char *text = inputBuffer;

	// text?
	Text();

	// action
	html->body = BODYNew(tag, text, inputBuffer);

	// h1block*
	while (H1Block()) {
	}

	// </body>
	if (!Expect("</body>")) {
		printf("\nerror:\tparser did not find </body> tag\n\n");
		printf("%s\n\n", inputBuffer);
		return 0;
	}

	return 1;
}

/*************************************************************************
 * h1block   := ( <h1> text? )
 */
static int H1Block() {
	// <h1>
	const char *startH1 = inputBuffer;
	if (Expect("<h1")) {
		// text?
		Text();
		const char *endH1 = inputBuffer;

		// action
		h1 = H1New(startH1, endH1);
		int idx;
		for (idx = 0; idx < MAXH1; idx++) {
			if (html->body->h1[idx] == 0) {
				html->body->h1[idx] = h1;
				break;
			}
		}

		return 1;
	}
	return 0;
}

/*************************************************************************
 * text := ( !( <h1> | </body> )* )
 */
static int Text() {
	while (*inputBuffer && !(Accept("<h1") || Accept("</body>"))) {
		inputBuffer++;
	}

	return 1;
}

/*************************************************************************
 * ( <body> code </body> )
 */
HTML *HTMLTree(char *buffer) {
	int   idx  = 0;

	html = HTMLNew();

	inputBuffer = buffer;

	if (!BodyBlock()) {
		printf("\nerror:\tunable to parse input\n\n");
		return 0;
	}

	return html;
}
