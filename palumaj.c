#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
	bool sync;
	bool wait;
} options;

void
help()
{
	printf("usage: [-s|--sync] [-w|--wait] [-h|--help]\nsync: run \"cave sync\" before upgrading\nwait: wait after upgrading (to read messages)\n");
}

options
get_options(int argc, char ** argv)
{
	options opts = {false, false};
	int i, j, size;
	for (i = 1 ; i < argc ; ++i) {
		size = strlen(argv[i]);
		if ((size < 2) || (argv[i][0] != '-'))
			continue;
		if (argv[i][1] == '-')
		{
			if (strcmp(argv[i], "--sync") == 0)
				opts.sync = true;
			else if (strcmp(argv[i], "--wait") == 0)
				opts.wait = true;
			else if (strcmp(argv[i], "--help") == 0)
				help();
			continue;
		}
		for (j = 1 ; j < size ; ++j)
		{
			if (argv[i][j] == 's')
				opts.sync = true;
			else if (argv[i][j] == 'w')
				opts.wait = true;
			else if (argv[i][j] == 'h')
				help();
		}
	}
	return opts;
}

pid_t
cave_sync()
{
	pid_t pid = 0;
	if (!(pid = fork()))
		execl("/usr/bin/cave", "cave", "sync", NULL);
	return pid;
}

pid_t
cave_resolve(bool sync)
{
	pid_t pid = 0;
	if (!(pid = fork()))
	{
		if (sync)
			waitpid(cave_sync(), NULL, 0);
		execl("/usr/bin/cave", "cave", "resolve", "-x1?c", "-Cs", "-U", "*/*", "-d", "*/*", "-P", "*/*", "--suggestions", "ignore", "--recommendations", "ignore", "installed-slots", NULL);
	}
	return pid;
}

pid_t
cave_purge(options opts)
{
	pid_t pid = 0;
	if (!(pid = fork()))
	{
		waitpid(cave_resolve(opts.sync), NULL, 0);
		if (opts.wait)
		{
			printf("Press any key to continue...");
			while (getchar() != '\n');
		}
		execl("/usr/bin/cave", "cave", "purge", "-x?", NULL);
	}
	return pid;
}

int
main(int argc, char ** argv)
{
	waitpid(cave_purge(get_options(argc, argv)), NULL, 0);
	execl("/usr/bin/cave", "cave", "fix-linkage", "-x", "--", "-?", "-Ca", NULL);
	return 0;
}
