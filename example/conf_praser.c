#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>

#include "typedef.h"
#include "dmdb.h"

void
usage()
{
	printf("usage: conf_praser file1 \n");
}

int
main(int argc, char *argv[])
{
	struct conf_l *l;
	char *p;

	if (argc < 2) {
		usage();
		return -1;
	}

	l = conf_file2list(argv[1]);

	conf_printf_ct(l->ct, 0, LOG_ALERT);

	p = conf_list2text(l);
	if (p == NULL) {
		printf("Failed!\n");
		return -1;
	}
	printf("Result:\n%s\n", p);

	free(p);
	conf_list_free(l);

	return 0;
}
