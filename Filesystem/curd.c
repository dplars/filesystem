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

void curd(char *cpn, int wortel)
{
	printf("Current dir  %s \n", cpn);
}

