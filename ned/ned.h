/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
typedef struct EditBox {
	int   x,          /* вертикальное начало окна                      */
	      y,          /* горизонтальное начало окна                    */
	      l,          /* вертикальная длина окна                       */
	      h,          /* горизонтальная длина окна                     */
	      colorborder,/* цвет рамки                                    */
	      colortext;  /* цвет текста                                   */
} NEB; /* Ned Edit Box */

typedef struct {
	int  addr1;
	int  addr2;
	int  mode;
} UN;

typedef struct StringBuffer {
	long *Buf;        /* буфер со строками                             */
	int  MaxStr;      /* число строк в буфере                          */
	int  MaxLen;      /* максимальное число указателей на строки       */
	int   len;         /* а это для блочных операций и еще для кое-чего */
	void *fd;         /* дескриптор файла                              */
	long seekf;       /* текущая позиция в файле                       */
	UN *U;
	int  CurU;
	int  NumU;
	int  MemU;
	int  red:1,       /* флаг изменений в буфере                       */
	     undo:1;
} NSB; /* Ned String Buffer */

NSB    DB;                  /* удаленные  блок    Delete Block             */
NSB    CB;                  /* помеченный блок    Copy Block               */

/* Типы окон */
typedef enum { NORMWIND,TEMPWIND,HELPWIND,CBWIND,DBWIND,
	       FULLWIND,MANWIND } WTYPE;

typedef struct EditFile {
	int    wd;        /* внутренний номер окна                          */
	WTYPE  typ;
	NEB    EB;        /* текущее окно                                   */
	NSB    *AS;       /* все строки                                     */
	char   *FileName; /* имя файла                                      */
	char   *WindName;
	int    CursorX,   /* горизонтальная позиция относительно окна       */
	       CursorY;   /* вертикальная позиция относительно окна         */
	int    GorPoz,    /* горизонтальное смещение начала видимой обл.    */
	       CurStr;    /* номер текущей строки                           */
	int    RetPoz;    /* позиция возврата каретки                       */
	int    PozAF;     /* позиция автоформатирования                     */
	char   pgraph;    /*     состояние рисования рамок                  */
	int    rdonly:1,  /* 1 - только на просмотр                         */
	       block:1,   /* 1 - файл блокирован другим пользователям       */
	       copy:1,    /* 1 - состояние режима пометки блока             */
	       nocom:1,   /* 1 - запрет команд через ком. клавиши           */
	       flscr:1,   /* 1 - нужно перерисовывать все текущее окно      */
	       move:1,    /* 1 - состояние изменения положения окна         */
	       size:1,    /* 1 - состояние изменения размера окна           */
	       af1:1,     /* 1 - позиция автоф. - рвать, но не выравнивать  */
	       af2:1,     /* 1 - позиция автоф. - рвать и выравнивать       */
	       af3:1,     /* 1 - позиция автоф. - рвать и выравнивать с пер.*/
	       colour:1;  /* 1 - подсвечивать выражения                     */
} NEF; /* Ned Edit File */

/* Глобальные режимы и запреты */
typedef struct mode_ned {
	int    Insert,       /* вставка/замена                             */
	       X,Y,L,H;      /* координаты доступной на экране области     */
	char   *AltName,     /* альтер. имя для первого файла              */
	       PozTab,       /* сколько пробелов выводить при нажатии таб. */
	       ScrTab,       /* на сколько заменять табуляцию в тексте     */
	       last;         /* последний введенный символ                 */
	int    NumAltName;   /* номер файла, для которого исп. альт. имя   */
	int    StrInPage;    /* строк в странице                           */
	int    time;         /* через сколько выкл. экран при отс. работы  */
	int    FdProt;       /* дескриптор файла протокола                 */
	int    sarg;         /* дескрипторы                                */
	int    sfrom;        /*             стеков                         */
	int    sto;          /*                    в                       */
	int    sfiltr;       /*                      редакторе строки      */
	int    NoSelFile:1,  /* запрет на выбор файлов                     */
	       NoColumn:1,   /* не показывать текущую колонку              */
	       NoPlusFile:1, /* не показывать скрытые файлы                */
	       Rdonly:1,     /* все файлы только на просмотр               */
	       NoNewFile:1,  /* нельзя создавать новые файлы               */
	       RestProt:1,   /* идет восстановление из протокола           */
	       UseProt:1,    /* команды записываются в протокол            */
	       UseMan:1,     /* читать как man все файлы                   */
	       UseConf:1,    /* сохранять ли конфигурационные файлы        */
	       UseColour:1;  /* использовать ли раскраску                  */
} CMN;  /* Common Modes of Ned */

