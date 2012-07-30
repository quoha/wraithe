#include "../local.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *StrDup(const char *s) {
	if (!s) {
		s = "";
	}

	char *t = (char *)malloc(sizeof(char) * (strlen(s) + 1));
	if (!t) {
		perror("StrDup");
		exit(2);
	}

	strcpy(t, s);

	return t;
}
