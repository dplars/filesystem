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

void lnk( char *cpn, char *new)
{
	printf("Link %s %s\n", cpn, new);
    
    LeesSuperBlok();
    
    // oproep: link(bron, doel);
    // voor bron checken of de file bestaat
    int bronInode = namei(cpn);
    if (bronInode <= 0) {
        // De bron inode bestaat nog niet, kan geen file linken die niet bestaat
        printf("Het bron bestand bestaat niet \n");
        u.u_error = GEENINO;
        return;
    }
    
    // Bron bestand bestaat wel, inode nummer is gegeven in bronInode
    // Inode uitlezen
    Inode bInode;
    memset(&bInode, '\0',INOSIZE);      // inode leegmaken
    LeesInode(bronInode, &bInode);
    
    // link count van de bron inode verhogen
    bInode.i_link = ++bInode.i_link;
    
    SchrijfInode(bronInode, &bInode);
    
    //################################################################
    // Voor doel inode
    // checken of inode bestaat
    int doelInode = namei(new);
    if (doelInode > 0) {
        // De doel inode (nieuwe file) bestaat al, kan dus niet gelinkt worden want deze heeft al een eigen inode
        printf("Het doel bestand bestaat al \n");
        u.u_error = REEDS;
        return;
    }
    
    // De naam van het bestand wegschrijven in de parent directory en linken aan die inode (bronInode)
    schrijf_parent(bronInode);
    
    // superblok wegschrijven na aanpassingen
    SchrijfSuperBlok();
    
    
    
}

