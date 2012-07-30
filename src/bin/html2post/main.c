#include "local.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int OptIs(const char *s1, const char *s2) {
	return (s1[0] == '-' && s1[1] == '-' && strcmp(s1 + 2, s2) == 0) ? 1 : 0;
}

int main(int argc, char *argv[]) {
stdout = stderr;

	int   idx;
	int   showPosts           = 1;
	int   showPostContents    = 0;

	const char *input        = 0;
	const char *output       = 0;

	for (idx = 1; idx < argc; ++idx) {
		char *opt = argv[idx];
		char *val = opt;
		while (*val && *val != '=') {
			val++;
		}
		if (*val == '=') {
			*(val++) = 0;
		} else {
			val = 0;
		}

		if (OptIs(opt, "help")) {
			printf("usage:\thtml2post [opt] --input=xxx --output-dir=xxx\n");
			printf("\t--input=xxx            use xxx for input       [required]\n");
			printf("\t--output-dir=xxx       use xxx for output      [required]\n");
			printf("\t                       should end with /\n");
			printf("\t--show-posts           debugging show posts\n");
			printf("\t--show-posts-contents  debugging show content\n");
			printf("\t--version              show version and halt\n");
			return 2;
		} else if (OptIs(opt, "version")) {
			printf(" info:\thtml2post version " PKG_VER "\n");
			return 2;
		} else if (OptIs(opt, "input") && val && *val) {
			input = val;
		} else if (OptIs(opt, "output-dir") && val && *val) {
			output = val;
		} else if (OptIs(opt, "show-posts")) {
			showPosts = 1;
		} else if (OptIs(opt, "show-post-contents")) {
			showPostContents = 1;
		} else {
			printf("error:\tinvalid option '%s%s%s'\n", opt, val ? "=" : "", val ? val : "");
			return 2;
		}
	}

	if (!input || !output) {
		printf("error:\tinvalid command line\n\ttry --help if you're stuck\n");
		return 2;
	}

	printf(" info:\t%-18s == '%s'\n", "input", input);
	printf("\t%-18s == '%s'\n", "outputDir", output);

	// read in the combined html file
	InputBuffer *ib = new_InputBuffer(input);
	if (!ib->startOfData) {
		printf("error:\tunable to create input buffer\n");
		return 2;
	}

	printf(" info:\tread %d bytes from %s\n", (int)(ib->endOfData - ib->startOfData), ib->fileName);

	// extract the articles from the input buffer
	Articles *index = new_Articles(ib->startOfData, 4 * 1024);
	index->vtable->DumpHeaders(index);

	index->vtable->WritePosts(index, output);

	printf(" info:\ti think that it worked\n");

	return 0;
}
