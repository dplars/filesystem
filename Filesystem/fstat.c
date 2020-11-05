/*
 *         fstat.c  : opvragen van de inhoud van een inode
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#define EXT extern
#include "hfs.h"


void fstatus(char *cpn)
{
	Inode inode;
	Inode *ip;
	short ino;

	printf("Status bestand  %s \n", cpn);
	ino = namei(cpn);
	if ( ino > 0 )
	{
		ip = LeesInode(ino, &inode);
		DrukInode(ip);
	}
	else
		printf("status %s : geen inode gevonden\n", cpn);
}

