#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define ASK_OPT "-?"

typedef struct {
	bool sync;
	bool wait;
	char * ask;
} options;

void
help()
{
	printf("usage: [-s|--sync] [-w|--wait] [-a|-?|--ask] [-h|--help]\n");
	printf("sync: run \"cave sync\" before upgrading\n");
	printf("wait: wait after upgrading (to read messages)\n");
	printf("ask: tell cave to ask before executing (needs a patched cave, like the one from the Keruspe overlay)\n");
	printf("help: print this help");
	exit(0);
}

options
get_options(int argc, char ** argv)
{
	options opts = {false, false, NULL};
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
			else if (strcmp(argv[i], "--ask") == 0)
				opts.ask = ASK_OPT;
			else if (strcmp(argv[i], "--help") == 0)
				help();
			continue;
		}
		for (j = 1 ; j < size ; ++j)
		{
			switch(argv[i][j])
			{
			case 's':
				opts.sync = true;
				break;
			case 'w':
				opts.wait = true;
				break;
			case '?':
			case 'a':
				opts.ask=ASK_OPT;
				break;
			case 'h':
				help();
			}
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
cave_resolve(options opts)
{
	pid_t pid = 0;
	if (!(pid = fork()))
	{
		if (opts.sync)
			waitpid(cave_sync(), NULL, 0);
		execl("/usr/bin/cave", "cave", "resolve", "-x1c", "-Cs", "-U", "*/*", "-d", "*/*", "-P", "*/*", "--suggestions", "ignore", "--recommendations", "ignore", "installed-slots", opts.ask, NULL);
	}
	return pid;
}

pid_t
cave_purge(options opts)
{
	pid_t pid = 0;
	if (!(pid = fork()))
	{
		waitpid(cave_resolve(opts), NULL, 0);
		if (opts.wait)
		{
			printf("Press any key to continue...");
			while (getchar() != '\n');
		}
		execl("/usr/bin/cave", "cave", "purge", "-x", opts.ask, NULL);
	}
	return pid;
}

int
main(int argc, char ** argv)
{
	options opts = get_options(argc, argv);
	waitpid(cave_purge(opts), NULL, 0);
	execl("/usr/bin/cave", "cave", "fix-linkage", "-x", "--", "-Ca", opts.ask, NULL);
	return 0;
}