/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: box.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 * Операции по выводу на экран
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>
#include <setjmp.h>
#include <errno.h>
// #define X11
#include <tty_codes.h>
#include "ned.h"

extern NEF    *AllFiles;
extern RMB    mb;
extern int    LenAllFiles;


/* Перерисовка окна */
void OutEditBox(register NEF *W)
{
	int i,p;

	setcolor(W->EB.colorborder);
	dpp(W->EB.x,W->EB.y);
	dpo((int)'╔');
	dpn((int)(W->EB.l-1),(int)'═');
	dpo((int)'╗');
#ifdef X11
	dpp(W->EB.x+2,W->EB.y);
	dps("[■]");
#endif
	setcolor(W->EB.colorborder);
	PutFileName(W);
	if(W==AllFiles+LenAllFiles-1) PutLang();
	p=CS(W)-CY(W);
#ifndef NOBORDIND
	if(W->typ!=CBWIND && W->typ!=DBWIND) W->AS->len=0;
#endif
	for(i=p;i-p<=MAXY(W);i++) {
		int j,k;
		STR *S;

		dpp(W->EB.x,W->EB.y+(i-p+1));
		if(GP(W))
			dpo('<');
		else
			dpo('║');
		setcolor(W->EB.colortext);
		S=GetMSTR(W->AS,i);
		if(S->rlen<GP(W) || i>=NMAX(W)) {
			if(i>=NMAX(W)) {
				setcolor(W->EB.colorborder);
				dpp(W->EB.x,W->EB.y+(i-p+1));
				dpo(':');
				setcolor(W->EB.colortext);
			}
			j=0;
		}
		else {
#ifndef NOBORDIND
			if(W->typ!=CBWIND && W->typ!=DBWIND && S->rlen>W->AS->len)
				W->AS->len=S->rlen;
#endif
			for(j=0;j<=MAXX(W) && S->s[j+GP(W)];j++)
				if(nis_print((unsigned char)S->s[j+GP(W)]))
					dpo(S->s[j+GP(W)]);
				else
					nopr(S->s[j+GP(W)]);
		}
		for(k=j;k<=MAXX(W);k++) dpo(' ');
		setcolor(W->EB.colorborder);
#ifdef NOBORDIND
		if(j>MAXX(W) && j+GP(W)<S->rlen)
			dpo('>');
		else
			dpo('║');
#endif
		if(!W->rdonly)
			ColourWords(W,S,i-p,1);
		else
			if(W->typ==MANWIND || cmn.UseMan)
				ManWords(W,i,i-p);
	}
	if(W->copy) ShowMark(W);
	setcolor(W->EB.colorborder);
	dpp(W->EB.x,W->EB.y+W->EB.h);
	dpo('╚'); dpn(W->EB.l-1,'═'); dpo('╝');
	ShowTitleEvent(W);
	ShowMemPoz(W);
	if(W->EB.x+W->EB.l<X+L)
		for(i=W->EB.y+1;i<W->EB.y+W->EB.h+1;i++)
			revers(W->EB.x+W->EB.l+1,i);
	if(W->EB.y+W->EB.h<Y+H-1)
		for(i=W->EB.x+1;i<W->EB.x+W->EB.l+2;i++)
			revers(i,W->EB.y+W->EB.h+1);
	pscp(W,1);
	IndPoz(W);
}

/* Позиционирование в окне */
void Dpp(register NEB *E,int x,int y)
{
	if(x>E->l-2 || y>E->h-2) return;
	dpp(E->x+1+x,E->y+1+y);
}

/* Инициализация окна */
void InitEditBoxForFile(register NEF *W,int arg)
{
	switch(arg)
	{
	case NORMWIND:
		InitEditBoxForFileB(W-AllFiles); break;
	case TEMPWIND:
		W->EB.x=X; W->EB.y=Y+H-10; W->EB.l=L-1; W->EB.h=min(8,H);
		break;
	case FULLWIND:
		W->EB.x=X; W->EB.y=Y; W->EB.l=L; W->EB.h=H-1;
		break;
	case HELPWIND:
		W->EB.x=X+3; W->EB.y=Y+1; W->EB.l=L-6; W->EB.h=H-3;
		break;
	}
	W->EB.colortext=W->rdonly?COLRDTEXT:COLTEXT;
	W->EB.colorborder=W->rdonly?COLRDACTIV:COLACTIV;
	OutEditBox(W);
}

