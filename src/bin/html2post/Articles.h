#ifndef wraithe_src_bin_wraithe_input_Articles_H
#define wraithe_src_bin_wraithe_input_Articles_H

#include "Article.h"

typedef struct Articles Articles;
struct Articles {
	struct Articles_vtable *vtable;

	int maxItems;
	int currItem;
	int nextItem;

	Article **list;
};

struct Articles_vtable {
	int      (*Append)(Articles *, Article *a);
	Article *(*First)(Articles *);
	Article *(*Curr)(Articles *);
	Article *(*Next)(Articles *);
	void     (*DumpHeaders)(Articles *);
	int      (*WritePosts)(Articles *, const char *path);
};

Articles *new_Articles(const char *data, int maxNumberOfArticles);
Articles *delete_Articles(Articles *);
	
#endif
