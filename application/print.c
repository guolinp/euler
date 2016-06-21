#include <stdio.h>

#include "print.h"

void function3()
{
	err_printf("this is err_printf test\n");
}

void function2()
{
	backtrace_printf("this is backtrace_printf test\n");
	dbg_printf("this is dbg_printf test\n");
	function3();
}

void function1()
{
	trc_printf("this is trc_printf test\n");
	function2();
}

int main(int argc, char *argv[])
{
	function1();
	return 0;
}
