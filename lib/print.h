#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdio.h>

#define dbg_printf(fmt, args...)  fprintf(stderr, "function %s, line %d: "fmt, __FUNCTION__, __LINE__, ##args)
#define trc_printf(fmt, args...)  fprintf(stdout, fmt, ##args)

/* print message and exit program */
void err_printf(const char *format, ...);

/* print the backtrace info */
void backtrace_printf(const char *format, ...);

#endif
