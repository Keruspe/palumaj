#include "run_bg.h"

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
	bool retry;
	char * ask;
} options;

void
help(char * caller)
{
	printf("usage: %s [-s|--sync] [-w|--wait] [-a|-?|--ask] [-r|--retry] [-h|--help]\n", caller);
	printf("sync: run \"cave sync\" before upgrading\n");
	printf("wait: wait after upgrading (to read messages)\n");
	printf("ask: tell cave to ask before executing (needs a patched cave, like the one from the Keruspe overlay)\n");
	printf("retry: ask if you want to retry when upgrade fails\n");
	printf("help: print this help\n");
	exit(0);
}

void
get_options(options * opts, int argc, char ** argv)
{
	opts->sync = false;
	opts->wait = false;
	opts->retry = false;
	opts->ask = NULL;
	int i, j, size;
	for (i = 1 ; i < argc ; ++i) {
		size = strlen(argv[i]);
		if ((size < 2) || (argv[i][0] != '-'))
			continue;
		if (argv[i][1] == '-')
		{
			if (strcmp(argv[i], "--sync") == 0)
				opts->sync = true;
			else if (strcmp(argv[i], "--wait") == 0)
				opts->wait = true;
			else if (strcmp(argv[i], "--ask") == 0)
				opts->ask = ASK_OPT;
			else if (strcmp(argv[i], "--retry") == 0)
				opts->retry = true;
			else if (strcmp(argv[i], "--help") == 0)
				help(argv[0]);
			continue;
		}
		for (j = 1 ; j < size ; ++j)
		{
			switch(argv[i][j])
			{
			case 's':
				opts->sync = true;
				break;
			case 'w':
				opts->wait = true;
				break;
			case '?':
			case 'a':
				opts->ask = ASK_OPT;
				break;
			case 'r':
				opts->retry = true;
				break;
			case 'h':
				help(argv[0]);
			}
		}
	}
}

void
cave_sync()
{
	exec_bg_and_wait("/usr/bin/cave", "cave", "sync", NULL);
}

void
cave_resolve(options * opts)
{

	bool retry = true;
	while (retry && exec_bg_and_wait("/usr/bin/cave", "cave", "resolve", "-x1c", "-Cs", "-U", "*/*", "-d", "*/*", "-P", "*/*", "--suggestions", "ignore", "--recommendations", "ignore", "world", opts->ask, NULL) && opts->retry)
	{
		char c;
		printf("Do you want to retry ? [Y/n] : ");
		while ((c=getchar()) != '\n')
		{
			if (c == 'N' || c == 'n')
			{
				retry = false;
				opts->wait = false;
			}
		}
	}
}

void
cave_purge(const char * ask)
{
	exec_bg_and_wait("/usr/bin/cave", "cave", "purge", "-x", ask, NULL);
}

void
cave_fix_linkage(const char * ask)
{
	exec_bg_and_wait("/usr/bin/cave", "cave", "fix-linkage", "-x", "--", "-Ca", ask, NULL);
}

int
main(int argc, char ** argv)
{
	options opts;
	get_options(&opts, argc, argv);
	if (opts.sync) cave_sync();
	cave_resolve(&opts);
	if (opts.wait)
	{
		printf("Press any key to continue...");
		while (getchar() != '\n');
	}
	cave_purge(opts.ask);
	cave_fix_linkage(opts.ask);
	return 0;
}
