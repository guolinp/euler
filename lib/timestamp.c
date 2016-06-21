#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "timestamp.h"

unsigned long timestamp_microseconds(void)
{
	struct timeval tv;
	if (gettimeofday(&tv, 0))
		return 0;
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

unsigned long timestamp_milliseconds(void)
{
	return timestamp_microseconds() / 1000;
}

unsigned long timestamp_seconds(void)
{
	return timestamp_microseconds() / 1000000;
}

int timestamp_string(char *buf, int buf_size)
{
	time_t timep;
	struct tm *p;
	int n;

	if (!buf || buf_size <= 0)
		return -1;

	if (time(&timep) == -1)
		return -1;

	p = localtime(&timep);
	if (p == NULL)
		return -1;

	n = snprintf(buf, buf_size,
			"%04d-%02d-%02d %02d:%02d:%02d",
			p->tm_year + 1900,
			p->tm_mon + 1,
			p->tm_mday,
			p->tm_hour,
			p->tm_min,
			p->tm_sec);

	if (n >= buf_size)
		n = buf_size - 1;
	buf[n] = '\0';

	return 0;
}
