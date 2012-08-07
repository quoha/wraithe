/*************************************************************************
 * wraithe/src/bin/wraithe/postArticle.c
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*************************************************************************
 */
static const char *FileName(sqlite3 *db, int typeID, const char *title) {
	static char buffer[1024];

	if (strlen(title) + 8 > 1023) {
		printf("\nerror:\tmaximum path length exceeded\n");
		printf("\t%-18s == '%s'\n", "title", title);
		exit(2);
	}

	char       *p    = buffer;
	const char *path = 0;
	const char *sql  =
		"\tselect output_path\n"
		"\t  from page_types\n"
		"\t where id = ?\n";

	sqlite3_stmt *pStmt = 0;
	const char   *pTail = 0;
	if (sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &pStmt, &pTail) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not compile SQL to fetch page output path\n");
		exit(2);
	}

	int colNo = 0;
	if (sqlite3_bind_int(pStmt, ++colNo, typeID) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind id\n");
		printf("\t%-18s == %d\n", "id", typeID);
		exit(2);
	}

	int rc;
	do {
		rc = sqlite3_step(pStmt);
		switch (rc) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			path = (const char *)sqlite3_column_text(pStmt, 0);
			if (path) {
				if (strlen(path) + strlen(title) + 8 > 1023) {
					printf("\nerror:\tmaximum path length exceeded\n");
					printf("\t%-18s == '%s'\n", "path", path);
					printf("\t%-18s == '%s'\n", "title", title);
					exit(2);
				}

				while (*path) {
					*(p++) = *(path++);
				}
			}
			break;
		default:
			printf("  sql:%s\n", sql);
			printf("\nerror:\t%s\n", sqlite3_errmsg(db));
			exit(2);
		}
	} while (rc == SQLITE_ROW);

	sqlite3_finalize(pStmt);

	while (*title) {
		if (isalnum(*title)) {
			*(p++) = tolower(*(title++));
		} else {
			*(p++) = '-';
			title++;
		}
	}

	*(p++) = '.';
	*(p++) = 'h';
	*(p++) = 't';
	*(p++) = 'm';
	*(p++) = 'l';
	*(p++) = 0;

	return buffer;
}

/*************************************************************************
 */
static const char *FetchCRC(sqlite3 *db, int id) {
	static char crc[128];

	const char *sql =
		"\tselect text_crc\n"
		"\t  from articles\n"
		"\t where id = ?\n";

	sqlite3_stmt *pStmt = 0;
	const char   *pTail = 0;
	if (sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &pStmt, &pTail) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not compile SQL to fetch article CRC\n");
		exit(2);
	}

	int colNo = 0;
	if (sqlite3_bind_int(pStmt, ++colNo, id) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind id\n");
		printf("\t%-18s == %d\n", "id", id);
		exit(2);
	}

	int rc;
	do {
		rc = sqlite3_step(pStmt);
		switch (rc) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			strncpy(crc, (const char *)sqlite3_column_text(pStmt, 0), 128);
			crc[127] = 0;
			break;
		default:
			printf("  sql:%s\n", sql);
			printf("\nerror:\t%s\n", sqlite3_errmsg(db));
			exit(2);
		}
	} while (rc == SQLITE_ROW);

	sqlite3_finalize(pStmt);

	return crc;
}

/*************************************************************************
 * return -1 if article is not found
 */
static int FetchID(sqlite3 *db, int typeID, const char *title) {
	int id = -1;

	const char *sql =
		"\tselect id\n"
		"\t  from articles\n"
		"\t where page_type_id = ?\n"
		"\t   and title        = ?\n";

	sqlite3_stmt *pStmt = 0;
	const char   *pTail = 0;
	if (sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &pStmt, &pTail) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not compile SQL to fetch article ID\n");
		exit(2);
	}

	int colNo = 0;
	if (sqlite3_bind_int(pStmt, ++colNo, typeID) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind page_type_id\n");
		printf("\t%-18s == %d\n", "page_type_id", typeID);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, title, strlen(title), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind title\n");
		printf("\t%-18s == '%s'\n", "title", title);
		exit(2);
	}

	int rc;
	do {
		rc = sqlite3_step(pStmt);
		switch (rc) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			id = sqlite3_column_int(pStmt, 0);
			break;
		default:
			printf("  sql:%s\n", sql);
			printf("\nerror:\t%s\n", sqlite3_errmsg(db));
			exit(2);
		}
	} while (rc == SQLITE_ROW);

	sqlite3_finalize(pStmt);

	return id;
}

/*************************************************************************
 */
static int FetchType(sqlite3 *db, const char *type) {
	int id = -1;

	const char *sql =
		"\tselect id\n"
		"\t  from page_types\n"
		"\t where type = ?\n";

	sqlite3_stmt *pStmt = 0;
	const char   *pTail = 0;
	if (sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &pStmt, &pTail) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not compile SQL to fetch type\n");
		exit(2);
	}

	if (sqlite3_bind_text(pStmt, 1, type, strlen(type), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind type\n");
		printf("\t%-18s == '%s'\n", "type", type);
		exit(2);
	}

	int rc;
	do {
		rc = sqlite3_step(pStmt);
		switch (rc) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			id = sqlite3_column_int(pStmt, 0);
			break;
		default:
			printf("  sql:%s\n", sql);
			printf("\nerror:\t%s\n", sqlite3_errmsg(db));
			exit(2);
		}
	} while (rc == SQLITE_ROW);

	sqlite3_finalize(pStmt);

	return id;
}

/*************************************************************************
 */
