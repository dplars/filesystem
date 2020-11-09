/*
 * hfs.c : een filesysteem simulatie programma
 *                          aanpassingen 15 oktober 2004
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define EXT 
#include "hfs.h"

extern int	optind;
extern char	*optarg;
int	verbose = 0;

main( int	argc, char	*argv[] )
{
	int		cc;
	char	str[SYSLEN] ;
	int		a1, a2, a3, i, k;
	char	*ptr;
	char	*cpn;

	strncpy(sysnaam, NAAMFS, SYSLEN);
	while ( (cc=getopt(argc, argv, "vf:h")) != EOF )
	{
		switch (cc)
		{
		case 'f':
			strncpy(sysnaam, optarg, SYSLEN);
			break;
		case 'v':
			verbose++;
			break;
		case 'h':
		default:
			fprintf(stderr, "gebruik: hfs [-v] [-f naam]\n");
			break;
		}
	}
	if ( access(sysnaam, 0) >= 0 )
	{
		LeesSuperBlok();
		DrukSuperBlok(&sbk);
	}
	/* initialisatie van de user structuur */
	u.u_uid = u.u_gid = 0;   u.u_cdir = u.u_rdir = 1;  u.u_mask = 026;
	while ( fgets(str, SYSLEN, stdin) != NULL  )
	{
		if ( (cpn = strrchr(str, '\n')) != NULL )
			*cpn = '\0';
		if ( verbose )
			printf("Gelezen : %s\n", str);
		i = 1;
		while ( str[i] == ' ' )
			i++;
		cpn = &str[i];
		switch(str[0])
		{
		case 'f' :
			while ( str[i] != ' ' )
				i++;
			str[i] = '\0';
			if ( verbose )
			{
				for(k=0; k<=i; k++)
					printf("%2c", str[k]);
				printf(" : %d \n", i);
			}
			i++;
			a2 = strtol(&str[i], &ptr, 8);      /* mode */
			a3 = strtol(ptr+1, &ptr, 10);     /* lengte */
            
			makfile(cpn, (short)a2, (short)a3, *(ptr+1) );
			break ;
		case 'm' :
			makdir(cpn);
			break ;
		case 's' :
			fstatus(cpn);
			break ;
		case 'u' :
			unlnk(cpn);
			break ;
		case 'l' :
			while ( str[i] != ' ' )
				i++;
			str[i] = '\0';
			i++;
			lnk(cpn, &str[i]);
			break ;
		case 'z' :
			zien() ;
			break ;
		case 'i' :
			a1 = strtol(cpn, &ptr, 10) ;      /* aantal inodes */
			if ( a1%2 ) a1++;
			a2 = strtol(ptr+1, &ptr, 10) ;    /* aantal blokken */
			mkfs((short)a1,(short)a2) ;
			break ;
		case 'a' :
			u.u_mask = strtol(cpn, &ptr, 8) ; /* mask */
			break ;
		case 'e' :
			u.u_uid = strtol(cpn, &ptr, 10) ; /* uid */
			break ;
		case 'c' :
			curd(cpn, 0);
			break ;
		case 'r' :
			curd(cpn, 1);
			break ;
		case 'n' :
			a1 = strtol(&str[i], &ptr, 8) ;   /* mode */
			chmode(ptr+1, (short)a1);
			break ;
		case 'd' :
			a1 = strtol(&str[i], &ptr, 10) ;  /* uid */
			chuid(ptr+1, (char)a1);
			break ;
		case 'p' :
			DrukProc();
			break ;
		case 'q' :
			exit(0);
		default:
		case '?' :
			printf("i f m l u : s p z q : a e d n c r \n") ;
			printf("i aantal_inodes aantal_blokken      a mask \n") ;
			printf("f naam mode size teken              e uid \n") ;
			printf("m naam                              d uid naam  \n") ;
			printf("l naam naam                         n mode naam \n") ;
			printf("u naam                              c naam \n") ;
			printf("s naam                              r naam \n") ;
			break ;
		}
		if ( verbose )
			DrukProc();
		memset(str,0,SYSLEN);
	}
}

void DrukProc(void)
{
	printf("U: Id %d,%d Mask %.3o  Err %d  Cd %d Rd %d Pd %d ",
		u.u_uid, u.u_gid, u.u_mask, u.u_error, u.u_cdir, u.u_rdir, u.u_pdir);
	printf("  diract %d  Entry |%.2d|%-7.7s|\n",
		 u.u_diract, u.u_dirent.d_ino, u.u_dirent.d_naam);
}

void zien(void)
{
	char buf[BLOKSIZE];
	Superblok *sb;
	Inode *ip;
	char *bp;
	int	fd, i, j, k, nin, nbuf;

	fd = open(sysnaam, O_RDONLY);
	if ( fd == -1 )
	{
		fprintf(stderr, "Er is nog niets te zien %s\n", sysnaam);
		return;
	}
	if ( read(fd, buf, BLOKSIZE) <= 0 )
	{
		fprintf(stderr, "Er is nog niets te zien %s\n", sysnaam);
		return;
	}
	sb = (Superblok *)buf;
	nin = sb->s_inode/2; nbuf = sb->s_blok;
	DrukSuperBlok(sb);
	for(i=1; i<=nin; i++)
	{
		read(fd, buf, BLOKSIZE);
		ip = (Inode *)buf;
		for (k=1; k>=0; k--, ip++ )
		{
			printf("%2d : ", 2*i-k);
			DrukInode(ip);
		}
	}
	for(i=1; i<=nbuf; i++)
	{
		read(fd, buf, BLOKSIZE);
		bp = (char *)buf;
		printf("%2d : ", i);
		DrukBlok(bp, BLOKSIZE);
	}
	close(fd);
}

void DrukSuperBlok(Superblok *sb)
{
	int	i;

	printf("%2d %2d ", sb->s_inode, sb->s_blok);
	for (i=0; i<MAXINODE; i++) printf("%.2x", sb->s_fi[i]);
	printf("  ");
	for (i=0; i<MAXBLOK; i++) printf("%.2x", sb->s_fb[i]);
	printf("\n");
}

void DrukInode(Inode *ip)
{
	int	j;

	printf("%2d %2d %4x %3d : ", ip->i_uid, ip->i_link,
	                             ip->i_mode, ip->i_size);
	for(j=0; j<BLKADDR; j++)
		printf("%2d ",ip->i_blok[j]);
	printf("\n");
}

void DrukBlok(char *bp, int len)
{
	int	j;

	for (j=0; j<len; j++)
		if ( *(bp+j) < 32 )               /* non-printable tekens */
			printf("%2x", *(bp+j) );
		else
			printf("%2c", *(bp+j) );
	printf("\n");
}
