#ifndef wraithe_src_bin_wraithe_input_InputBuffer_H
#define wraithe_src_bin_wraithe_input_InputBuffer_H

typedef struct InputBuffer InputBuffer;
struct InputBuffer {
	struct InputBuffer_vtable *vtable;

	char *fileName;
	int   line;
	int   col;
	char *startOfData;
	char *endOfData;
	char *data;
};

struct InputBuffer_vtable {
	void (*MagicTranslation)(InputBuffer *);
};

InputBuffer *new_InputBuffer(const char *fileName);

#endif
