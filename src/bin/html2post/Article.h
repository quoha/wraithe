#ifndef wraithe_src_bin_wraithe_input_Article_H
#define wraithe_src_bin_wraithe_input_Article_H

typedef struct Article Article;
struct Article {
	struct Article_vtable *vtable;

	const char *fileName;
	const char *id;
	const char *rawHeading;
	const char *title;

	const char *startOfData;
	const char *endOfData;
};

struct Article_vtable {
	int (*SetFileName)(Article *, const char *source);
	int (*SetID)(Article *, const char *source);
	int (*SetRawHeading)(Article *, const char *source);
	int (*SetTitle)(Article *, const char *source);
	int (*WritePost)(Article *, const char *path);
};

Article *new_Article(const char *startOfData);
Article *delete_Article(Article *);

#endif
