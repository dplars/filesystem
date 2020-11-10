/*
 *      namei.c         met eventuele stub : om te kunnen testen 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define EXT extern
#include "hfs.h"


//#ifdef STUB
short namei(char *cpn)
{
	static Inode	ipb;
	Inode	*ip;
	char	*cpl;
	short wdir = 0;

	printf(" diract: offset in dir : "); scanf("%hd%*c", &u.u_diract);
	printf(" parent inode nummer : "); scanf("%hd%*c", &u.u_pdir);
	printf(" inode nummer : "); scanf("%hd%*c", &wdir);
	if ( wdir != 0 )
	{
		ip = LeesInode(wdir, &ipb);
		if ( verbose && ip )
		{
			printf("%3d : ", wdir);
			DrukInode(ip);
		}
		u.u_dirent.d_ino = wdir;
	}
	else
	{
		ip = (struct inode *)NULL;
		u.u_dirent.d_ino = 0;
	}
	/* afsplitsen van de laatste component van de padnaam */
	cpl = strrchr(cpn, '/');
	if ( cpl == NULL ) 
		cpl = cpn;
	else
		cpl++;
	memset(u.u_dirent.d_naam, '\0', NAMELEN);
	strncpy(u.u_dirent.d_naam, cpl, NAMELEN);
	if ( verbose )
		printf("dirent %d %-7.7s   diract %d   pdir %d\n",
			u.u_dirent.d_ino, u.u_dirent.d_naam, u.u_diract, u.u_pdir); 
	u.u_error = 0;
	return wdir;
}
/*#else
short namei(char *cpn)
{
	printf("namei bestand  %s \n", cpn);
	return 1;
}
#endif*/
