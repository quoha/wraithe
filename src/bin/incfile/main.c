/*************************************************************************
 * wraithe/src/bin/incfile/main.c
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
	const char *wraitheDB   = 0;

	int idx;

	for (idx = 1; idx < argc; idx++) {
		char *opt, *val;
		SetOpt(argv[idx], &opt, &val);
		if (OptIs(opt, "help")) {
			printf("\nusage:\tincfile --wraithe-db=xxx\n");
			printf("\tadd source files to a sqlite3 database\n");
			printf("\t--help           print this text\n");
			printf("\t--version        show version information\n");
			return 2;
		} else if (OptIs(opt, "version")) {
			printf(" info:\tincfile " PKG_VER " compiled on " __DATE__ " at " __TIME__"\n\n");
			return 2;
		} else if (OptIs(opt, "wraithe-db") && val && *val) {
			wraitheDB = val;
		} else {
			printf("\nerror:\tinvalid command line option\n\t%s%s%s\n", opt, val ? "=" : "", val ? val : "");
			return 2;
		}
	}

	if (!wraitheDB) {
		printf("\nerror:\tinvalid command line\n\tmissing --wraithe-db=xxx\n\ttry the --help option if you're stuck\n\n");
		return 2;
	}

	printf(" info\t%-18s == '%s'\n", "wraithe-db", wraitheDB);

	printf(" info:\tconnecting to the wraithe database...\n");
	sqlite3 *db = 0;
	if (sqlite3_open_v2(wraitheDB, &db, SQLITE_OPEN_READWRITE, 0) != SQLITE_OK) {
		printf("\nerror:\tunable to open the wraithe database\n\t%-18s == '%s'\n\n", "wraithe-db", wraitheDB);
		return 2;
	}

	int id = -1;

	const char *select =
		"\tselect id\n"
		"\t     , text\n"
		"\t  from articles\n"
		"\t where write_dttm is null\n";

	const char *text = "foo <";

	Translate(id, text);

	printf(" info:\tupdate completed\n");

	sqlite3_close(db);

	return 0;
}
