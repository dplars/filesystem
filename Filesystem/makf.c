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

void schrijf_parent(int inodenr);

void makfile(char *cpn,  short mode, short size, char opvul)
{
	Inode werkInode;
	memset(&werkInode, '\0',INOSIZE);
	
	printf("makFile %s voor %d  met %o : %d * %c tekens \n",
				cpn,u.u_uid,mode,size,opvul);
				
	// superblok inlezen om inode te kunnen vinden
	LeesSuperBlok();
    
    for (int i = 0; i< 20; i++) {
        printf("%c", cpn[i]);
    }
    printf("\n", cpn);
    
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
		
		// vrije blokken zoeken
	
		//  blok schrijven
		// checken hoeveel datablokken de file zal innemen elke 32 bytes 1 datablok dus size delen door 32 en afronden naar boven
		int aant_blokken = 0;
		aant_blokken = size/BLOKSIZE;
		if(size%BLOKSIZE > 0) {
			aant_blokken++;
		}
		
		// inode invullen in parent inode
		schrijf_parent(vrijeInode);	// inode nummer meegeven en naam van de file die weggeschreven wordt
		
		// lijst maken om de vrije blokken in te steken
		int teller = 0;	// teller voor weg te schrijven in inode
		
		printf("aantal blokken te zoeken %d \naantal blokken nodig %d\n", sbk.s_blok, aant_blokken);	// 6
        if (size != 0) {
            int i = 0;
            for(i = 1; i <= sbk.s_blok; i++) {
                printf("blok zoeken: %d \n",i);
                if(IsBfree(i)) {
                    // blok is nog vrij, alloceren
                    SetBalloc(i);
                    werkInode.i_blok[teller] = i;
                    teller++;
                    if(teller >= aant_blokken) {
                        break;
                    }
                }
            }
            if( i > sbk.s_blok) {
                printf("niet voldoende vrije blokken gevonden tot %d gezocht \n", i);
                // filesize aanpassen aan de grootte van aantal gevonden blokken
                size = teller*BLOKSIZE;
                printf("nieuwe size van de file toevoegen: %d \n", size);
                u.u_error = GEENBLO;
                
                // aant blokken opnieuw bereken
                aant_blokken = size/BLOKSIZE;
                if(size%BLOKSIZE > 0) {
                    aant_blokken++;
                }
            }
        }
        else {
            printf("bestandsgrootte is NUL, kan geen file blok alloceren \n");
        }
		
		
		
		// nu is i_blok aangevuld en is het superblok juist gemaakt, maar nog niet weggeschreven
		werkInode.i_uid = u.u_uid;
		werkInode.i_size = size;
		werkInode.i_mode = mode | S_IFREG;
		werkInode.i_link = 1;
	
		printf("vrije inode : %d \n",vrijeInode);
		// vrije inode is vrijeInode, op die plaats de werkInode schrijven
		SchrijfInode(vrijeInode, &werkInode);
		
		//Inode pInode;
		//LeesInode(u.u_pdir, &pInode);
		
		// nu de datablokken vullen met de gegeven karakters
		// eerste is gelijk aan de hoeveelste Inode, rest is gelijk aan de opvulkarakters
		// hoeveel blokken moeten geschreven worden
		
		printf("aant blokken: %d voor %d grootte \n",aant_blokken, size);
        if (aant_blokken != 0) {
            // grootte van de file is nul dus geen blokken alloceren
            char buf[BLOKSIZE];
            for (int j = 0; j<aant_blokken; j++) {
                for (int i = 0; i<size-(BLOKSIZE*j); i++) {
                    buf[i] = opvul;
                }
                printf("schrijfblok werkinode: %d \n",werkInode.i_blok[j]);
                SetBalloc(werkInode.i_blok[j]);
                SchrijfBlok(werkInode.i_blok[j], buf);
                memset(&buf, '0', BLOKSIZE);
            }
        }
        else {
            printf("File grootte NUL, ik ben niet dom\n");
            
        }
		SchrijfSuperBlok();
		
		// nog verder werken als 3e file wordt toegevoegd -> zien dat dat in een echt vrij blok wordt gestoken en niet in blok 0
	}
}

