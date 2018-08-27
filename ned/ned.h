/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
typedef struct EditBox {
	int   x,          /* ���⨪��쭮� ��砫� ����                      */
	      y,          /* ��ਧ��⠫쭮� ��砫� ����                    */
	      l,          /* ���⨪��쭠� ����� ����                       */
	      h,          /* ��ਧ��⠫쭠� ����� ����                     */
	      colorborder,/* 梥� ࠬ��                                    */
	      colortext;  /* 梥� ⥪��                                   */
} NEB; /* Ned Edit Box */

typedef struct {
	int  addr1;
	int  addr2;
	int  mode;
} UN;

typedef struct StringBuffer {
	long *Buf;        /* ���� � ��ப���                             */
	int  MaxStr;      /* �᫮ ��ப � ����                          */
	int  MaxLen;      /* ���ᨬ��쭮� �᫮ 㪠��⥫�� �� ��ப�       */
	int   len;         /* � �� ��� ������ ����権 � �� ��� ���-祣� */
	void *fd;         /* ���ਯ�� 䠩��                              */
	long seekf;       /* ⥪��� ������ � 䠩��                       */
	UN *U;
	int  CurU;
	int  NumU;
	int  MemU;
	int  red:1,       /* 䫠� ��������� � ����                       */
	     undo:1;
} NSB; /* Ned String Buffer */

NSB    DB;                  /* 㤠�����  ����    Delete Block             */
NSB    CB;                  /* ����祭�� ����    Copy Block               */

/* ���� ���� */
typedef enum { NORMWIND,TEMPWIND,HELPWIND,CBWIND,DBWIND,
	       FULLWIND,MANWIND } WTYPE;

typedef struct EditFile {
	int    wd;        /* ����७��� ����� ����                          */
	WTYPE  typ;
	NEB    EB;        /* ⥪�饥 ����                                   */
	NSB    *AS;       /* �� ��ப�                                     */
	char   *FileName; /* ��� 䠩��                                      */
	char   *WindName;
	int    CursorX,   /* ��ਧ��⠫쭠� ������ �⭮�⥫쭮 ����       */
	       CursorY;   /* ���⨪��쭠� ������ �⭮�⥫쭮 ����         */
	int    GorPoz,    /* ��ਧ��⠫쭮� ᬥ饭�� ��砫� ������� ���.    */
	       CurStr;    /* ����� ⥪�饩 ��ப�                           */
	int    RetPoz;    /* ������ ������ ���⪨                       */
	int    PozAF;     /* ������ ����ଠ�஢����                     */
	char   pgraph;    /*     ���ﭨ� �ᮢ���� ࠬ��                  */
	int    rdonly:1,  /* 1 - ⮫쪮 �� ��ᬮ��                         */
	       block:1,   /* 1 - 䠩� �����஢�� ��㣨� ���짮��⥫�       */
	       copy:1,    /* 1 - ���ﭨ� ०��� ����⪨ �����             */
	       nocom:1,   /* 1 - ����� ������ �१ ���. ������           */
	       flscr:1,   /* 1 - �㦭� ����ᮢ뢠�� �� ⥪�饥 ����      */
	       move:1,    /* 1 - ���ﭨ� ��������� ��������� ����         */
	       size:1,    /* 1 - ���ﭨ� ��������� ࠧ��� ����           */
	       af1:1,     /* 1 - ������ ����. - ࢠ��, �� �� ��ࠢ������  */
	       af2:1,     /* 1 - ������ ����. - ࢠ�� � ��ࠢ������       */
	       af3:1,     /* 1 - ������ ����. - ࢠ�� � ��ࠢ������ � ���.*/
	       colour:1;  /* 1 - ���ᢥ稢��� ��ࠦ����                     */
} NEF; /* Ned Edit File */

