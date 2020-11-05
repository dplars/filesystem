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

void chuid(char *cpn, char uid)
{
	printf("chuiD  %d %s \n", uid, cpn);
}

