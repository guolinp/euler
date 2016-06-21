#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define FROM_STDOUT "From stdout: "
#define FROM_STDERR "From stderr: "
#define FROM_FILE   "From file  : "

#define LOG_FILE    "fork.log"

int main(int argc, char *argv[])
{
	pid_t pid;
	FILE *fp;

	fp = fopen(LOG_FILE, "w+");
	if (!fp) {
		fprintf(stderr, "open file %s failed\n", LOG_FILE);
		return -1;
	}

	setbuf(fp, NULL);

	pid = fork();
	if (pid == 0) {
		fprintf(stdout, FROM_STDOUT "this is child process\n");
		fprintf(stderr, FROM_STDERR "this is child process\n");
		fprintf(fp, FROM_FILE "this is child process\n");
	} else if (pid > 0) {
		fprintf(stdout, FROM_STDOUT "this is parent process\n");
		fprintf(stderr, FROM_STDERR "this is parent process\n");
		fprintf(fp, FROM_FILE "this is parent process\n");
	} else {
		fprintf(stderr, "fork failed\n");
		return -1;
	}

	fclose(fp);
	return 0;
}

/* gcc fork.c -o fork */