/* Альтернативная инициализация окна */
void InitEditBoxForFileB(int i)
{
	NEF *W=AllFiles+i;


	if(!i) {
		W->EB.x=X; W->EB.y=Y;
	}
	else {
		W->EB.x=AllFiles[i-1].EB.x+W_OFFSX;
		W->EB.y=AllFiles[i-1].EB.y+W_OFFSY;
	}
	W->EB.l=L-(W->EB.x-X);
	W->EB.h=H-(W->EB.y-Y)-1;
	if(CheckBoxParam(W->FileName,W->EB.x,W->EB.y,W->EB.l,W->EB.h)) {
		/* если не можем расположить - даем окно на весь экран */
		W->EB.x=X; W->EB.y=Y; W->EB.l=L-1; W->EB.h=H-2;
	}
}

/* Выводит координаты  в файле, а также признак редактирования */
void IndPoz(register NEF *W)
{
	char str[SMALLSTR];

	dpp(W->EB.x+1,W->EB.y+W->EB.h);
	setcolor(W->EB.colorborder);
	if(W->AS->red)
		dpo(SYMBRED);
	else
		dpo('═');
	dpp(W->EB.x+2,W->EB.y+W->EB.h);
	if(cmn.NoColumn)
		sprintf(str," %d : %d ",CS(W)/cmn.StrInPage+1,CS(W)+1);
	else
		sprintf(str," %d : %d : %d ",CS(W)/cmn.StrInPage+1,CS(W)+1,SP(W)+1);
#ifdef  NOBORDIND
	dps(str);
#else
	{
		int l;

		for(l=0;str[l];) dpo(str[l++]);
		show_bord_gor((W->EB.x+l+2),W->EB.y+MAXY(W)+2,MAXX(W)-l-2,
			       W->AS->len,GP(W)+CX(W),W->EB.colorborder);
	}
	show_bord_vert(W->EB.x+MAXX(W)+2,W->EB.y+1,MAXY(W)-1,NMAX(W),CS(W),W->EB.colorborder);
#endif
	setcolor(W->EB.colortext);
	Dpp(&W->EB,CX(W),CY(W));
}

/* Выводит обозначение непечатного символа */
void nopr(int c)
{
	int col=setcolor(-1);

	setcolor(COLNOPRNT); dpo('A'+c); setcolor(col);
}

/* Выводит название действия */
void PutTitleAll(register NEF *W,register char *str,int arg)
{
	arg++;
	stor_poz();
	dpp(X+L-(LENTITLEVENT+1)*arg,Y);
	if(str!=NULL) {
	        setcolor(COLTITLE); dpn(LENTITLEVENT,' ');
	        dpp(X+L+1-(LENTITLEVENT+1)*arg+(((LENTITLEVENT+1)-strlen(str))>>1)-1,Y);
	        dps(str);
	} else
		if(W!=NULL) {
		        setcolor(W->EB.colorborder); dpn(LENTITLEVENT,'═');
		}
	rest_poz();
}

void PutTitleEvent(register NEF *W,register char *str)
{
	PutTitleAll(W,str,0);
}

/* Проверяет, допустимы ли координаты текущего окна */
int CheckBoxParam(char *str,int x,int y,int l,int h)
{
	if(x<X || y<Y || x+l>X+L || y+h>Y+H-1 || l<MXWIND ||
	   h<MYWIND || (str && strlen(basen(str))+6>l))
		return(-1);
	return(0);
}