/* А это используется при пометке блоков */
typedef struct RunMarkBlock {
	int Tgrp,            /* горизонтальное смещение в начале пометки   */
	    Tcy,             /* положение курсора в начале пометки         */
	    Tcln,            /* текущая колонка в начале пометки           */
	    Tln,             /* текущая строка  в начале пометки           */
	    Bcln,            /* текущая колонка в конце  пометки           */
	    Bln;             /* текущая строка  в конце  пометки           */
} RMB;

/* Запоминание позиций */
typedef struct mempoz {
	int wd;             /* дескриптор файла с данной меткой            */
	int n;              /* номер строки с данной меткой                */
	char c;             /* символ, обозначающий метку                  */
} MP;   /* Mark Position */

/* Описание динамически меняющей размеры сроки */
typedef struct dynamic_string {
	char *s;            /* буфер для строки                            */
	int  len;           /* выделено динамической памяти под буфер      */
	int  rlen;          /* используется для хранения  длины s          */
} STR;

/* Макро */
#ifndef HELPNAME
#define HELPNAME "/usr/new/lib/ned.help" /* файл с помощью                */
#endif
#ifdef X11W
#define CNFNAME  ".Xcnfned"               /* имя файла общей настройки     */
#else
#define CNFNAME  ".cnfned"               /* имя файла общей настройки     */
#endif
#define CONFNAME ".vn%s.%s"              /* временная конфигурация        */
#define PROTNAME ".vr%s.%s"              /* файл протокола                */
#define BUFNAME  ".vb%s.%s"              /* файл для буфера               */
#define MANNAME  ".vm%s.%s.%s"
#define TMPDIR    "."
#define F_NAMEIN  "%s/.%d.in"
#define F_NAMEOUT "%s/.%d.out"
#define F_NAMEERR "%s/.%d.err"

#define X         cmn.X
#define Y         cmn.Y
#define L         cmn.L
#define H         cmn.H
#define Insert    cmn.Insert
#define Fdp       cmn.FdProt
#define POZTAB    cmn.PozTab
#define SCRTAB    cmn.ScrTab

#define BACKWARD  0    /* искать назад                                      */
#define FORWARD   1    /* искать вперед                                     */
#define REPLACE   2    /* поиск при замене с запросом                       */
#define REPLSH    3    /* поиск при полной замене                           */

#define _LBIT(n)  (1<<n)
#define U_CHANGE _LBIT(0)
#define U_DELETE _LBIT(1)
#define U_INSERT _LBIT(2)
#define U_SPEC   _LBIT(3)
#define U_BEGIN  _LBIT(4)
#define U_END    _LBIT(5)
#define U_MASK   (U_CHANGE|U_DELETE|U_INSERT|U_SPEC)

#define FOUND     1
#define FAIL      0

#define MAXX(N)      (int)((N)->EB.l-2)  /* максимум видимых символов в окне  */
#define MAXY(N)      (int)((N)->EB.h-2)  /* максимум видимых строк    в окне  */
#define G_OFFS(N)    (MAXX(N)>>1)      /*позиций на горизонтальный скроллинг*/


/* Макро для определения положения мыши относительно окна */
#define IF_LB(W,X,Y) ((W)->EB.x==X && (W)->EB.y<=Y && (W)->EB.y+MAXY(W)+2>=Y)
		                    /* правая граница окна */
#define IF_RB(W,X,Y) ((W)->EB.x+MAXX(W)+2==X && (W)->EB.y<=Y && (W)->EB.y+MAXY(W)+2>=Y)
		                   /* левая граница окна */
#define IF_TB(W,X,Y) ((W)->EB.y==Y && (W)->EB.x<=X && (W)->EB.x+MAXX(W)+2>=X)
		                   /* верхняя граница окна */
#define IF_BB(W,X,Y) ((W)->EB.y+MAXY(W)+2==Y && (W)->EB.x<=X && (W)->EB.x+MAXX(W)+2>=X)
		                   /* нижняя граница окна */
#define IF_IN(W,X,Y) (X>(W)->EB.x && X<=(W)->EB.x+MAXX(W)+1 && Y>(W)->EB.y && Y<=(W)->EB.y+MAXY(W)+1)
		                   /* попадание внутрь окна */
#define IF_BO(W,X,Y) (IF_LB(W,X,Y) || IF_RB(W,X,Y) || IF_TB(W,X,Y) || IF_BB(W,X,Y))
		                   /* попадание на любую границу окна */
