/*
 *                   
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#define EXT extern
#include "hfs.h"

void unlnk(char *cpn)
{
	printf("Unlink %s \n", cpn);
}

