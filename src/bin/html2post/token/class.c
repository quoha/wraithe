#include "../Token.h"
#include "../Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct Token_vtable *vtable;

Token *new_Token(const char *data, int len) {
	if (!vtable) {
		vtable = (struct Token_vtable *)malloc(sizeof(*vtable));
		if (!vtable) {
			perror("Token_vtable");
			exit(2);
		}
		memset(vtable, 0, sizeof(*vtable));
	}

	Token *this = (Token *)malloc(sizeof(*this));
	if (!this) {
		perror("new_Token");
		exit(2);
	}
	memset(this, 0, sizeof(*this));

	this->vtable = vtable;

	this->data = StrNDup(data, len);

	return this;
}

Token *delete_Token(Token *this) {
	if (this) {
		if (this->prev) {
			this->prev = this->next;
		}

		if (this->next) {
			this->next = this->prev;
		}

		if (this->data) {
			free(this->data);
		}

		free(this);
	}

	return 0;
}
