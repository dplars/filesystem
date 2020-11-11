/*
 *      namei.c         met eventuele stub : om te kunnen testen 
 	Bram Lauwens
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define EXT extern
#include "hfs.h"
#include <ctype.h>


short namei(char *cpn)
{
    verbose = 1;

    if (verbose) {
        printf("NAMEI \n");
        for (int i = 0; i< 20; i++) {
            printf("%c\n", cpn[i]);
        }
        
    }
	
	Inode werkinode;
	int werkinodenummer;
	int teller;
	int naam=0;
	int intelezenblokken;
	char buf[BLOKSIZE];
	Dir *dirp;
	int terug=1;
	int lus;
    int gevonden = 0;
	
	// if padnaam start bij root = starten met /
	memset(u.u_dirent.d_naam, '\0', NAMELEN);
	u.u_diract=0;
	if( cpn[0] =='/')
	{
		
        if (verbose) {printf("gestart bij de root van pad \n");}
		// werkinode = rootinode
		werkinodenummer=u.u_rdir; // is 1
		teller=1;
        if (verbose) {printf(" werkinodenummer is %d \n", werkinodenummer);}
	}
	else
	{
        if (verbose) {printf(" niet gestart bij de root van pad \n");}
		// werkinode = inode van actuele directory
		werkinodenummer=u.u_cdir; // is 0
		teller =0;
        if (verbose) {printf(" werkinodenummer is %d \n", werkinodenummer);}
	}
	
	// nu lezen we inode
	LeesInode(werkinodenummer, &werkinode);
	// DrukInode(werkinode);
	
	// testen of inode een directory is
	if ((werkinode.i_mode & S_IFMT) != S_IFDIR)
	{
        if (verbose) {printf(" ai geen directory \n");}
		u.u_error=GEENDIR;
		return -1;
	} 
	
	// zolang er nog een padnaam is, zolang er een / is
	//while( cpn[teller] != '\0')
    // voor makdir kan het ook een \0 zijn
    while(!isdigit(cpn[teller]) && cpn[teller] != '\0')
	{
        
        //gevonden = 0;
		
		// lees volgende component van padnaam uit input
		//printf(" er is nog padnaam met charchter %c \n" , cpn[teller]);
		
		memset(u.u_dirent.d_naam, '\0', NAMELEN);
		while ( cpn[teller] != '/')
		{
			// proberen nu woord per woord in te lezen en te kopieren
			u.u_dirent.d_naam[naam] = cpn[teller];
            if (verbose) {printf(" de teller van naam is %d met waarde %c  \n ", naam, u.u_dirent.d_naam[naam]);}
			teller++;
			naam++;
			if (cpn[teller] == '\0')
			{
				break;
			}
            gevonden = 0;
		}
		teller++;
		// nu hebben we in u.u_dirent.d_naam[naam] bijvoorbeeld temp staan
		// zoeken naar de innode, inlezen van innode 0 of 1 zit in werkinode
		// goed kijken of de i_size kleiner is dan 32, of 64,...
		
		intelezenblokken=(werkinode.i_size/BLOKSIZE); 
		if ( werkinode.i_size%BLOKSIZE != 0)
		{
			intelezenblokken++;
            if (verbose) {printf("eentje bijgeteld");}
		}
        if (verbose) {printf(" intelezenblokken is %d \n", intelezenblokken);}
		
		u.u_pdir=werkinodenummer;
		for(int i=0;i<intelezenblokken;i++)
		{
            
            if (gevonden) {
                break;
            }
            //u.u_pdir=werkinodenummer;
            LeesBlok(werkinode.i_blok[i], buf);
            DrukBlok(buf,BLOKSIZE); // 32 is bij default
            // output: 1 . 0 0 0 0 0 0 1 . . 0 0 0 0 0 0 j e f 0 0 0 0 3 t m p 0 0 0 0
            
            if (verbose) {printf("for lus in te lezen blokken");}
			if ((werkinode.i_mode & S_IFMT) != S_IFDIR)
						{
                            if (verbose) {printf(" ai geen directory \n");}
							u.u_error=GEENDIR;
							u.u_pdir=1;
							//u.u_pdir= de oude; niet nodig mss lijn 100 aanpassen naar hierna pas
							return -1;
						} 
			
			
			
			// ingelezen in array buf en typecasten naar een dir structuur
			dirp=(Dir*) buf;
			// nakijken of het overeenkomt
			
            if (verbose) {printf("u.u_dirent.d_naam is %s \n", u.u_dirent.d_naam);}
			
			terug=-1;
            if (verbose) {printf("werkinode i size is %d \n",werkinode.i_size);}
			
			int verschil= werkinode.i_size - i*BLOKSIZE;
			if (verschil<BLOKSIZE)
			{
				lus=werkinode.i_size%BLOKSIZE;
			}
			else
				lus=BLOKSIZE;
				
			if (intelezenblokken>1)
			{
                printf("intelezen blokken = %d; werkinode?i_size %d \n", intelezenblokken, werkinode.i_size);
				lus=werkinode.i_size-DIRLEN;
                if (werkinode.i_size%BLOKSIZE) {
                    lus= ((werkinode.i_size-DIRLEN)/BLOKSIZE)*BLOKSIZE-DIRLEN;     // als lus = 40 -> 32 ingeven om mee verder te werken
                }
                printf("lus = %d \n", lus);
			}
			for( int j=0; j<=lus/DIRLEN;j++)
			{
                if (verbose) {
                    printf("j is %d \n", j);
                    printf("dirp->d_naam is %s \n", dirp->d_naam);
                }
				if (strncmp(u.u_dirent.d_naam, dirp->d_naam,NAMELEN)==0)
				{
					if ( dirp->d_ino != 0)
					{
                        if (verbose) {printf("gevonden en inode niet 0\n");}
						werkinodenummer=dirp->d_ino;
						
						u.u_diract=j*DIRLEN;
						LeesInode(werkinodenummer, &werkinode);
						
						naam=0;
						terug=werkinodenummer;
                        gevonden = 1;
					}
					else
                    if ( dirp->d_ino == 0)
					{
                        if (verbose) {printf("deze is recent verwijderd en innodde is 0\n");}
						u.u_diract=j*DIRLEN;
						terug=0;
					}
				}
				else
				{
                    if (verbose) {printf("nog niet gevonden \n");}
                    if (!gevonden) {
                        terug = 0;
                    }
                    
                    //u.u_diract=j*DIRLEN;
				}
				if ( dirp->d_ino == 0)
				{
                    printf("inode 0 op het einde, gevonden: %d \n", gevonden);
					
                    u.u_diract=j*DIRLEN;
                    if(gevonden !=1 ) {
                        terug = 0;
                    }
                    // helpt link met leeg plaatsje vinden en diract juist zetten
                    gevonden = 1;
                    
				}
                if (j == BLOKSIZE/DIRLEN && !gevonden) {
                    // tot op het einde moeten zoeken
                    u.u_diract=j*DIRLEN;
                    printf("tot op einde moeten zoeken in blok: u.u_diract = %d j = %d \n", u.u_diract,j);
                }
				dirp++;
			}
		}
        
	}
	
    
        printf ("namei goed verlopen met eindresultaat %d \n ", terug);
    if (verbose) {
        printf("namei bestand  %s  %c \n", cpn,cpn[0]);
        printf("u.u_diract: %d",u.u_diract);
    }
	return terug;
	
}