#define IF_FU(W,X,Y) (IF_BO(W,X,Y) || IF_IN(W,X,Y))
				   /* любое попадание на окно */
#define IF_RL(W,X,Y) (!Y && X==l_x)
				   /* изменеие раскладки клавиатуры */
#define IF_FS(W,X,Y)    (Y==W->EB.y && X>W->EB.x+1 && X<W->EB.x+5)
				   /* окно на весь экран             */


#define GP(N)   (N)->GorPoz
#define CX(N)   (N)->CursorX
#define SP(N)   (GP(N)+CX(N))
#define CS(N)   (N)->CurStr
#define CY(N)   (N)->CursorY
#define RP(N)   (N)->RetPoz
#define BMAX(B) (B)->MaxStr
#define NMAX(N) BMAX(N->AS)

#define LINESIZE  256
#define NAMESIZE  128
#define SMALLSTR  32

#define max(a,b)      ((a>b)?a:b)
#define min(a,b)      ((a>b)?b:a)
#define nis_print(c)  ((unsigned)(c)>=' ' && (unsigned)(c)<255 && (unsigned)c!=127 && (unsigned)c!=155)
#define nis_graph(c)  ((unsigned char)c>=179 && (unsigned char)c<=218)
#ifdef X11W
#define invers(a)     (((a>>4)&0x000f)|((a<<4)&0x00f0)|(a&0xff00))
#else
#define invers(a)     (((a>>4)&0x000f)|((a<<4)&0x00f0))
#endif
#define dost(d,step)  (step-((d)%(step)))
#define dnext(d,step) (d+dost(d,step))
#define AddSTR(sd,l)  if(l>sd->len) AddMemSTR(sd,l);


#define LENTITLEVENT  3

#define MAXFILES      24   /* максимально возможное число окон с файлами   */
#define MEMPOZ        16   /* максимально возможное число запомн. позиций  */

#define MXWIND        32   /* минимальный горизонтальный размер окна       */
#define MYWIND         5   /* минимальный вертикальный   размер окна       */

#define W_OFFSX        2   /* горизонтальное смещение при посл. вводе окон */
#define W_OFFSY        1   /* вертикальное смещение при посл. вводе окон   */

#define TIMESCREEN   600   /* через сколько секунд гасить экран по умолч.  */

#define RETNOFIND    127   /* условный код завершения при отсутствии прогр.*/

#define SYMBBGR       ' '  /* символ для покрытия фона под окнами          */
#define SYMBRED       '#'  /* символ - признак изменений в файле           */

#define MAXCOLOR      48   /* максимальное количество цветов               */
#define USECOLOR      38   /* используемое в данный момент кол-во цветов   */

#define SPECMANCODE1  0x08 /* в man для выделения текста используется два  */
#define SPECMANCODE2  0x5f /* типа управляющих последовательностей - '\x8'
			      и '\x5f\x8'.                                 */
#ifndef NO_MAN_A
#define CALLMAN "man -a %s > %s" /* вызов man                      */
#else
#define CALLMAN "man  %s > %s" /* вызов man                      */
#endif

/* Сокращения для цветов, подробней в rwconf.c */
#define COLMSW      C[0]
#define COLRDMSW    C[1]
#define COLBGR      C[8]
#define COLMBGR     C[9]
#define COLMSTR     C[10]
#define COLACTIV    C[2]
#define COLPASS     C[4]
#define COLTEXT     C[6]
#define COLRDTEXT   C[7]
#define COLRDACTIV  C[3]
#define COLRDPASS   C[5]
#define COLNOPRNT   C[11]
#define COLTITLE    C[12]
#define COLFSTL     C[13]
#define COLCSTL     C[14]
#define COLEDIT     C[15]
#define COLDIR      C[16]
#define COLFILE     C[17]
#define COLMESS     C[18]
#define COLHELPMENU C[19]
#define COLMARK     C[20]
#define COLSBGR     C[21]
#define COLSSTR     C[22]
#define COLDLNK     C[23]
#define COLFLNK     C[24]
#define COLDWAIT    C[25]
#define COLMACT     C[26]
#define COLCOP      C[27]
#define COLCCP      C[28]
#define COLCTP      C[29]
#define COLCME      C[30]
#define COLFUN      C[31]
#define COLFIG      C[32]
#define COLROU      C[33]
#define COLCOM      C[34]
#define COLRUS      C[35]
#define COLQUO      C[36]
#define COLMNOACT    C[37]

