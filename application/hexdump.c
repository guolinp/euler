#include <stdio.h>
#include "hexdump.h"

int main(int argc, char *argv[])
{
	char buff[512];
	hexdump(buff, 345);
	return 0;
}
