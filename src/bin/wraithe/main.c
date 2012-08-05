/*************************************************************************
 * wraithe/src/bin/wraithe/main.c
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

#include <stdio.h>

int main(int argc, char *argv[]) {
	const char *configFile  = "~/.wraitherc";
	const char *inputFile   = 0;
	const char *outputPath  = "";
	const char *preloadLib  = 0;
	const char *postloadLib = 0;
	const char *wraitheFile = 0;
	const char *wraitheDB   = 0;

	int foundErrors = 0;
	int idx;

	for (idx = 1; idx < argc; idx++) {
		char *opt, *val;
		SetOpt(argv[idx], &opt, &val);
		if (OptIs(opt, "help")) {
			return 2;
		} else if (OptIs(opt, "version")) {
			return 2;
		} else if (OptIs(opt, "config-file") && val && *val) {
			configFile = val;
		} else if (OptIs(opt, "input-file") && val && *val) {
			inputFile = val;
		} else if (OptIs(opt, "output-path") && val && *val) {
			outputPath = val;
		} else if (OptIs(opt, "post-write") && val && *val) {
			postloadLib = val;
		} else if (OptIs(opt, "pre-write") && val && *val) {
			preloadLib = val;
		} else if (OptIs(opt, "wraithe-db") && val && *val) {
			wraitheDB = val;
		} else if (OptIs(opt, "wraithe-file") && val && *val) {
			wraitheFile = val;
		} else {
			printf("\nerror:\tinvalid command line option\n\t%s%s%s\n", opt, val ? "=" : "", val ? val : "");
			return 2;
		}
	}

	if (!configFile) {
		printf("\nerror:\tinvalid command line\n\tmissing --config-file=xxx\n\ttry the --help option if you're stuck\n\n");
		return 2;
	} else if (!inputFile) {
		printf("\nerror:\tinvalid command line\n\tmissing --input=xxx\n\ttry the --help option if you're stuck\n\n");
		return 2;
	} else if (!outputPath) {
		printf("\nerror:\tinvalid command line\n\tmissing --output-path=xxx\n\ttry the --help option if you're stuck\n\n");
		return 2;
	} else if (!wraitheDB) {
		printf("\nerror:\tinvalid command line\n\tmissing --wraithe-db=xxx\n\ttry the --help option if you're stuck\n\n");
		return 2;
	}

	printf(" info:\t%-18s == '%s'\n", "config-file", configFile);
	printf("\t%-18s == '%s'\n", "pre-output", preloadLib ? preloadLib : "");
	printf("\t%-18s == '%s'\n", "post-output", postloadLib ? postloadLib : "");
	printf("\t%-18s == '%s'\n", "input-file", inputFile);
	printf("\t%-18s == '%s'\n", "output-path", outputPath);
	printf("\t%-18s == '%s'\n", "wraithe-file", wraitheFile ? wraitheFile : "not specified");
	printf("\t%-18s == '%s'\n", "wraithe-db", wraitheDB);

	// the preload file initializes the environment
	if (!LoadConfigFile(configFile)) {
		return 2;
	}

	// load the prelib if needed
	char *prelib = 0;
	if (preloadLib) {
		prelib = InputBuffer(preloadLib);
	}

	// load the postlib if needed
	char *postlib = 0;
	if (postloadLib) {
		postlib = InputBuffer(postloadLib);
	}

	// load the input file
	char *inp = InputBuffer(inputFile);
	//printf("\n==================================================================\n");
	//printf("%s\n", inp);
	//printf("\n==================================================================\n");

	HTML *html = HTMLTree(inp);

	if (wraitheFile) {
		printf(" info:\tcreating %s\n", wraitheFile);
		MakePath((char *)wraitheFile);
		FILE *fp = fopen(wraitheFile, "w");
		if (!fp) {
			perror(wraitheFile);
			return 2;
		}
		fprintf(fp, "<?php /* auto-generated by wraithe - do not edit */\n");
		if (html && html->body) {
			BODY *body = html->body;
			for (idx = 0; body->h1[idx]; idx++) {
				H1 *h1 = body->h1[idx];
				fprintf(fp, "\t$wraithe->AddArticle('");
				const char *p = h1->title;
				while (*p) {
					if (*p == '\'') {
						fputc('\\', fp);
					}
					fputc(*(p++), fp);
				}
				fprintf(fp, "');\n");
			}
		}
		fprintf(fp, "?>\n");
		fclose(fp);
	}

	// spin through the html.
	// information in the body text will be written to the top
	//    of every output file.
	// every h1 entry starts a new article.
	// each article is written to a PHP file based on the name
	//    of the article.
	//
	if (html && html->body) {
		//HTMLDump(html, stdout);
		for (idx = 0; html->body->h1[idx]; idx++) {
			H1 *h1 = html->body->h1[idx];
			WriteArticle(outputPath, prelib, html->body->text, h1, postlib);
		}
	}

	printf(" info:\tadding articles to the wraithe database...\n");
	sqlite3 *db = 0;
	if (sqlite3_open_v2(wraitheDB, &db, SQLITE_OPEN_READWRITE, 0) != SQLITE_OK) {
		printf("\nerror:\tunable to open the wraithe database\n\t%-18s == '%s'\n\n", "wraithe-db", wraitheDB);
		return 2;
	}
	if (html && html->body) {
		for (idx = 0; html->body->h1[idx]; idx++) {
			H1 *h1 = html->body->h1[idx];
			if (!PostArticle(db, "post", h1)) {
				printf("\nerror:\terror posting article\n");
				printf("\t%-18s == '%s'\n", "type", "post");
				printf("\t%-18s == '%s'\n", "title", h1->title);
				sqlite3_close(db);
				return 2;
			}
		}
	}
	printf("\tupdate completed\n");

	sqlite3_close(db);

	return 0;
}