/* Выводит имя окна с файлом */
void PutFileName(register NEF *W)
{
	int len;
	register char *ch;

	setcolor(W->EB.colorborder);
	ch=W->WindName;
	len=Strlen(ch);
	if(len>W->EB.l-4) {
		dpp(W->EB.x-2+((W->EB.l-strlen(ch=basen(ch)))>>1),W->EB.y);
		dpo(' '); dps(" ,,,/"); dps(ch); dpo(' ');
	}
	else {
		dpp(W->EB.x+((W->EB.l-len)>>1),W->EB.y);
		if(*ch) dpo(' ');
		dps(ch);
		if(*ch) dpo(' ');
	}
}

extern char curdir[LINESIZE+1];

/* Составляет имя окна с файлом */
char *GetFileName(register NEF *W)
{
	char *ch;

	if(W->typ==TEMPWIND)
		ch=pmes(29);
	else
		if(W->typ==HELPWIND)
			ch=pmes(28);
		else
			if(W->typ==CBWIND)
				ch=pmes(149);
			else
				if(W->typ==DBWIND)
					ch=pmes(148);
				else
					if(W->wd==cmn.NumAltName)
						ch=cmn.AltName;
					else
						if(!strcmp(pathn(W->FileName),curdir))
							ch=basen(W->FileName);
						else
							ch=W->FileName;
	return(ch);
}

/* Заполняет фон под окнами */
void PaintBgr()
{
	int i;

	dpp(X,Y);
	setcolor(COLBGR);
	for(i=0;i<H;i++) {
		dpp(X,Y+i);
		dpn(!i && !Y?L:L+1,SYMBBGR);
		if(!i) PutLang();
	}
	dpp(l_x-1,l_y);
}

void PutLang()
{
	int c;

	c=setcolor(-1);
	setcolor(COLACTIV);
	dpp(l_x,0);
	switch(rus_lat) {
	case 0:
		dpo('L'); break;
	case 1:
		dpo('R'); break;
	case 2:
		dpo('U'); break;
	}
	setcolor(c);
}

/* Перерисовывает все окна */
void RepaintAllWind()
{
	int i;

	PaintBgr();
	for(i=0;i<LenAllFiles;i++) {
		CorrXY(AllFiles+i); OutEditBox(AllFiles+i);
	}
}

/* Изменяет текущие координаты курсора в окне, если они недопустимы */
void CorrXY(register NEF *W)
{
	if(CX(W)>MAXX(W)) CX(W)=MAXX(W);
	if(CY(W)>MAXY(W)) CY(W)=MAXY(W);
	if(CY(W)<0) CY(W)=0;
	if(CX(W)<0) CX(W)=0;
}

/* Следующие четыре функции выполняют действия по вводу символов и команд,
 а также гасят экран, если редактором не пользовались cmn.time секунд */

#ifndef POSIX_INPUT  /* select - не POSIX функция, поэтому не уверенности
			что она есть в любом UNIX. Поэтому оставлена
			на всякий случай старая обработка */

#ifndef X11

#define CH_TIME 1

static int nb_screen;

void close_screen()
{
	int i,j,k;

	stor_poz();
	nb_screen=get_box(0,0,l_x+1,l_y+1);
	setcolor(012);
	for(k=0;k<=4;k++) {
		for(i=0;i<=l_y;i++) {
			for(j=k;j<=l_x;j+=5) {
				dpp(j,i); dpo(' ');
			}
		}
		scrbufout();
	}
}

void open_screen()
{
	put_box(0,0,nb_screen); free_box(nb_screen);
	rest_poz();
}

void change_time()
{
	time_t t;
	char *s;
	int i;

	t=time(0); s=ctime(&t);
	dpp(l_x-30,2);
	for(i=0; s && s[i]!='\n';i++) dpo(s[i]);
	dpp(l_x-1,l_y);
	scrbufout();
}

#endif /* !X11 */