/* �������� ०��� � ������ */
typedef struct mode_ned {
	int    Insert,       /* ��⠢��/������                             */
	       X,Y,L,H;      /* ���न���� ����㯭�� �� �࠭� ������     */
	char   *AltName,     /* �����. ��� ��� ��ࢮ�� 䠩��              */
	       PozTab,       /* ᪮�쪮 �஡���� �뢮���� �� ����⨨ ⠡. */
	       ScrTab,       /* �� ᪮�쪮 �������� ⠡���� � ⥪��     */
	       last;         /* ��᫥���� �������� ᨬ���                 */
	int    NumAltName;   /* ����� 䠩��, ��� ���ண� ��. ����. ���   */
	int    StrInPage;    /* ��ப � ��࠭��                           */
	int    time;         /* �१ ᪮�쪮 �몫. �࠭ �� ���. ࠡ���  */
	int    FdProt;       /* ���ਯ�� 䠩�� ��⮪���                 */
	int    sarg;         /* ���ਯ���                                */
	int    sfrom;        /*             �⥪��                         */
	int    sto;          /*                    �                       */
	int    sfiltr;       /*                      ।���� ��ப�      */
	int    NoSelFile:1,  /* ����� �� �롮� 䠩���                     */
	       NoColumn:1,   /* �� �����뢠�� ⥪���� �������              */
	       NoPlusFile:1, /* �� �����뢠�� ����� 䠩��                */
	       Rdonly:1,     /* �� 䠩�� ⮫쪮 �� ��ᬮ��               */
	       NoNewFile:1,  /* ����� ᮧ������ ���� 䠩��               */
	       RestProt:1,   /* ���� ����⠭������� �� ��⮪���           */
	       UseProt:1,    /* ������� �����뢠���� � ��⮪��            */
	       UseMan:1,     /* ���� ��� man �� 䠩��                   */
	       UseConf:1,    /* ��࠭��� �� ���䨣��樮��� 䠩��        */
	       UseColour:1;  /* �ᯮ�짮���� �� �����                  */
} CMN;  /* Common Modes of Ned */

/* � �� �ᯮ������ �� ����⪥ ������ */
typedef struct RunMarkBlock {
	int Tgrp,            /* ��ਧ��⠫쭮� ᬥ饭�� � ��砫� ����⪨   */
	    Tcy,             /* ��������� ����� � ��砫� ����⪨         */
	    Tcln,            /* ⥪��� ������� � ��砫� ����⪨           */
	    Tln,             /* ⥪��� ��ப�  � ��砫� ����⪨           */
	    Bcln,            /* ⥪��� ������� � ����  ����⪨           */
	    Bln;             /* ⥪��� ��ப�  � ����  ����⪨           */
} RMB;

/* ����������� ����権 */
typedef struct mempoz {
	int wd;             /* ���ਯ�� 䠩�� � ������ ��⪮�            */
	int n;              /* ����� ��ப� � ������ ��⪮�                */
	char c;             /* ᨬ���, ��������騩 ����                  */
} MP;   /* Mark Position */

/* ���ᠭ�� �������᪨ �����饩 ࠧ���� �ப� */
typedef struct dynamic_string {
	char *s;            /* ���� ��� ��ப�                            */
	int  len;           /* �뤥���� �������᪮� ����� ��� ����      */
	int  rlen;          /* �ᯮ������ ��� �࠭����  ����� s          */
} STR;

/* ���� */
#ifndef HELPNAME
#define HELPNAME "/usr/new/lib/ned.help" /* 䠩� � �������                */
#endif
#ifdef X11W
#define CNFNAME  ".Xcnfned"               /* ��� 䠩�� ��饩 ����ன��     */
#else
#define CNFNAME  ".cnfned"               /* ��� 䠩�� ��饩 ����ன��     */
#endif
#define CONFNAME ".vn%s.%s"              /* �६����� ���䨣����        */
#define PROTNAME ".vr%s.%s"              /* 䠩� ��⮪���                */
#define BUFNAME  ".vb%s.%s"              /* 䠩� ��� ����               */
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

#define BACKWARD  0    /* �᪠�� �����                                      */
#define FORWARD   1    /* �᪠�� ���।                                     */
#define REPLACE   2    /* ���� �� ������ � ����ᮬ                       */
#define REPLSH    3    /* ���� �� ������ ������                           */

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

#define MAXX(N)      (int)((N)->EB.l-2)  /* ���ᨬ� ������� ᨬ����� � ����  */
#define MAXY(N)      (int)((N)->EB.h-2)  /* ���ᨬ� ������� ��ப    � ����  */
#define G_OFFS(N)    (MAXX(N)>>1)      /*����権 �� ��ਧ��⠫�� �஫����*/


