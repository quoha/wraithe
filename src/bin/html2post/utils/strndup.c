#include "../local.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *StrNDup(const char *s, int len) {
	char *t = (char *)malloc(sizeof(char) * (len + 1));
	if (!t) {
		perror("StrDup");
		exit(2);
	}

	if (s && *s) {
		strncpy(t, s, len);
		t[len] = 0;
	} else {
		memset(t, 0, sizeof(char) * (len + 1));
	}

	return t;
}
