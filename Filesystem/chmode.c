/*
 *                   de uitbreiding
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#define EXT extern
#include "hfs.h"

void chmode(char *cpn, short mode)
{
	printf("chmode(N)  %o %s \n", mode, cpn);
}

