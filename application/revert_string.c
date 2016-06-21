#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"

/* alloc memory to save target s,
 * the caller should free it when unused */
char *revert(const char *s)
{
	int i, len;
	char *new;

	GODIE_ON(s == NULL, "invalid input");

	new = MALLOC((len = strlen(s)) + 1);
	for (i = 0; i < len; i++)
		new[i] = s[len - i - 1];

	return new;
}

int main(int argc, char *argv[])
{
	char *s = "hello world";
	char *d = revert(s);

	P("%s -> %s\n", s, d);

	FREE(d);

	return 0;
}

/* gcc revert_string.c -o revert_string */
