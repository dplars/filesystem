/*
 * fssubr.c : steunroutines
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#define EXT extern
#include "hfs.h"
#include <string.h> /* memset */
#include <unistd.h> /* close */


void SchrijfSuperBlok(void )
{
	int	fd;

	if ( (fd = open(sysnaam, O_WRONLY)) == -1 )
	{
		fprintf(stderr,"SchrijfSuperBlok FS %s : niet te openen\n", sysnaam);
		exit(1);
	}
	write(fd, &sbk, BLOKSIZE);
	close(fd);
}
	
void SchrijfInode(int ino,Inode *ip)
{
	int	fd;

	if ( (fd = open(sysnaam, O_WRONLY)) == -1 )
	{
		fprintf(stderr,"SchrijfInode FS %s : niet te openen\n", sysnaam);
		exit(1);
	}
	lseek(fd, BLOKSIZE+(ino-1)*INOSIZE,0);
	write(fd, ip, INOSIZE);
	close(fd);
}
	
void SchrijfBlok(int blkno,char *bp)
{
	int	fd;

	if ( (fd = open(sysnaam, O_WRONLY)) == -1 )
	{
		fprintf(stderr,"SchrijfBlok FS %s : niet te openen\n", sysnaam);
		exit(1);
	}
	lseek(fd, BLOKSIZE+ sbk.s_inode*INOSIZE + (blkno-1)*BLOKSIZE,0);
	write(fd, bp, BLOKSIZE);
	close(fd);
}
	

Superblok* LeesSuperBlok(void )
{
	int	fd;

	if ( (fd = open(sysnaam, O_RDONLY)) == -1 )
	{
		fprintf(stderr,"LeesSuperBlok FS %s : niet te openen\n", sysnaam);
		exit(1);
	}
	read(fd, &sbk, BLOKSIZE);
	close(fd);
	return &sbk ;
}
	
Inode* LeesInode(int ino,Inode *ip)
{
	int	fd;

	if ( (fd = open(sysnaam, O_RDONLY)) == -1 )
	{
		fprintf(stderr,"LeesInode FS %s : niet te openen\n", sysnaam);
		exit(1);
	}
	lseek(fd, BLOKSIZE+(ino-1)*INOSIZE,0);
	read(fd, ip, INOSIZE);
	close(fd);
	return ip ;
}
	
char* LeesBlok(int blkno,char *bp)
{
	int	fd;

	if ( (fd = open(sysnaam, O_RDONLY)) == -1 )
	{
		fprintf(stderr,"LeesBlok FS %s : niet te openen\n", sysnaam);
		exit(1);
	}
	lseek(fd, BLOKSIZE+ sbk.s_inode*INOSIZE + (blkno-1)*BLOKSIZE,0);
	read(fd, bp, BLOKSIZE);
	close(fd);
	return bp ;
}