void schrijf_parent(int inodenr) {
	// schrijft de inode en user gegvens in datablok van de parent inode (later ook nodig voor directories)
	
	// zoeken in parent inode on te kijken waar de locatie is voor de inhoud van de directory
	// dan kijken naar de grootte van die directory en checken of de file er nog bij past, anders 
	// een nieuwe datablok toevoegen om de file in weg te schrijven
		
	// user u_pdir lezen om inode te krijgen waar de gegevens van de file mogen geschreven worden
	Inode pInode;
	LeesInode(u.u_pdir, &pInode);
	
	int lengte;
	int toevoegplaats;
    int schrijfblok_plus_een = 0;
	if (pInode.i_size == u.u_diract) {
		// de directory is gevuld -> geen gaatje, achteraan toevoegen na de laatst gevulde positie van de parent inode
			
		// als oorspronkelijke size een veelvoud van 32 is, een nieuw datablok maken omdat de vorige helemaal gevuld is
		if(pInode.i_size%BLOKSIZE != 0) {
			// geen veelvoud van 32, dus kan achteraan gewoon toegevoegd worden in het datablok dat al beschreven staat in de parent
            printf("achteraan toevoegen\n");
			lengte = u.u_diract+DIRLEN;	// 8 bijtellen om het einde van het blok aan te geven
			toevoegplaats = u.u_diract;
		}
		else {
			// wel een veelvoud van 32, dus een nieuw datablok zoeken en gebruiken voor deze file
			// nieuw datablok ophalen -> leeg blok zoeken
			// in sbk zoeken naar vrije datablokken
			int vrijblok = 1;
			int i = 1;
			while (!IsBfree(i)) {
                printf("vrij blok zoeken om parent te vergroten blok nakijken: %d \n", i);
				if ( i > sbk.s_blok) {
					// geen vrij blok gevonden
					printf("geen vrij blok gevonden in schrijf_parent \n");
					u.u_error = GEENBLO;
					return;
				}
				i++;
			}
			SetBalloc(i);
			vrijblok = i;
            printf("waarde van vrijblok %d \n", vrijblok);
			// vrij blok gevonden
			
			// dit blok toevoegen aan de parent inode
				// berekenen waar in bloklijst de vrije blok moet gestoken worden
				int vrijeplaats = pInode.i_size/BLOKSIZE;
			pInode.i_blok[vrijeplaats] = vrijblok;
			toevoegplaats = u.u_diract;
			lengte = u.u_diract + DIRLEN;
		}
	}
	else {
		// gewoon invullen op de plaats van diract
		toevoegplaats = u.u_diract;
		lengte = pInode.i_size;
        printf("invullen op plaats van diract toevoegplaats %d \n", toevoegplaats);
        schrijfblok_plus_een = 1;
	}
	
	// toevoegen in datablok op plaats waar parent gegevens staan
	// datablok lezen
	// plaats in datablok berekenen
    
    
	int index = pInode.i_size/BLOKSIZE;
    printf("index for schrijfblokop : %d \n pinode.i_size/bloksize : %d\n", index, pInode.i_size/BLOKSIZE);
    int schrijfblokop = pInode.i_blok[index];
    if (schrijfblok_plus_een) {
        schrijfblokop+=1;
    }
	
	
	// voorbereiden chars om weg te schrijven
	char buf[BLOKSIZE];
    memset(&buf, '\0',BLOKSIZE);
	Dir *dirp;
	dirp = (Dir*)buf;
	
	printf("schrijfblok op: %d \n",schrijfblokop);
	LeesBlok(schrijfblokop, buf);
	
	// een aantal keer dirp verhogen naargelang de plaats in de blok 
	// als datablok op is, dirp terug vooraan plaatsen
    
	// delen met rest
	if ( toevoegplaats%BLOKSIZE >= 0 ) {
		dirp += toevoegplaats%BLOKSIZE/DIRLEN;
	}
	//printf("dirp %d toevoegplaats %d \n", dirp, toevoegplaats);
	
	// nu dirp vullen 
	dirp->d_ino = inodenr;
	strncpy(dirp->d_naam, u.u_dirent.d_naam, NAMELEN);
	
	SchrijfBlok(schrijfblokop, buf);
	
	// size van de parent inode verhogen met 8
	pInode.i_size = lengte;
	
	// inode terug wegschrijven
	SchrijfInode(u.u_pdir, &pInode);
	
	SchrijfSuperBlok();
	
}