/* Входные команды */
#define C_STRHOME    301  /* в начало текущей строки                       */
#define C_STREND     302  /* в конец текущей строки                        */
#define C_STRLEFT    303  /* на позицию влево                              */
#define C_STRRIGHT   304  /* на позицию вправо                             */
#define C_UP         305  /* на строку вверх                               */
#define C_DOWN       306  /* на строку вниз                                */
#define C_INS        307  /* режим вставка/замена                          */
#define C_PGUP       308  /* на экран назад  по файлу                      */
#define C_PGDN       309  /* на экран вперед по файлу                      */
#define C_BACKSP     310  /* удалить символ в предыдущей позиции           */
#define C_STRDEL     311  /* удалить символ в позиции курсора              */
#define C_RETURN     312  /* вставить строку                               */
#define C_TAB        313  /* нормальная табуляция                          */
#define C_BACKTAB    314  /* обратная табуляция                            */
#define C_HOME       315  /* стать в начало файла                          */
#define C_STOP       316  /* отказ от начатого действия, выход             */
#define C_MENU       317  /* вызов меню                                    */
#define C_MARKSTR    318  /* пометить строку                               */
#define C_SEARCH     319  /* повторить поиск вперед                        */
#define C_NEWSTR     320  /* вставить новую строку                         */
#define C_HELP       321  /* вызвать помощь                                */
#define C_QWRITE     322  /* быстрая запись (обновление текущего файла)    */
#define C_STRSTICK   323  /* склейка строк                                 */
#define C_DEFTAB     324  /* изменение числа пробелов в табуляции          */
#define C_END        325  /* стать в конец файла                           */
#define C_NEXTWIND   326  /* сделать рабочим следующее окно                */
#define C_BACKWIND   327  /* сделать рабочим предыдущее окно               */
#define C_BMARKBOX   328  /* начать пометку блока                          */
#define C_EMARKBOX   329  /* закончить пометку блока                       */
#define C_ARG        330  /* ввод аргумента                                */
#define C_COPYBOX    331  /* скопировать в текущую позицию                 */
#define C_MSWIND     332  /* изменить размер окна                          */
#define C_BACKSEAR   333  /* повторить поиск назад                         */
#define C_PUTDEL     334  /* вставить ранее удаленное                      */
#define C_DELBOX     335  /* удалить помеченный блок                       */
#define C_DEL        336  /* удалить строку                                */
#define C_DMARKBOX   337  /* закончить пометку блока и удалить его         */
#define C_SELNFILE   338  /* выбрать файл для редакт. из существующих      */
#define C_BRCOM      339  /* отказ от продолжения задания команды          */
#define C_CLOSE      340  /* уничтожить последнее окно                     */
#define C_CASCAD     341  /* расположить окна каскадом                     */
#define C_MOZAIK     342  /* расположить окна мозаикой                     */
#define C_SELWIND    343  /* вызвать меню со списком окон                  */
#define C_INPSEVDO   344  /* ввести псевдографику                          */
#define C_QNOWRITE   345  /* выход без записи                              */
#define C_EXEFILTR   346  /* запуск фильтра                                */
#define C_NEWBOX     347  /* вставить новый блок                           */
#define C_REPLACE    348  /* замена подстроки                              */
#define C_FULLSCR    349  /* сделать текущее окно на весь экран            */
#define C_POZUP      350  /* текущую строку в верхнюю часть экрана         */
#define C_POZDOWN    351  /* текущую строку в нижнюю часть экрана          */
#define C_BMACRO     352  /* начать ввод макрокоманды                      */
#define C_WMACRO     353  /* запустить макрокоманду                        */
#define C_COL        354  /* изменение цветов                              */
#define C_CTRLENT    355  /* lf                                            */
#define C_RETPOZ     356  /* выставить границу возврата каретки            */
#define C_CLRETPOZ   357  /* убрать    границу возврата каретки            */
#define C_AUTOF1     358  /* позиция автоф. - рвать, но не выравнивать     */
#define C_AUTOF2     359  /* позиция автоф. - рвать и выравнивать          */
#define C_AUTOF3     360  /* позиция автоф. - рвать и выравнивать с перен. */
#define C_CLAF       361  /* позиция автоф. - убрать                       */
#define C_BRSTR      362  /* удалить все справа от курсора                 */

#define C_MOUSE      363  /* позиционирование курсора мышкой               */