/* ���� ��� ��।������ ��������� ��� �⭮�⥫쭮 ���� */
#define IF_LB(W,X,Y) ((W)->EB.x==X && (W)->EB.y<=Y && (W)->EB.y+MAXY(W)+2>=Y)
		                    /* �ࠢ�� �࠭�� ���� */
#define IF_RB(W,X,Y) ((W)->EB.x+MAXX(W)+2==X && (W)->EB.y<=Y && (W)->EB.y+MAXY(W)+2>=Y)
		                   /* ����� �࠭�� ���� */
#define IF_TB(W,X,Y) ((W)->EB.y==Y && (W)->EB.x<=X && (W)->EB.x+MAXX(W)+2>=X)
		                   /* ������ �࠭�� ���� */
#define IF_BB(W,X,Y) ((W)->EB.y+MAXY(W)+2==Y && (W)->EB.x<=X && (W)->EB.x+MAXX(W)+2>=X)
		                   /* ������ �࠭�� ���� */
#define IF_IN(W,X,Y) (X>(W)->EB.x && X<=(W)->EB.x+MAXX(W)+1 && Y>(W)->EB.y && Y<=(W)->EB.y+MAXY(W)+1)
		                   /* ��������� ������ ���� */
#define IF_BO(W,X,Y) (IF_LB(W,X,Y) || IF_RB(W,X,Y) || IF_TB(W,X,Y) || IF_BB(W,X,Y))
		                   /* ��������� �� ���� �࠭��� ���� */
#define IF_FU(W,X,Y) (IF_BO(W,X,Y) || IF_IN(W,X,Y))
				   /* �� ��������� �� ���� */
#define IF_RL(W,X,Y) (!Y && X==l_x)
				   /* �������� �᪫���� ���������� */
#define IF_FS(W,X,Y)    (Y==W->EB.y && X>W->EB.x+1 && X<W->EB.x+5)
				   /* ���� �� ���� �࠭             */


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

#define MAXFILES      24   /* ���ᨬ��쭮 ��������� �᫮ ���� � 䠩����   */
#define MEMPOZ        16   /* ���ᨬ��쭮 ��������� �᫮ ������. ����権  */

#define MXWIND        32   /* ��������� ��ਧ��⠫�� ࠧ��� ����       */
#define MYWIND         5   /* ��������� ���⨪����   ࠧ��� ����       */

#define W_OFFSX        2   /* ��ਧ��⠫쭮� ᬥ饭�� �� ���. ����� ���� */
#define W_OFFSY        1   /* ���⨪��쭮� ᬥ饭�� �� ���. ����� ����   */

#define TIMESCREEN   600   /* �१ ᪮�쪮 ᥪ㭤 ����� �࠭ �� 㬮��.  */

#define RETNOFIND    127   /* �᫮��� ��� �����襭�� �� ������⢨� �ண�.*/

#define SYMBBGR       ' '  /* ᨬ��� ��� ������� 䮭� ��� ������          */
#define SYMBRED       '#'  /* ᨬ��� - �ਧ��� ��������� � 䠩��           */

#define MAXCOLOR      48   /* ���ᨬ��쭮� ������⢮ 梥⮢               */
#define USECOLOR      38   /* �ᯮ��㥬�� � ����� ������ ���-�� 梥⮢   */

#define SPECMANCODE1  0x08 /* � man ��� �뤥����� ⥪�� �ᯮ������ ���  */
#define SPECMANCODE2  0x5f /* ⨯� �ࠢ����� ��᫥����⥫쭮�⥩ - '\x8'
			      � '\x5f\x8'.                                 */
#ifndef NO_MAN_A
#define CALLMAN "man -a %s > %s" /* �맮� man                      */
#else
#define CALLMAN "man  %s > %s" /* �맮� man                      */
#endif

/* ����饭�� ��� 梥⮢, ���஡��� � rwconf.c */
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

