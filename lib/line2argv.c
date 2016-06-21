#include <stdio.h>
#include <string.h>

#include "line2argv.h"

size_t line2argv(char *str, char *argv[], size_t size)
{
	size_t argc = 0;

	if (!strlen(str))
		return 0;

	while (*str && *str == ' ')
		str++;

	if (!*str)
		return 0;

	argv[argc++] = str;

	while ((str = strchr(str, ' '))) {
		*str++ = '\0';

		while (*str && *str == ' ')
			str++;

		if (!*str)
			break;

		argv[argc++] = str;

		if (argc == size) {
			fprintf(stderr, "%s: Too many parameters (max %u)\n", __FUNCTION__, size - 1);
			return 0;
		}
	}

	argv[argc] = NULL;

	return argc;
}