#define C_SETSINP    364  /* установить количество строк в странице        */
#define C_VIEWCB     365  /* просмотр помеченного блока                    */
#define C_VIEWDB     366  /* просмотр удаленного блока                     */
#define C_CALLSH     367  /* вызов shell                                   */
#define C_PGRAPH     368  /* вход в режим рисования пмевдографики          */
#define C_FORMAT     369  /* форматирование блока                          */
#define C_COLOUR     370  /* режим подсветки                               */
#define C_NEXTWORD   371  /* следующее слово                               */
#define C_BACKWORD   372  /* предыдущее слово                              */
#define C_UNDO       373

#define RES1         400  /* псевдокоманда                                 */

#ifdef X11W
extern unsigned int C[MAXCOLOR];/* массив для настройки цветов                  */
#else
extern unsigned char C[MAXCOLOR];
#endif

extern CMN cmn;           /*  управляющая общими режимами структура        */

/* внешние переменные с экр. библиотеки  */
extern int l_x,l_y,x_c,y_c,insert,rus_lat,cur_poz;

extern struct mouse ev;

/* Описания управляющего меню */
#define NUMELEM 4


#define IF_SW(m,e) (e.x>m->x && e.x<m->x+m->len && e.y>m->y && e.y<=m->y+m->num)
#define IF_MB(m,e) (e.y==m->y)


typedef struct menu Menu;

/* Описание элемента меню */
typedef struct item {
	char *s;      /* сообщение                                         */
	Menu *m;      /* указатель на вложенное меню                       */
	int  c;       /* возвращаемая команда                              */
	char *h;      /* подсказка                                         */
	char active;
} IT;

struct menu
{
	int  x,y,     /* координаты меню                                   */
	     len,     /* горизонтальная длина для вертикального меню  или
			 максимальная длина для сообщения в горизонтальном
			 меню                                              */
	     area,    /* область экрана под меню                           */
	     areah,   /* область экрана под подсказкой                     */
	     colcur,  /* цвет текущего сообщения                           */
	     colno,
	     num,     /* количество элементов меню                         */
	     cur;     /* текущий элемент меню                              */
	IT   **it;    /* описание элемента меню                            */
	Menu *next,*last;

};

