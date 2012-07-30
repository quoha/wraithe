#ifndef wraithe_src_bin_wraithe_token_Token_H
#define wraithe_src_bin_wraithe_token_Token_H

typedef struct Token Token;
struct Token {
	struct Token_vtable *vtable;

	Token  *prev;
	Token  *next;
	char   *code;
	char   *data;
};

struct Token_vtable {
};

Token *new_Token(const char *data, int len);
Token *delete_Token(Token *);

#endif
