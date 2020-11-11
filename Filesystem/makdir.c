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

void makdir(char *cpn)
{
	printf("Make dir %s voor %d \n", cpn,u.u_uid);
	
	Inode werkInode;
	memset(&werkInode, '\0',INOSIZE);
	
	// superblok inlezen om inode te kunnen vinden
	LeesSuperBlok();
	
	// namei oproepen, geeft gevulde inode terug. deze inode wegschrijven in superblok
	int inode = namei(cpn);
	if(inode > 0){
		// de file bestaat al
		LeesInode(inode, &werkInode);
	}
	
	if(inode == 0) {
	// er moet een nieuwe inode aangemaakt worden
		// vrije inode zoeken
		
		// uit superblok lezen waar de vrije inode en de vrije blokken zijn
		int vrijeInode = 1;
		while (!IsIfree(vrijeInode)) {
			vrijeInode++;
			if (vrijeInode > sbk.s_inode) {
				printf("Geen vrije inodes meer!\n");
				u.u_error = GEENINO;
				return;
			}
		}
		
		//inode op niet-vrij zetten in superblok
		SetIalloc(vrijeInode);
		
		schrijf_parent(vrijeInode);
		
        // Vrij blok zoeken
		int teller = 0;
		int vrijblok = 0;
		for(vrijblok = 1; vrijblok <= sbk.s_blok; vrijblok++) {
			printf("blok zoeken: %d \n",vrijblok);
			if(IsBfree(vrijblok)) {
				// blok is nog vrij, alloceren
				SetBalloc(vrijblok);
				werkInode.i_blok[teller] = vrijblok;
				teller++;
				break;
			}
		}
		if( vrijblok > sbk.s_blok) {
			printf("onvoldoende blokken vrij om een directory aan te maken");
		}
		
		// nu is i_blok aangevuld en is het superblok juist gemaakt, maar nog niet weggeschreven
		werkInode.i_uid = u.u_uid;
		werkInode.i_size = 2*DIRLEN;
		werkInode.i_mode = 0777 | S_IFDIR;	// arbitrair gekozen
		werkInode.i_link = 2;
		
		printf("vrije inode : %d \n",vrijeInode);
		// vrije inode is vrijeInode, op die plaats de werkInode schrijven
		SchrijfInode(vrijeInode, &werkInode);
		
		// pdir linkcount verhogen
		Inode pInode;
		LeesInode(u.u_pdir, &pInode);
		pInode.i_link += 1;
		SchrijfInode(u.u_pdir, &pInode);
		
		// voorbereiden chars om weg te schrijven
		char buf[BLOKSIZE];
        memset(&buf, '\0',BLOKSIZE);
		Dir *dirp;
		dirp = (Dir*)buf;
		
		dirp->d_ino = vrijeInode;
		strncpy(dirp->d_naam, ".", NAMELEN);
		dirp++;
		dirp->d_ino = u.u_pdir;
		strncpy(dirp->d_naam, "..", NAMELEN);
		SchrijfBlok(vrijblok, buf);	// vrijblok hier invullen en dan werkt het
		// hierbovcen verder afwerken
		
		SchrijfSuperBlok();
	}
		
}