int Dpi()
{
	int c;
#ifndef X11
	int flag_screen=0;
	fd_set r_f;
	struct timeval timeout;
#endif

	scrbufout();
#ifndef X11
	while(1)
	{
	        FD_ZERO (&r_f);
	        FD_SET (0,&r_f);
		if(!flag_screen)
		        timeout.tv_sec=cmn.time;
		else
		        timeout.tv_sec=CH_TIME;
	        timeout.tv_usec=0;
	        switch(select(1,&r_f,(fd_set*)NULL,(fd_set*)NULL,&timeout))
		{
		case 0:
			if(!flag_screen) {
				flag_screen=1; close_screen();
			}
			change_time();
			continue;
		default:
			if(flag_screen) {
				flag_screen=0; open_screen();
				dpi();
				continue;
			}
			break;
		}
		break;
	}
#endif /* !X11 */
	c=dpin();
#ifdef X11
	if(!c) c=Xmouse(c);
#endif /* X11 */
	return(c);
}

#else  /* POZIX_INPUT */

#ifndef X11

static int flag_screen=0,nb_screen;
static jmp_buf one;
#define CH_TIME 1

void change_time(int);
void sigINT();

void close_screen(int sig)
{
	int i,j,k;

	flag_screen=1;
	stor_poz();
	nb_screen=get_box(0,0,l_x+1,l_y+1);
	setcolor(012);
	for(k=0;k<=4;k++) {
		for(i=0;i<=l_y;i++) {
			for(j=k;j<=l_x;j+=5) {
				dpp(j,i); dpo(' ');
			}
		}
		scrbufout();
	}
	change_time(SIGALRM);
}

void open_screen(int sig)
{
	signal(SIGINT,SIG_IGN);
	put_box(0,0,nb_screen); free_box(nb_screen);
	flag_screen=0;
	rest_poz();
	scrbufout();
	signal(SIGINT,sigINT);

}

void change_time(int sig)
{
	time_t t;
	char *s;
	int i;

	signal(SIGINT,open_screen);
	t=time(0);
	s=ctime(&t);
	dpp(50,2);
	for(i=0; s && s[i]!='\n';i++) dpo(s[i]);
	dpp(l_x-1,l_y);
	scrbufout();
	longjmp(one,1);
}

#endif /* !X11 */

int Dpi()
{
	int c;

#ifndef X11

	void (*old_trap)();

	while(1)
	{
		if(setjmp(one)) {
			signal(SIGALRM,change_time); alarm(CH_TIME);
		}
		else {
			old_trap=signal(SIGALRM,close_screen); alarm(cmn.time);
		}
#endif /* !X11 */
		c=dpi();
#ifdef X11
		if(!c) c=Xmouse(c);
#else  /* X11 */
		alarm(0);
		signal(SIGALRM,old_trap);
		if(flag_screen)
			open_screen(0);
		else
			break;
	}
#endif /* X11 */
	return(c);
}

#endif /* POSIX_INPUT */

static int cxp=-1,cyp=-1;
static fwd=-1;

/* Инициализировать обозначение курсора, вызванное командой */
void scp(register NEF *W)
{
	if(W==NULL || cxp>=0) return;
	cxp=GP(W)+CX(W);
	cyp=CS(W);
	fwd=W->wd;
	pscp(W,1);
	Dpp(&W->EB,CX(W),CY(W));
}

/* Вывести обозначение курсора, вызванное командой */
void pscp(register NEF *W,register int arg)
{
	if(W==NULL || W->wd!=fwd) return;
	if(CS(W)-CY(W)<=cyp && CS(W)-CY(W)+MAXY(W)>=cyp &&
	   GP(W)<=cxp && GP(W)+MAXX(W)>=cxp)
	{
		setcolor(W->EB.colortext);
		Dpp(&W->EB,(cxp-GP(W)),(cyp-CS(W)+CY(W)));
		if(arg)
			nopr('*'-'A');
		else   {
			if(cyp<NMAX(W)) {
				STR *S;

				S=GetMSTR(W->AS,cyp);
				if(cxp<S->rlen) {
					if(nis_print((unsigned char)S->s[cxp]))
						dpo(S->s[cxp]);
					else
						nopr(S->s[cxp]);
				}
				else
					dpo(' ');
		                if(!W->rdonly)
			                ColourWords(W,S,CY(W),0);
		                else
			                if(W->typ==MANWIND || cmn.UseMan)
				                ManWords(W,cyp,CY(W));
			}
			else
				dpo(' ');
		}
	}
}

