/* ОБЩИЕ ДАННЫЕ */
#define X11
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
/* #include <sys/dir.h> */
/* ИЗМЕНЕНИЯ СИНТАКСИСА */
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
/* ПАРАМЕТРЫ */
#define history    512      /* размер буфера для history */
#define maxcmd     256      /* максимальная длина команды */
#define lname      512      /* длина составного имени */
#define maxalt     256      /* имен в другом каталоге */
#define maxmark    256      /* макс. число помеченых файлов */
int maxcat;
int   addnice;              /* изменение приоритета */

struct dir2 {               /* поле inode заменено на маркер и 0 */
	char d_mark;
	char *d_name;
	char d_null;
	long size,date;
	short uid,gid,mode;
};

char  *hcat;            /* имя текущего каталога */
char *arpath;           /* имя пути в библиотеке */
char *arname, *arbuf;   /* имя библиотеки, в которой находимся */
char  *altcat;          /* имя другого каталога */
char *altpath;          /* имя пути в библиотеке в нем */
char  *altarname;       /* имя библиотеки в нем */
int   nalt;
struct dir2 *altnam;    /* имена в другом каталоге */
char *alt_names;
int   xsize,   ysize; /* размер экрана */
int   x0;             /* угол таблицы */
int   columns, width, /* параметры таблицы */
lines,   current;
long        last_mod;           /* дата последней модификации каталога */
struct dir2 *cat;       /* сам каталог (+2 стрелки и +1 нули) */
int         lcat;     /*   его длина */
int         lind;     /* полное число файлов в каталоге */
int         nfiles;   /* число выбранный файлов в каталоге */
int        *indcat;   /* индексы файлов в катал.(по алфавиту)*/
int        *indico;   /* индексы выбранных файлов */
char         *d_names;  /* имена файлов в каталоге */
int         maxico;             /* макс. число файлов на экране */

int   only_x;                   /* показывать ли зависимые файлы */

char  *Select;         /* образец для выбора части файлов */
char  selsuf;                   /* суффикс для селекции */
int   ico_from;                 /* индекс начала изображаемой части */

int   *marked;         /* индексы этих файлов */
int    nmarked;                 /* число помеченных файлов */

char   y_head, y_run,           /* y-координаты полей на экране */
y_name, y_table;

char   *inern[10];              /* внутренние переменные $0 - $9 */
/* guba, 01.07.87 */

/* ВНЕШНИЕ ОПИСАНИЯ */

extern int   errno;
//extern char *strcat(), *index(),   *getenv(),    *rindex(), *strcpy();
//extern long  time();
char        *conv(),   *lookcom(), *get_shbuf(), *copy(),   *Mkdir(),
*Rename(), *no_esc(),  *prot_name();
struct dir2 *item();

extern char *messages[];
char *Conv (unsigned value,int length,int radix,char begins);
