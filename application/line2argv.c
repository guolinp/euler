#include <stdio.h>
#include <string.h>

#include "line2argv.h"

#define ARGC_MAX    8

static void parse_cmdline(char *cmdline)
{
	char *argv[ARGC_MAX + 1];
	size_t argc;
	int i;

	argc = line2argv(cmdline, argv, ARGC_MAX + 1);

	for (i = 0; i < argc; i++)
		printf("argv[%d]: %p %s\n", i, argv[i], argv[i]);
}

int main(int argc, char *argv[])
{
	char buff[128];
	char *cmdline;

	cmdline = fgets(buff, 128, stdin);
	if (cmdline) {
		printf("input %s\n", cmdline);
		parse_cmdline(cmdline);
	} else {
		printf("bad input\n");
	}

	return 0;
}