/* Прототипы функций */
MP   *FindMemPoz(char);
MP   *FindMemPozStr(int,int,int);
STR  *CreatSTR(char *);
STR  *DupSTR(STR *);
STR  *GetMSTR(NSB *,int);
STR  *GetSTR(NSB *,int);
NEF  *IfAlreadyUse(char *,int);
STR  *InitSTR();
int  AddMemSTR(STR *,int);
int  AddMemoryAllFiles(int);
int  BreakStrInBuf(NEF *,int,int);
int  CallHelp(void);
int  CheckBoxParam(char *,int,int,int,int);
int  CheckCommandMark(int);
int  ClearEndStr(char *);
int  ClearTail(NEF *);
int  ControlMemoryBuffer(NSB *,int);
int  CopyBufferInBuffer(NSB *,int,int,NSB *,int,int,int,int,int);
int  DelPartStr(NEF *);
int  DeleteBlock(NEF *);
int  DeleteBlockInBuffer(NSB *,int,int,int,int,int);
int  DeleteStrInStr(STR *,int,int,int);
int  Dpi(void);
int  Edit(NEF *,int,char *,int,int,int,int,char,int);
int  EditStr(NEF *,int);
int  ExeCommand(char *);
int  Exec_Filtr(NEF *);
int  Execlp(char *,char *);
int  Execvp(char *,char **);
int  FindString(NEF *,char *,int);
int  GetMemBlock(NEF *,int);
int  InitEdit(char *,int);
int  InputArgument(NEF *,int);
int  InputPoz(NEF *);
int  InputPsevdoGraph(void);
int  InsertStrInBuf(NSB *,int,int);
int  MoveSizeWind(NEF *);
int  NumBegSpace(char *);
int  OutWindName(int,int,int,int);
int  PutMemBlock(NEF *,int);
int  PutMemPoz(int,int,char);
int  PutNewBlock(NEF *);
int  PutSTR(NSB *,int,STR *);
int  PutStr(NSB *,int,STR *);
int  PutStrInStr(STR *,int,STR *,int,int,int);
int  QuestBox(NEF *,int,int,int,int,int,char,int,char *,int,int);
int  ReadColorFile(char *);
int  ReadComInProt(char *);
int  ReadCommand(NEF *,int);
int  RestoreP(void);
int  RestoreS(void);
int  RunEditFile(NEF *);
int  SaveP(void);
int  SaveP(void);
int  SaveS(void);
int  ScreenOneColor(int,int,int,int);
int  ScreenPsevdoGraph(int,int,int,int);
int  SetPozAF(NEF *,int,int);
int  StickStrInBuf(NEF *,int,int);
int  StringsInPage(NEF *);
int  Strlen(char *);
int  WhatDraw(NEF *,STR *,int *,int);
int  WriteAll(int);
int  WriteEditFile(NEF *,int);
int  get_bord_gor(int,int);
int  get_bord_vert(int,int);
int  init_stk(int);
int  line_status(int);
int  menu_steck(int,int,int,int,char *);
int  put_l_s(int,char *);
char *GetCurrentWord(NEF *);
char *GetFileName(NEF *);
char *Malloc(int);
char *Realloc(void *,int);
char *Strdup(char *);
char *basen(char *);
char *execat(char *,char *,char *);
char *get_l_s(int,int);
char *get_menu(int,int);
char *get_tty(void);
char *make_bak_name(char *name);
char *norm_path(char *);
char *pathn(char *);
char *pmes(int);
void AddMemStr(int);
void AddMemStr1(int);
void Box(int,int,int,int,int,int);
void BufToBuf(NSB *,int,NSB *,int);
void CallSpecWindow(int);
void Cascade(void);
void ChangeColor(void);
void ChangeRed(NEF *,char);
void ClearEndSTR(STR *);
void CloseRestProt();
void CloseUseProt();
void CloseWind(int);
void ColourWords(NEF *,STR *,int,int);
void CorrXY(NEF *);
void DelBufFile(void);
void DelMemPoz(char);
void DelTextFile(int);
void DeleteStrInBuf(NSB *,int,int);
void Dpp(NEB *,int,int);
void DrawOpBorder(NEF *);
void EndMark(NEF *);
void ExeUsrSh();
void Free(void *);
void FreeSTR(STR *);
void GetColors(void);
void IndPoz(NEF *);
void InitEditBoxForFile(NEF *,int);
void InitEditBoxForFileB(int);
void InitMark(NEF *);
void InsPGraphInStr(char *);
void LineOut(NEF *,char *);
void Line_Out(NEF *);
void ManWords(NEF *,int,int);
void MoveMemPoz(int,int,int);
void Mozaik(void);
void OutEditBox(NEF *);
void OutEditText(NEF *);
void PaintBgr(void);
void PutFileName(NEF *);
void PutLang(void);
void PutTitleAll(NEF *,char *,int);
void PutTitleEvent(NEF *,char *);
void PutTitleEvent(NEF *,char *);
void ReadPozConf(int,char *,int,int *);
void ReadStecksConf(int);
void RepaintAllWind(void);
void Replace(NEF *);
void ResetStrBuf(NSB *);
void RestoreScreen(void);
void RotateAllWind(int);
void SavePozConf(int,int);
void SaveStecksConf(int);
void SelectNewFile();
void SelectWindName(void);
void ShowMark(NEF *);
void ShowMemPoz(NEF *);
void ShowTitleEvent(NEF *);
void WriteInProt(int,char *);
void clean_temporary(void);
void close_long_pause(void);
void close_screen();
void color_defa(void);
void def_sig_action();
void down_mes(char *,int,int);
void draw_pause_mess(int);
void emess(char *);
void init_long_pause(int,char *,int);
void make_sig_action();
void nopr(int);
void open_screen();
void pause_screen();
void pscp(NEF *,int);
void rest_poz(void);
void root_path(char *,char *);
void scp(NEF *);
void show_bord_gor(int,int,int,int,int,int);
void show_bord_vert(int,int,int,int,int,int);
void stor_poz(void);
void title(int,char *);
void unscp(NEF *);
void use_scroll(NEF *,int,int);
void wind(int,int,int,int);
int Undo(NEF *);
int InsertHistory(NSB *,int,int,int);
int BackWord(NEF *);
int NextWord(NEF *);
int ReallyQuit(void);
void ChangeTab(void);
int SelectWindMouse(int,int);


/* Отладочные макро */
#define S(X)  {char t_t[128];stor_poz();dpp(0,0);sprintf(t_t,"%s",X);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define RS(X)  {stor_poz();dpp(0,0);dps(X);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define I(X)  {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%5d",X);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define D(X)  {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%6ld",X);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define SI(X,Y) {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%14.14s %d",X,Y);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define SC(X,Y) {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%14.14s 0x%x",X,Y);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
