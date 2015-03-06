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
	printf("usage: conf_merge file1 file2 tbs (cover)\n");
}

int
main(int argc, char *argv[])
{
	struct conf_l *l1, *l2, *lm1, *lm2, *lm;
	char *p, *tbs[1];
	int cover = 0;

	if (argc < 4) {
		usage();
		return -1;
	}

	l1 = conf_file2list(argv[1]);
	l2 = conf_file2list(argv[2]);

	lm1 = conf_dup_l(l1);
	lm2 = conf_dup_l(l2);

	if (argc == 5)
		cover = 1;

	tbs[0] = argv[3];
	lm = conf_merge(lm1, lm2, tbs, 1, cover);

	conf_printf_ct(lm->ct, 0, LOG_ALERT);

	p = conf_list2text(lm);
	if (p == NULL) {
		printf("Failed!\n");
		return -1;
	}
	printf("Result:\n%s\n", p);

	free(p);
	conf_list_free(lm);
	conf_list_free(l1);
	conf_list_free(l2);

	return 0;
}
