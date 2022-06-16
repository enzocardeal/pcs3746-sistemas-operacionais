#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

static const char * const programs[] = { "/child_program"};

void panic(const char *msg)
{
	fprintf(stderr, "%s: %s (errno = %d)\n", msg, strerror(errno), errno);
	exit(-1);
}

int main()
{
	init_semaphore();

	printf("Forking to run %d programs\n", 1);

	for (int i = 0; i < 1; i++) {
		const char *path = programs[i];
		pid_t pid = fork();
		if (pid == -1) {
			panic("fork");
		} else if (pid) {
			printf("Starting %s (pid = %d)\n", path, pid);
		} else {
			execl(path, path, (char *)NULL);
			panic("execl");
		}
	}
	print_message("init");

	int program_count = 1;
	while (program_count) {
		int wstatus;
		pid_t pid = wait(&wstatus);
		if (WIFEXITED(wstatus))
			printf("pid %d exited with %d\n", pid, WEXITSTATUS(wstatus));
		else if (WIFSIGNALED(wstatus))
			printf("pid %d killed by signal %d\n", pid, WTERMSIG(wstatus));
		else
			continue;
		program_count--;
	}
	return 0;
}
