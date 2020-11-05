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
#include <string.h> /* memset */
#include <unistd.h> /* close */

void mkfs(short ninode, short nblok)
{

	// datablokken aanmaken
	char buf[BLOKSIZE];
	Dir *dirp;
	
	dirp = (Dir*)buf;
	
	printf("mkfs met inode %d en blok %d \n",ninode,nblok);
	
	int fd;
	if((fd = creat(sysnaam,0666)) == -1) {
		printf("kan file niet maken");
		exit(0);
	}
	
	// superblock aanmaken
	
	sbk.s_inode = ninode;
	sbk.s_blok = nblok;
	
	// superblok vrijmaken
	for (int i = 2; i<=nblok ; i++) {
		SetBfree(i);
	}
	
	// inodes aanmaken
	// eerste allocceren en volgende ninode-1 free setten
	SetIalloc(1);
	
	for (int i = 2; i<=ninode; i++) {
		SetIfree(i);
	}
	
	SchrijfSuperBlok();
	
	// eerste inode aanmaken
	Inode inode;
	inode.i_uid = 0;
	inode.i_link = 2;
	inode.i_mode = S_IFDIR | 0777;
	inode.i_size = DIRLEN*2;
	inode.i_blok[0] = 1;
	for ( int j = 1; j <BLKADDR; j++) {
		inode.i_blok[j] = 0;
	}
	
	SchrijfInode(1, &inode);
	
	// lege inodes aanmaken
	memset(&inode,'\0', INOSIZE);
		
	for (int i = 2; i<=ninode; i++) {
		SchrijfInode(i, &inode);
	}
	
	memset(buf, '\0', BLOKSIZE);
	
	dirp->d_ino = 1;
	strncpy(dirp->d_naam, ".", NAMELEN);
	dirp++;
	dirp->d_ino = 1;
	strncpy(dirp->d_naam, "..", NAMELEN);
	SchrijfBlok(1, buf);
	
	memset(buf, '\0', BLOKSIZE);
	for (int k = 2; k <=nblok; k++) {
		SchrijfBlok(k, buf);
	}
	
	close(fd);
}