/* Погасить обозначение курсора, вызванное командой */
void unscp(register NEF *W)
{
	if(W==NULL) return;
	pscp(W,0);
	cxp=-1; cyp=-1;
	Dpp(&W->EB,CX(W),CY(W));
}

/* Удалить i строку и вставить строку j в текущем окне, используя аппаратный
   скроллинг, если редактор и текущее окно запущены на весь экран */
void use_scroll(NEF *W,int i,int j)
{
#ifndef NO_SCROLL
	if(L>=l_x && W->EB.l==L && !W->copy)
	{
		setcolor(W->EB.colortext);
		dpp(0,W->EB.y+i+1); dpo(dl);
		dpp(0,j+W->EB.y+1); dpo(il);

		setcolor(W->EB.colortext);
		dpo(el);
	}
#endif
}

void ShowTitleEvent(register NEF *W)
{
	int c;

	if(W!=AllFiles+LenAllFiles-1) return;
	c=setcolor(-1);
	if(W->copy)
		PutTitleEvent(W,pmes(12));
	else
		if(W->size)
			PutTitleEvent(W,pmes(14));
		else
			if(W->move)
				PutTitleEvent(W,pmes(13));
			else
				PutTitleEvent(W,!Insert ||  W->pgraph?pmes(11):pmes(21));
	switch(W->pgraph) {
	case 1:
		PutTitleAll(W,pmes(125),2); break;
	case 2:
	       PutTitleAll(W,pmes(130),2); break;
	case 3:
	        {
		        char str[4];

		        sprintf(str,pmes(152),cmn.last?cmn.last:' ');
	                PutTitleAll(W,str,2); break;
	        }
	}
	if(RP(W)!=-1)
		PutTitleAll(W,pmes(126),2);
	if(W->PozAF)
	        if(W->af1)
		        PutTitleAll(W,pmes(127),1);
	        else
		        if(W->af2)
		                PutTitleAll(W,pmes(128),1);
		        else
			        if(W->af3)
		                        PutTitleAll(W,pmes(129),1);
	setcolor(c);
}

int Edit(NEF *W,int c,char *str,int size,int show,int x,int y,char k,int nsteck)
{
	int ret;

	if(cmn.RestProt) {
		if((ret=ReadComInProt(NULL))>=0 && ReadComInProt(str)>=0)
			return(ret);
		*str=0;
	}
	while(1) {
		ret=dpedit((char)c,str,size,show,x,y,(int)k);
		if(nsteck>=0)
		        if(ret==cd || ret==cu) {
			        menu_steck(nsteck,x,y-1,3,str);
			        continue;
		        } else
			        put_l_s(nsteck,str);
	        ClearEndStr(str);
	        ret=ReadCommand(W,ret);
		WriteInProt(-1,str);
		break;
	}
	return(ret);
}

void ShowMark(register NEF *W)
{
	int Xm,Ym,Lm,Hm,i,j;
	int xc,yc;

	Ym=max(min(mb.Tln,CS(W))-(CS(W)-CY(W)),0);
	Hm=max(mb.Tln,CS(W))-(CS(W)-CY(W));
	if(mb.Tcln!=GP(W)+CX(W)) {
		Xm=max(min(mb.Tcln,GP(W)+CX(W))-GP(W),0);
		Lm=max(mb.Tcln,GP(W)+CX(W))-GP(W);
	}
	else {
		Xm=0; Lm=MAXX(W)+1;
	}
	for(yc=W->EB.y+(i=Ym)+1;i<=MAXY(W) && i<=Hm;i++,yc++) {
		xc=W->EB.x+(j=Xm)+1;
		for(;j<=MAXX(W) && j<Lm;j++) put_clr(xc++,yc,COLMARK);
	}
}

