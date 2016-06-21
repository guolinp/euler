#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <execinfo.h>

#include "print.h"

#define MAX_FRAMES    32

static void __backtrace_printf()
{
	void *frames[MAX_FRAMES];
	int frame_nr;
	char **symbol;
	int i;

	frame_nr = backtrace(frames, MAX_FRAMES);
	symbol = backtrace_symbols(frames, frame_nr);

	fprintf(stderr, "backtrace:\n");
	if (symbol) {
		for (i = 0; i < frame_nr; i++)
			fprintf(stderr, "[%02d]  %s\n", i, symbol[i]);
		fprintf(stderr, "\n");
	} else {
		fprintf(stderr, "cannot get backtrace\n");
	}
}

void backtrace_printf(const char *format, ...)
{
	va_list vargs;

	va_start(vargs, format);
	vfprintf(stderr, format, vargs);
	va_end(vargs);

	__backtrace_printf();
}

void err_printf(const char *format, ...)
{
	va_list vargs;

	va_start(vargs, format);
	vfprintf(stderr, format, vargs);
	va_end(vargs);

	backtrace_printf("!!! There is a crital error occurred !!!\n");

	exit(EXIT_FAILURE);
}
