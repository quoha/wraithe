#include "../Articles.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static struct Articles_vtable *vtable;


static int Append(Articles *this, Article *a) {
	if (a && this->nextItem < this->maxItems) {
		this->list[this->nextItem++] = a;
		return 1;
	}
	return 0;
}


static Article *Curr(Articles *this) {
	if (this->currItem < this->nextItem) {
		return this->list[this->currItem];
	}
	return 0;
}


static void DumpHeaders(Articles *this) {
	printf("\n dump:\tindex of headers ===============================================\n");
	printf(" dump:\tindex believes it has %d articles\n", this->nextItem);

	Article *a = this->vtable->First(this);
	while (a) {
		printf("%5d:\traw   is %s\n", this->currItem, a->rawHeading);
		printf("\tid    is %s\n", a->id);
		printf("\ttitle is %s\n", a->title);
		printf("\tfname is %s\n", a->fileName);
		a = this->vtable->Next(this);
	}

	printf(" dump:\tindex of headers complete =========================================\n");
}


static Article *First(Articles *this) {
	this->currItem = 0;
	return this->vtable->Curr(this);
}


static Article *Next(Articles *this) {
	if (this->currItem < this->nextItem) {
		this->currItem++;
	}
	return this->vtable->Curr(this);
}


static int WritePosts(Articles *this, const char *path) {
	int      hasErrors = 0;
	Article *a = this->vtable->First(this);
	while (a) {
		if (!a->vtable->WritePost(a, path)) {
			hasErrors = 1;
		}

		a = this->vtable->Next(this);
	}

	return hasErrors == 0 ? 1 : 0;
}


Articles *new_Articles(const char *data, int maxItems) {
	if (!vtable) {
		vtable = (struct Articles_vtable *)malloc(sizeof(*vtable));
		if (!vtable) {
			perror("Articles_vtable");
			exit(2);
		}
		memset(vtable, 0, sizeof(*vtable));
		vtable->Append      = Append;
		vtable->DumpHeaders = DumpHeaders;
		vtable->Curr        = Curr;
		vtable->First       = First;
		vtable->Next        = Next;
		vtable->WritePosts  = WritePosts;
	}

	Articles *this = (Articles *)malloc(sizeof(*this));
	if (!this) {
		perror("new_Articles");
		exit(2);
	}
	memset(this, 0, sizeof(*this));

	this->vtable = vtable;
	
	if (maxItems > 0 && maxItems < (16 * 1024)) {
		this->maxItems = maxItems + 1;
	} else {
		this->maxItems = 4096;
	}
	this->currItem =  0;
	this->nextItem =  0;

	this->list = (Article **)malloc(sizeof(Article *) * (maxItems + 1));
	if (!this->list) {
		perror("Articles_list");
		exit(2);
	} else {
		int idx;
		for (idx = 0; idx <= this->maxItems; idx++) {
			this->list[idx] = 0;
		}
	}

	// extract the raw articles from the input file
	while (*data) {
		Article *a = new_Article(data);
		if (a->startOfData) {
			this->vtable->Append(this, a);
			data = a->endOfData;
		} else {
			a = delete_Article(a);
			break;
		}
	}

	return this;
}


Articles *delete_Articles(Articles *this) {
	if (this) {
		int idx;
		for (idx = 0; idx < this->nextItem; idx++) {
			delete_Article(this->list[idx]);
		}
		free(this->list);
	}

	return 0;
}
