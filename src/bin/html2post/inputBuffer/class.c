#include "../InputBuffer.h"
#include "../Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static struct InputBuffer_vtable *vtable;

static char *TranslatePHP(char *buffer) {
	char *p = buffer;
	while (*p) {
		if (p[0] == '<' && p[1] == '!' && p[2] == '-' && p[3] == '-' && isspace(p[4]) && p[5] == 'p' && p[6] == 'h' && p[7] == 'p' && isspace(p[8])) {
			break;
		} else if (isspace(p[0]) && p[1] == 'p' && p[2] == 'h' && p[3] == 'p' && isspace(p[4]) && p[5] == '-' && p[6] == '-' && p[7] == '>') {
			break;
		} else {
			p++;
		}
	}

	char *to = p;
	while (*p) {
		if (p[0] == '<' && p[1] == '!' && p[2] == '-' && p[3] == '-' && isspace(p[4]) && p[5] == 'p' && p[6] == 'h' && p[7] == 'p' && isspace(p[8])) {
			*(to++) = '<';
			*(to++) = '?';
			*(to++) = 'p';
			*(to++) = 'h';
			*(to++) = 'p';
			p += 8;
		} else if (isspace(p[0]) && p[1] == 'p' && p[2] == 'h' && p[3] == 'p' && isspace(p[4]) && p[5] == '-' && p[6] == '-' && p[7] == '>') {
			*(to++) = *p;
			*(to++) = '?';
			*(to++) = '>';
			p += 8;
		} else {
			*(to++) = *(p++);
		}
	}

	*to = 0;

	return buffer;
}

static char *TranslateSmartQuotes(char *buffer) {
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

InputBuffer *new_InputBuffer(const char *fileName_) {
	if (!vtable) {
		vtable = (struct InputBuffer_vtable *)malloc(sizeof(*vtable));
		if (!vtable) {
			perror("InputBuffer_vtable");
			exit(2);
		}
		memset(vtable, 0, sizeof(*vtable));
	}

	InputBuffer *this = (InputBuffer *)malloc(sizeof(*this));
	if (!this) {
		perror("new_InputBuffer");
		exit(2);
	}

	memset(this, 0, sizeof(*this));

	this->vtable = vtable;
	this->line   = 0;
	this->col    = 0;
	this->startOfData = 0;
	this->endOfData   = 0;
	this->data   = 0;

	this->fileName = StrDup(fileName_);

	struct stat statBuf;
	if (stat(this->fileName, &statBuf) != 0) {
		perror(this->fileName);
		exit(2);
	}

	FILE *fp = fopen(this->fileName, "r");
	if (!fp) {
		perror(this->fileName);
		exit(2);
	}

	this->data = (char *)malloc(sizeof(char) * (statBuf.st_size + 1));
	if (!this->data) {
		perror("InputBuffer_data");
		exit(2);
	}

	if (fread(this->data, statBuf.st_size, 1, fp) != 1) {
		perror(this->fileName);
		exit(2);
	}

	this->startOfData  = this->data;
	this->endOfData    = this->data + statBuf.st_size;
	this->endOfData[0] = 0;

	fclose(fp);

	TranslatePHP(TranslateSmartQuotes(this->startOfData));

	return this;
}

InputBuffer *delete_InputBuffer(InputBuffer *this) {
	if (this) {
		free(this->data);
		free(this->fileName);
		free(this);
	}

	return 0;
}
