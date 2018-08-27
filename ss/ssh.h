/* ����� ������ */
#define X11
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
/* #include <sys/dir.h> */
/* ��������� ���������� */
#define or  ||
#define and &&
#define not !
#define NO      0
#define YES     1
#define boolean int
#ifndef NULL
#define NULL    ((char*)0)
#endif
#define D_NAME 512
/* ��������� */
#define history    512      /* ࠧ��� ���� ��� history */
#define maxcmd     256      /* ���ᨬ��쭠� ����� ������� */
#define lname      512      /* ����� ��⠢���� ����� */
#define maxalt     256      /* ���� � ��㣮� ��⠫��� */
#define maxmark    256      /* ����. �᫮ ����祭�� 䠩��� */
int maxcat;
int   addnice;              /* ��������� �ਮ��� */

struct dir2 {               /* ���� inode �������� �� ��થ� � 0 */
	char d_mark;
	char *d_name;
	char d_null;
	long size,date;
	short uid,gid,mode;
};

char  *hcat;            /* ��� ⥪�饣� ��⠫��� */
char *arpath;           /* ��� ��� � ������⥪� */
char *arname, *arbuf;   /* ��� ������⥪�, � ���ன ��室���� */
char  *altcat;          /* ��� ��㣮�� ��⠫��� */
char *altpath;          /* ��� ��� � ������⥪� � ��� */
char  *altarname;       /* ��� ������⥪� � ��� */
int   nalt;
struct dir2 *altnam;    /* ����� � ��㣮� ��⠫��� */
char *alt_names;
int   xsize,   ysize; /* ࠧ��� �࠭� */
int   x0;             /* 㣮� ⠡���� */
int   columns, width, /* ��ࠬ���� ⠡���� */
lines,   current;
long        last_mod;           /* ��� ��᫥���� ����䨪�樨 ��⠫��� */
struct dir2 *cat;       /* ᠬ ��⠫�� (+2 ��५�� � +1 �㫨) */
int         lcat;     /*   ��� ����� */
int         lind;     /* ������ �᫮ 䠩��� � ��⠫��� */
int         nfiles;   /* �᫮ ��࠭�� 䠩��� � ��⠫��� */
int        *indcat;   /* ������� 䠩��� � ��⠫.(�� ��䠢���)*/
int        *indico;   /* ������� ��࠭��� 䠩��� */
char         *d_names;  /* ����� 䠩��� � ��⠫��� */
int         maxico;             /* ����. �᫮ 䠩��� �� �࠭� */

int   only_x;                   /* �����뢠�� �� ����ᨬ� 䠩�� */

char  *Select;         /* ��ࠧ�� ��� �롮� ��� 䠩��� */
char  selsuf;                   /* ���䨪� ��� ᥫ��樨 */
int   ico_from;                 /* ������ ��砫� ����ࠦ����� ��� */

int   *marked;         /* ������� ��� 䠩��� */
int    nmarked;                 /* �᫮ ����祭��� 䠩��� */

char   y_head, y_run,           /* y-���न���� ����� �� �࠭� */
y_name, y_table;

char   *inern[10];              /* ����७��� ��६���� $0 - $9 */
/* guba, 01.07.87 */

/* ������� �������� */

extern int   errno;
//extern char *strcat(), *index(),   *getenv(),    *rindex(), *strcpy();
//extern long  time();
char        *conv(),   *lookcom(), *get_shbuf(), *copy(),   *Mkdir(),
*Rename(), *no_esc(),  *prot_name();
struct dir2 *item();

extern char *messages[];
char *Conv (unsigned value,int length,int radix,char begins);