/* �室�� ������� */
#define C_STRHOME    301  /* � ��砫� ⥪�饩 ��ப�                       */
#define C_STREND     302  /* � ����� ⥪�饩 ��ப�                        */
#define C_STRLEFT    303  /* �� ������ �����                              */
#define C_STRRIGHT   304  /* �� ������ ��ࠢ�                             */
#define C_UP         305  /* �� ��ப� �����                               */
#define C_DOWN       306  /* �� ��ப� ����                                */
#define C_INS        307  /* ०�� ��⠢��/������                          */
#define C_PGUP       308  /* �� �࠭ �����  �� 䠩��                      */
#define C_PGDN       309  /* �� �࠭ ���। �� 䠩��                      */
#define C_BACKSP     310  /* 㤠���� ᨬ��� � �।��饩 ����樨           */
#define C_STRDEL     311  /* 㤠���� ᨬ��� � ����樨 �����              */
#define C_RETURN     312  /* ��⠢��� ��ப�                               */
#define C_TAB        313  /* ��ଠ�쭠� ⠡����                          */
#define C_BACKTAB    314  /* ���⭠� ⠡����                            */
#define C_HOME       315  /* ���� � ��砫� 䠩��                          */
#define C_STOP       316  /* �⪠� �� ���⮣� ����⢨�, ��室             */
#define C_MENU       317  /* �맮� ����                                    */
#define C_MARKSTR    318  /* ������� ��ப�                               */
#define C_SEARCH     319  /* ������� ���� ���।                        */
#define C_NEWSTR     320  /* ��⠢��� ����� ��ப�                         */
#define C_HELP       321  /* �맢��� ������                                */
#define C_QWRITE     322  /* ������ ������ (���������� ⥪�饣� 䠩��)    */
#define C_STRSTICK   323  /* ᪫���� ��ப                                 */
#define C_DEFTAB     324  /* ��������� �᫠ �஡���� � ⠡��樨          */
#define C_END        325  /* ���� � ����� 䠩��                           */
#define C_NEXTWIND   326  /* ᤥ���� ࠡ�稬 ᫥���饥 ����                */
#define C_BACKWIND   327  /* ᤥ���� ࠡ�稬 �।��饥 ����               */
#define C_BMARKBOX   328  /* ����� ������ �����                          */
#define C_EMARKBOX   329  /* �������� ������ �����                       */
#define C_ARG        330  /* ���� ��㬥��                                */
#define C_COPYBOX    331  /* ᪮��஢��� � ⥪���� ������                 */
#define C_MSWIND     332  /* �������� ࠧ��� ����                          */
#define C_BACKSEAR   333  /* ������� ���� �����                         */
#define C_PUTDEL     334  /* ��⠢��� ࠭�� 㤠������                      */
#define C_DELBOX     335  /* 㤠���� ����祭�� ����                       */
#define C_DEL        336  /* 㤠���� ��ப�                                */
#define C_DMARKBOX   337  /* �������� ������ ����� � 㤠���� ���         */
#define C_SELNFILE   338  /* ����� 䠩� ��� ।���. �� ���������      */
#define C_BRCOM      339  /* �⪠� �� �த������� ������� �������          */
#define C_CLOSE      340  /* 㭨�⮦��� ��᫥���� ����                     */
#define C_CASCAD     341  /* �ᯮ������ ���� ��᪠���                     */
#define C_MOZAIK     342  /* �ᯮ������ ���� ��������                     */
#define C_SELWIND    343  /* �맢��� ���� � ᯨ᪮� ����                  */
#define C_INPSEVDO   344  /* ����� �ᥢ����䨪�                          */
#define C_QNOWRITE   345  /* ��室 ��� �����                              */
#define C_EXEFILTR   346  /* ����� 䨫���                                */
#define C_NEWBOX     347  /* ��⠢��� ���� ����                           */
#define C_REPLACE    348  /* ������ �����ப�                              */
#define C_FULLSCR    349  /* ᤥ���� ⥪�饥 ���� �� ���� �࠭            */
#define C_POZUP      350  /* ⥪���� ��ப� � ������ ���� �࠭�         */
#define C_POZDOWN    351  /* ⥪���� ��ப� � ������ ���� �࠭�          */
#define C_BMACRO     352  /* ����� ���� ���ப������                      */
#define C_WMACRO     353  /* �������� ���ப������                        */
#define C_COL        354  /* ��������� 梥⮢                              */
#define C_CTRLENT    355  /* lf                                            */
#define C_RETPOZ     356  /* ���⠢��� �࠭��� ������ ���⪨            */
#define C_CLRETPOZ   357  /* ����    �࠭��� ������ ���⪨            */
#define C_AUTOF1     358  /* ������ ����. - ࢠ��, �� �� ��ࠢ������     */
#define C_AUTOF2     359  /* ������ ����. - ࢠ�� � ��ࠢ������          */
#define C_AUTOF3     360  /* ������ ����. - ࢠ�� � ��ࠢ������ � ��७. */
#define C_CLAF       361  /* ������ ����. - ����                       */
#define C_BRSTR      362  /* 㤠���� �� �ࠢ� �� �����                 */