static int InsertArticle(sqlite3 *db, int typeID, H1 *h1, const char *crc) {
	printf(" info:\tinserting '%s'\n", h1->title);

	const char *fileName = FileName(db, typeID, h1->title);

	const char *sql =
		"\tinsert into articles (\n"
		"\t  page_type_id\n"
		"\t, create_dttm\n"
		"\t, update_dttm\n"
		"\t, write_dttm\n"
		"\t, hit_counter\n"
		"\t, file_name\n"
		"\t, title\n"
		"\t, keywords\n"
		"\t, text_crc\n"
		"\t, text\n"
		"\t) values (\n"
		"\t  ?          -- page_type_id\n"
		"\t, datetime() -- create_dttm\n"
		"\t, datetime() -- update_dttm\n"
		"\t, null       -- write_dttm\n"
		"\t, 0          -- hit_counter\n"
		"\t, ?          -- file_name\n"
		"\t, ?          -- title\n"
		"\t, null       -- keywords\n"
		"\t, ?          -- text_crc\n"
		"\t, ?||?||?||? -- text\n"
		"\t)\n";

	sqlite3_stmt *pStmt = 0;
	const char   *pTail = 0;
	if (sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &pStmt, &pTail) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not compile SQL to insert article\n");
		exit(2);
	}

	int colNo = 0;
	if (sqlite3_bind_int(pStmt, ++colNo, typeID) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind page_type_id\n");
		printf("\t%-18s == %d\n", "page_type_id", typeID);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, fileName, strlen(fileName), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind file_name\n");
		printf("\t%-18s == '%s'\n", "file_name", fileName);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->title, strlen(h1->title), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind title\n");
		printf("\t%-18s == '%s'\n", "title", h1->title);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, crc, strlen(crc), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind crc\n");
		printf("\t%-18s == '%s'\n", "crc", crc);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->tag, strlen(h1->tag), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.tag\n");
		printf("\t%-18s == '%s'\n", "text.tag", h1->tag);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->title, strlen(h1->title), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.title\n");
		printf("\t%-18s == '%s'\n", "text.title", h1->title);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->close, strlen(h1->close), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.close\n");
		printf("\t%-18s == '%s'\n", "text.close", h1->close);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->text, strlen(h1->text), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.text\n");
		printf("\t%-18s == '%s'\n", "text.text", "<<snipped>>");
		exit(2);
	}

	int rc;
	do {
		rc = sqlite3_step(pStmt);
		switch (rc) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			break;
		default:
			printf("  sql:%s\n", sql);
			printf("\nerror:\t%s\n", sqlite3_errmsg(db));
			exit(2);
		}
	} while (rc == SQLITE_ROW);

	sqlite3_finalize(pStmt);

	return 1;
}

/*************************************************************************
 */
static int UpdateArticle(sqlite3 *db, int id, H1 *h1, const char *crc) {
	const char *sql =
		"\tupdate articles\n"
		"\t   set update_dttm = datetime()\n"
		"\t     , text_crc    = ?\n"
		"\t     , text        = ?||?||?||?\n"
		"\t     , write_dttm  = null\n"
		"\t where id          = ?\n";

	sqlite3_stmt *pStmt = 0;
	const char   *pTail = 0;
	if (sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &pStmt, &pTail) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not compile SQL to update article\n");
		exit(2);
	}

	int colNo = 0;
	if (sqlite3_bind_text(pStmt, ++colNo, crc, strlen(crc), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind crc\n");
		printf("\t%-18s == '%s'\n", "crc", crc);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->tag, strlen(h1->tag), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.tag\n");
		printf("\t%-18s == '%s'\n", "text.tag", h1->tag);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->title, strlen(h1->title), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.title\n");
		printf("\t%-18s == '%s'\n", "text.title", h1->title);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->close, strlen(h1->close), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.close\n");
		printf("\t%-18s == '%s'\n", "text.close", h1->close);
		exit(2);
	} else if (sqlite3_bind_text(pStmt, ++colNo, h1->text, strlen(h1->text), 0) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind text.text\n");
		printf("\t%-18s == '%s'\n", "text.text", "<<snipped>>");
		exit(2);
	} else if (sqlite3_bind_int(pStmt, ++colNo, id) != SQLITE_OK) {
		printf("  sql:%s\n", sql);
		printf("\nerror:\t%s\n", sqlite3_errmsg(db));
		printf("\nerror:\tcould not bind id\n");
		printf("\t%-18s == %d\n", "id", id);
		exit(2);
	}

	int rc;
	do {
		rc = sqlite3_step(pStmt);
		switch (rc) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			break;
		default:
			printf("  sql:%s\n", sql);
			printf("\nerror:\t%s\n", sqlite3_errmsg(db));
			exit(2);
		}
	} while (rc == SQLITE_ROW);

	sqlite3_finalize(pStmt);

	return 1;
}

/*************************************************************************
 */
int PostArticle(sqlite3 *db, const char *type, H1 *h1) {
	int idType = FetchType(db, type);
	if (idType == -1) {
		printf(" post:\t%s/%s\n", type, h1->title);
		printf("\nerror:\tinvalid type for article\n");
		printf("\t%-18s == '%s'\n", "type", type);
		printf("\t%-18s == '%s'\n", "title", h1->title);
		return 0;
	}

	char crc[129];
	CRC((const unsigned char *)h1->text, strlen(h1->text), crc, 129);

	int id  = FetchID(db, idType, h1->title);

	if (id == -1) {
		printf(" post:\tinsert     %s/%s\n", type, h1->title);
		return InsertArticle(db, idType, h1, crc);
	} else if (strcmp(crc, FetchCRC(db, id)) != 0) {
		printf(" post:\tupdate     %s/%s\n", type, h1->title);
		return UpdateArticle(db, id, h1, crc);
	} 

	printf(" post:\tno changes %s/%s\n", type, h1->title);

	return 1;
}
