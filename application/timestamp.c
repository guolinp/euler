#include <stdio.h>
#include "timestamp.h"

int main(int argc, char *argv[])
{
	unsigned long ret;
	char buf[32];

	ret = timestamp_seconds();
	printf("timestamp_seconds: %lu\n", ret);

	ret = timestamp_milliseconds();
	printf("timestamp_milliseconds: %lu\n", ret);

	ret = timestamp_microseconds();
	printf("timestamp_microseconds: %lu\n", ret);

	ret = timestamp_string(buf, 32);
	if (ret < 0)
		printf("timestamp_string: cannot get time string\n");
	else
		printf("timestamp_string: %s\n", buf);

	return 0;
}

/* gcc timestmp.c -o timestmp */