#define C_MOUSE      363  /* ����樮��஢���� ����� ��誮�               */

#define C_SETSINP    364  /* ��⠭����� ������⢮ ��ப � ��࠭��        */
#define C_VIEWCB     365  /* ��ᬮ�� ����祭���� �����                    */
#define C_VIEWDB     366  /* ��ᬮ�� 㤠������� �����                     */
#define C_CALLSH     367  /* �맮� shell                                   */
#define C_PGRAPH     368  /* �室 � ०�� �ᮢ���� ��������䨪�          */
#define C_FORMAT     369  /* �ଠ�஢���� �����                          */
#define C_COLOUR     370  /* ०�� ���ᢥ⪨                               */
#define C_NEXTWORD   371  /* ᫥���饥 ᫮��                               */
#define C_BACKWORD   372  /* �।��饥 ᫮��                              */
#define C_UNDO       373

#define RES1         400  /* �ᥢ���������                                 */

#ifdef X11W
extern unsigned int C[MAXCOLOR];/* ���ᨢ ��� ����ன�� 梥⮢                  */
#else
extern unsigned char C[MAXCOLOR];
#endif

extern CMN cmn;           /*  �ࠢ����� ��騬� ०����� �������        */

/* ���譨� ��६���� � ��. ������⥪�  */
extern int l_x,l_y,x_c,y_c,insert,rus_lat,cur_poz;

extern struct mouse ev;

/* ���ᠭ�� �ࠢ���饣� ���� */
#define NUMELEM 4


#define IF_SW(m,e) (e.x>m->x && e.x<m->x+m->len && e.y>m->y && e.y<=m->y+m->num)
#define IF_MB(m,e) (e.y==m->y)


typedef struct menu Menu;

/* ���ᠭ�� ����� ���� */
typedef struct item {
	char *s;      /* ᮮ�饭��                                         */
	Menu *m;      /* 㪠��⥫� �� ��������� ����                       */
	int  c;       /* �����頥��� �������                              */
	char *h;      /* ���᪠���                                         */
	char active;
} IT;

struct menu
{
	int  x,y,     /* ���न���� ����                                   */
	     len,     /* ��ਧ��⠫쭠� ����� ��� ���⨪��쭮�� ����  ���
			 ���ᨬ��쭠� ����� ��� ᮮ�饭�� � ��ਧ��⠫쭮�
			 ����                                              */
	     area,    /* ������� �࠭� ��� ����                           */
	     areah,   /* ������� �࠭� ��� ���᪠����                     */
	     colcur,  /* 梥� ⥪�饣� ᮮ�饭��                           */
	     colno,
	     num,     /* ������⢮ ����⮢ ����                         */
	     cur;     /* ⥪�騩 ����� ����                              */
	IT   **it;    /* ���ᠭ�� ����� ����                            */
	Menu *next,*last;

};

/* ���⨯� �㭪権 */
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


/* �⫠���� ���� */
#define S(X)  {char t_t[128];stor_poz();dpp(0,0);sprintf(t_t,"%s",X);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define RS(X)  {stor_poz();dpp(0,0);dps(X);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define I(X)  {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%5d",X);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define D(X)  {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%6ld",X);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define SI(X,Y) {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%14.14s %d",X,Y);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
#define SC(X,Y) {char t_t[30];stor_poz();dpp(0,0);sprintf(t_t,"%14.14s 0x%x",X,Y);dps(t_t);dpi();dpp(0,0);dpn(20,' ');rest_poz();}