void ShowMemPoz(register NEF *W)
{
	int p;
	MP *s;

	p=CS(W)-CY(W);
	for(s=FindMemPozStr(W->wd,p,MAXY(W));s!=NULL;s=FindMemPozStr(-1,p,MAXY(W)))
	{
		dpp(W->EB.x,W->EB.y+s->n-p+1);
		nopr(s->c-'A');
	}
}

static char  *pause_string=NULL;
static int yp,nbp=-1;
static void (*old_trap)(int)=NULL;

/* Следующие три функции реализуют вывод сообщения  *mess про ожидание
   конца действия, если через timeout секунд действие еще не закончилось.
   draw_pause_mess собственно рисует сообщение */
void draw_pause_mess(int sig)
{
	int c=setcolor(-1);

	nbp=get_box(0,yp,l_x+1,4);
	setcolor(COLMBGR);
	title(yp,pause_string);
	scrbufout();
	setcolor(c);
}

static int i;

/* Инициализирует сообщение про ожидание */
void init_long_pause(int y,char *mess,int timeout)
{
	sigset_t mask;

	stor_poz();
	yp=y;
	pause_string=Strdup(mess);
	old_trap=signal(SIGALRM,draw_pause_mess);
	sigemptyset(&mask); sigaddset(&mask,SIGALRM);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	i=0;
	alarm(timeout);
}

/* Эта функция вызывается для определения появления SIGALRM. Вызывается
   в "безопасных" местах (т.е. вне фунций, генерирующих системные вызовы).
   Если сигнал появился, он разблокируется и срабатывыает draw_pause_mess. */
void draw_b()
{
	/* Проверяем, не приходил ли уже SIGALRM, и стоит ли вызывать
	   sigpending - эта функция занимает сравнительно много времени */
	if(i>=0 && !((i++)%256))
	{
		sigset_t mask;

		sigpending(&mask);
		if(sigismember(&mask,SIGALRM)) {
			sigemptyset(&mask); sigaddset(&mask,SIGALRM);
	                sigprocmask(SIG_UNBLOCK,&mask,NULL); i=-1;
		}
	}
}

/* Закрывает сообщение про ожидание */
void close_long_pause()
{
	alarm(0);
	if(old_trap!=NULL) {
		/* Если действие закончилось до прихода SIGALRM,
		   разблокируем этот сигнал */
		if(i>=0) {
			sigset_t mask;

			sigemptyset(&mask); sigaddset(&mask,SIGALRM);
		        sigprocmask(SIG_UNBLOCK,&mask,NULL);
		}
		/* Возобновляем старую реакцию на SIGALRM */
	        signal(SIGALRM,old_trap); old_trap=NULL;
	}
	Free(pause_string); pause_string=NULL;
	if(nbp>=0) {
	        put_box(0,yp,nbp); free_box(nbp); nbp=-1;
	}
	rest_poz();
}

int PutEmptyLineToEnd(NEF *W)
{
	if(InsertStrInBuf(W->AS,1,CS(W)+1)) return(-1);
	if(CY(W)<MAXY(W)) {
		setcolor(W->EB.colorborder);
		dpp(W->EB.x,W->EB.y+CY(W)+1); dpo('║');
	        dpp(W->EB.x,W->EB.y+CY(W)+2); dpo('║');
	}
	else
		W->flscr=1;
	return(0);
}

void ClrScr()
{
	if(L==l_x && H==l_y && !X && !Y) {
		setcolor(03); dpp(0,0); dpo(es);
	} else
	        clear_area(X,Y,L+1,H+1);
}

struct clr color_to_clr();
clear_area(int x,int y,int l,int h)
{
	int i,j;
	union fpic cc;

	cc.p.ch=' ';
#ifdef X11
	cc.p.clr.fg=3;cc.p.clr.bg=0;
#else
	cc.p.clr=color_to_clr(3);
#endif
	for(i=x;i<x+l;i++)
		for(j=y;j<y+h;j++)
			put_f(i,j,cc);
}
