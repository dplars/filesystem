/*
 *   Bram Lauwens                
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

void unlnk(char *cpn)
{
	Dir *dp;
	char buf[BLOKSIZE];
	dp = (Dir*)buf;
	
	int resultaatnamei;
	Inode werkInode;
    
    LeesSuperBlok();
	
	resultaatnamei=namei(cpn);
	printf("resultaat van namei : %d \n", resultaatnamei); // namei oproepen
	if ( resultaatnamei<=0 ) // namei mislukt dus return
	{
		u.u_error=GEENINO;
		printf("resultaat van namei failed: %d \n", resultaatnamei);
		return;
	}
	LeesInode(resultaatnamei,&werkInode);
	
	int Mapgevonden=0;
	int Leeg=1;
	
	if((werkInode.i_mode&S_IFMT) == S_IFDIR) // als hete een directory is
	{
		Mapgevonden=1; // gevonden
		printf("Dit is een map \n");
		//
		int offset=0;
		printf("werkInode size: %d \n",werkInode.i_size);
		Leeg=0;
		
		while(offset<werkInode.i_size) // lezen 
		{
			printf("offset %i \n",offset);
			if ((offset%BLOKSIZE)==0)
			{
				LeesBlok(werkInode.i_blok[offset/BLOKSIZE],buf);
				dp=(Dir*)buf;
			}
			DrukBlok(buf,BLOKSIZE); // druk blok af voor controle
			if(dp->d_ino!= 0) // bekijken of de directory leeg is of niet, INODE mag niet 0 zijn
			{
				if (((strcmp(dp->d_naam, ".")==0)) || ((strcmp(dp->d_naam, "..")==0))) // lege directory
				{
					printf("direcorty is leeg \n");
					Leeg=1;
				}
				else
				{
					printf("direcorty is niet leeg hoezee \n"); // niet leeg dus we bekijken verder
					Leeg=0;
					break;
				}
			}
			offset+=DIRLEN;
			dp++;
		}
	}
	
	if(Leeg) // leeg maken indien het kan dooe verlagen link counts
	{
		printf("dit is leeg of bevat een bestand\n");
		printf("Pinode");
		Inode p;
		LeesInode(u.u_pdir,&p);
		DrukInode(&p);
		printf("werkInode-< i_link: %d \n",werkInode.i_link);
		printf("werkInode size: %d \n",werkInode.i_size);
		
		werkInode.i_link-=1;
		
		LeesBlok(p.i_blok[u.u_diract/BLOKSIZE],buf);
		if(Mapgevonden)
		{
			printf("parent is %d \n",p.i_link);
			p.i_link--;
			printf("parent is %d \n",p.i_link);
			SchrijfInode(u.u_pdir,&p);
		}
		
		dp+=(u.u_diract%BLOKSIZE)/DIRLEN;
		dp->d_ino=0;
        printf("schrijf blok %d weg \n",p.i_blok[u.u_diract/BLOKSIZE] );
		SchrijfBlok(p.i_blok[u.u_diract/BLOKSIZE],buf);
		
		if(werkInode.i_link<=0)
		{
			int ditbloknummer=werkInode.i_size/BLOKSIZE;
			if (werkInode.i_size%BLOKSIZE)
			{
				ditbloknummer++;
			}
			werkInode.i_link=0;
			werkInode.i_mode=0;
			werkInode.i_size=0;
			
			for(int k=0;k<ditbloknummer;k++)
			{
				SetBfree(werkInode.i_blok[k]);
				werkInode.i_blok[k]=0;
			}
			SetIfree(resultaatnamei);
			
			SchrijfSuperBlok();
		}
		DrukInode(&werkInode);
		SchrijfInode(resultaatnamei,&werkInode);
	}
	else
	{
		u.u_error=NIETLEEG;
		printf("De map bevat bestanden \n");
		return;
	}
}
