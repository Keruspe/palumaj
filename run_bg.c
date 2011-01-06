#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 31

int
run_bg(void (*fun)(), int wait)
{
	pid_t pid = fork();
	int retval;
	if (!pid) {
		fun();
		exit(0);
	} else if (wait)
		waitpid(pid, &retval, 0);
	return retval;
}

int
run_bg_with_argv(void (*fun)(void *), void * argv, int wait)
{
	pid_t pid = fork();
	int retval;
	if (!pid) {
		fun(argv);
		exit(0);
	} else if (wait)
		waitpid(pid, &retval, 0);
	return retval;
}

int
exec_bg_and_wait(char * path, char * arg, ...)
{
	pid_t pid = fork();
	int retval;
	if (!pid) {
		char * argv[MAX_ARGS + 1];
		int argno = 0;
		va_list al;
		va_start(al, arg);
		while (arg && argno < MAX_ARGS)
		{
			argv[argno++] = arg;
			arg = va_arg(al, char *);
		}
		argv[argno] = NULL;
		va_end(al);
		execv(path, argv);
		exit(0);
	} else waitpid(pid, &retval, 0);
	return retval;
}

