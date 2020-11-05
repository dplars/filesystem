/*
 * hfs.h : globale definities voor een filesysteem simulatie programma
 *
 */

#define NAAMFS "klein"
#define SYSLEN  80
#define MAXINODE 12
#define MAXBLOK 16
#define BLOKSIZE 32
#define INOSIZE 16
#define BLKADDR 5
#define BLKINDIR (BLKADDR-1)
#define NAMELEN 7
#define DIRLEN (NAMELEN+1)

/* foutcodes */
#define GEENACC  801
#define GEENDIR  802
#define REEDS    803
#define GEENBLO  804
#define GEENINO  805
#define NOGNIET  806
#define NIETLEEG 807

/* toegangsrechten */
#define RRR 4
#define WWW 2
#define XXX 1

#define SetIalloc(n) sbk.s_fi[((n)-1)/8] &= ~(1<<(7-(((n)-1)%8)))
#define SetIfree(n) sbk.s_fi[((n)-1)/8] |= (1<<(7-(((n)-1)%8)))
#define IsIfree(n) (sbk.s_fi[((n)-1)/8] & (1<<(7-(((n)-1)%8))))
#define SetBalloc(n) sbk.s_fb[((n)-1)/8] &= ~(1<<(7-(((n)-1)%8)))
#define SetBfree(n) sbk.s_fb[((n)-1)/8] |= (1<<(7-(((n)-1)%8)))
#define IsBfree(n) (sbk.s_fb[((n)-1)/8] & (1<<(7-(((n)-1)%8))))

typedef	struct superblok
{
	short s_inode;
	short s_blok;
	unsigned char	s_fi[MAXINODE];
	unsigned char	s_fb[MAXBLOK];
} Superblok ;

typedef	struct inode
{
	unsigned char i_uid;
	unsigned char i_link;
	unsigned short i_mode;
	unsigned short i_size;
	unsigned short i_blok[BLKADDR];
} Inode ;

typedef	struct dir
{
	char	d_ino;
	char	d_naam[NAMELEN];
} Dir ;


typedef	struct ussr
{
	short	u_uid;
	short	u_gid;
	short	u_mask;
	short	u_error;
	short	u_cdir;
	short	u_rdir;
	short	u_pdir;    /* parent directory, gezet door namei */
	short	u_diract;  /* offset in directory, voor nieuwe entry */
	Dir	u_dirent;
} User ;

EXT Superblok sbk;
EXT User u;
EXT char sysnaam[SYSLEN];
EXT int verbose;

/* prototypes */

void zien(void);
void initproc(void);
void makfile(char *cpn, short mode, short size, char opvul);
void lnk( char *cpn, char *new);
void unlnk(char *cpn);
void makdir(char *cpn);
void curd(char *cpn, int wortel);
void fstatus(char *cpn);
void chmode(char *cpn, short mode);
void chuid(char *cpn, char uid);
void mkfs(short ninode, short nblok);

short namei(char *cp);
void SchrijfSuperBlok(void);
void SchrijfInode(int ino,Inode *ip);
void SchrijfBlok(int blkno,char *bp);
Superblok* LeesSuperBlok(void);
Inode* LeesInode(int ino,Inode *ip);
char* LeesBlok(int blkno,char *bp);
void DrukSuperBlok(Superblok *sb);
void DrukInode(Inode *ip);
void DrukBlok(char *bp, int len);
void DrukProc(void);


/* Eigen functies */

void schrijf_parent(int inodenr); 
