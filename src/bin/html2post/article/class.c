#include "../Article.h"
#include "../Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static struct Article_vtable *vtable;

static const char *NextHeader(const char *data) {
	while (*data) {
		if (data[0] == '<') {
			if (data[1] == 'h' && data[2] == '2') {
				return data;
			} else if (data[1] == '/' && data[2] == 'b' && data[3] == 'o' && data[4] == 'd' && data[5] == 'y' && data[6] == '>') {
				return data;
			}
		}
		data++;
	}
	return data;
}


static int SetFileName(Article *this, const char *data) {
	if (this->fileName) {
		free((void *)(this->fileName));
		this->fileName = 0;
	}

	char *tmpFile = (char *)malloc(sizeof(char) * (strlen(data) + 6));
	if (!tmpFile) {
		perror("Article.SetFileName");
		exit(2);
	}
	sprintf(tmpFile, "%s.post", data);

	this->fileName = tmpFile;

	while (*tmpFile) {
		if (*tmpFile == '.') {
			// keep as is
		} else if (isascii(*tmpFile) && isalnum(*tmpFile)) {
			if (isupper(*tmpFile)) {
				*tmpFile = tolower(*tmpFile);
			}
		} else {
			*tmpFile = '-';
		}
		tmpFile++;
	}

	return 1;
}


static int SetID(Article *this, const char *data) {
	if (this->id) {
		free((void *)(this->id));
		this->id = 0;
	}

	const char *startOfID = strchr(data, '"');
	if (startOfID) {
		startOfID++;
		const char *endOfID = strchr(startOfID, '"');
		if (endOfID) {
			this->id = StrNDup(startOfID, (int)(endOfID - startOfID));
		}
	}

	if (!this->id) {
		this->id = StrDup("no-id-provided");
		return 0;
	}

	return 1;
}


static int SetRawHeading(Article *this, const char *data) {
	if (this->rawHeading) {
		free((void *)(this->rawHeading));
		this->rawHeading = 0;
	}

	const char *p = data;
	while (*p && *p != '\n') {
		if (p[0] == '<' && p[1] == '/' && p[2] == 'h' && p[3] == '2' && p[4] == '>') {
			p += 4;
			break;
		}
		p++;
	}
	if (*p == '>') {
		p++;
	} else if (*p == '\n') {
		p--;
	}

	this->rawHeading = StrNDup(this->startOfData, (int) (p - this->startOfData));

	return 1;
}


static int SetTitle(Article *this, const char *data) {
	if (this->title) {
		free((void *)(this->title));
		this->title = 0;
	}

	const char *startOfTitle = strchr(data, '>');
	if (startOfTitle) {
		startOfTitle++;
		const char *endOfTitle = strchr(startOfTitle, '<');
		if (endOfTitle) {
			this->title = StrNDup(startOfTitle, (int)(endOfTitle - startOfTitle));
		}
	}

	if (!this->title) {
		this->title = StrDup("No Title Provided");
		return 0;
	}

	return 1;
}


static int WritePost(Article *this, const char *path) {
	char fname[1024];
	snprintf(fname, 1024, "%s%s", path, this->fileName);
	fname[1023] = 0;
	if (strlen(fname) != (strlen(path) + strlen(this->fileName))) {
		printf("\nerror:\tinternal buffer limit exceeded\n");
		printf("\t%-18s == %d\n", "maxPathLength", 1023);
		printf("\t%-18s == '%s'\n", "path", path);
		printf("\t%-18s == '%s'\n", "fileName", this->fileName);

		return 2;
	}

	FILE *fp = fopen(fname, "w");
	if (!fp) {
		perror(fname);
		printf("\nerror:\tunable to create file\n");
		printf("\t%-18s == '%s'\n", "fileName", fname);
		return 0;
	}

	if (fwrite(this->startOfData, sizeof(char) * (this->endOfData - this->startOfData), 1, fp) != 1) {
		perror(fname);
		fclose(fp);
		printf("\nerror:\tunable to create file\n");
		printf("\t%-18s == '%s'\n", "fileName", fname);
		return 0;
	}

	fclose(fp);

	printf(" post:\t%s\n", fname);

	return 1;
}


Article *new_Article(const char *data) {
	if (!vtable) {
		vtable = (struct Article_vtable *)malloc(sizeof(*vtable));
		if (!vtable) {
			perror("Article_vtable");
			exit(2);
		}
		memset(vtable, 0, sizeof(*vtable));
		vtable->SetFileName   = SetFileName;
		vtable->SetID         = SetID;
		vtable->SetRawHeading = SetRawHeading;
		vtable->SetTitle      = SetTitle;
		vtable->WritePost     = WritePost;
	}

	Article *this = (Article *)malloc(sizeof(*this));
	if (!this) {
		perror("new_Article");
		exit(2);
	}
	memset(this, 0, sizeof(*this));
	this->vtable = vtable;

	this->fileName    = 0;
	this->id          = 0;
	this->rawHeading  = 0;
	this->title       = 0;

	this->startOfData = NextHeader(data);
	if (*this->startOfData == 0 || strncmp(this->startOfData, "</body>", 7) == 0) {
		this->startOfData = 0;
	} else if (*this->startOfData) {
		this->endOfData = NextHeader(this->startOfData + 1);
	} else {
		this->endOfData = this->startOfData;
	}

	if (this->startOfData) {
		this->vtable->SetRawHeading(this, this->startOfData);
		this->vtable->SetID(this, this->rawHeading);
		this->vtable->SetTitle(this, this->rawHeading);
		this->vtable->SetFileName(this, this->title);
	}

	return this;
}


Article *delete_Article(Article *this) {
	if (this) {
		if (this->fileName) {
			free((void *)(this->fileName));
		}
		if (this->id) {
			free((void *)(this->id));
		}
		if (this->rawHeading) {
			free((void *)(this->rawHeading));
		}
		if (this->title) {
			free((void *)(this->title));
		}

		free(this);
	}
	return 0;
}
