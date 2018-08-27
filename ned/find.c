/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: find.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tty_codes.h>
#include "ned.h"

typedef struct find_poz {
	STR *S;         /* используется в операции замены для избежания
			   повторного чтения                            */
	int poz;        /* позиция в строке                             */
	int nstr;       /* номер строки                                 */
} FP;

static char *strstrback(char *s,char *f,char *std)
{
	for(;s!=std-1;s--) {
		int j=0;

		for(;s[j]==f[j] && f[j];j++);
		if(!f[j]) return(s);
	}
	return(NULL);
}

/* Нижний уровень поиска строки expr в буфере NSB с аргументом arg,
   принимающим значения BACKWARD,FORWARD,REPLACE и REPLSH */
static FP *FindLow(NSB *W,FP *f,char *expr,int arg)
{
	int i;
	static FP ret[1];

	if(arg!=REPLSH)
	        init_long_pause(10,pmes(113),1);
	ret->S=NULL;
	for(i=f->nstr;arg!=BACKWARD?(i<BMAX(W)):(i>=0);arg?i++:i--) {
		register char *ch;
		register STR *S;

		draw_b();
		S=GetMSTR(W,i);
		ch=(arg!=BACKWARD?S->s:S->s+S->rlen-1);
		for(;ch>=S->s && ch-S->s<=S->rlen &&
		(ch=(arg!=BACKWARD?strstr(ch,expr):strstrback(ch,expr,S->s)))!=NULL;
		arg!=BACKWARD?ch++:ch--)
		{
			if(i==f->nstr &&
			   ((arg==FORWARD  && ch-S->s<=f->poz) ||
			    (arg==BACKWARD && ch-S->s>=f->poz) ||
			    ((arg==REPLACE || arg==REPLSH)  && ch-S->s<f->poz)))
				continue;
			ret->nstr=i; ret->poz=ch-S->s; ret->S=S;
			goto END;
		}
	}
END:
	if(arg!=REPLSH)
	        close_long_pause();
	return(ret->S==NULL?NULL:ret);
}

static void DrawFound(NEF *W,FP *f1,char *expr,int arg)
{
	int i;

	if(f1->nstr>=CS(W)-CY(W) && f1->nstr<=CS(W)-CY(W)+MAXY(W))
		CY(W)+=f1->nstr-CS(W);
	else {
		if(arg==FORWARD || arg==REPLACE)
			if(CS(W)-CY(W)+f1->nstr>MAXY(W))
				CY(W)=(MAXY(W)>>1);
			else
				CY(W)+=(f1->nstr-CS(W));
		else
			if(CS(W)-f1->nstr>CY(W))
				if(CY(W)<f1->nstr)
					CY(W)=(MAXY(W)>>1);
				else
					CY(W)=f1->nstr;
			else
				CY(W)-=(CS(W)-f1->nstr);
		if(f1->nstr<CY(W)) CY(W)=f1->nstr;
	}
	CS(W)=f1->nstr;
	CX(W)=f1->poz;
	GP(W)=(CX(W)/MAXX(W))*MAXX(W);
	CX(W)-=GP(W);
	OutEditBox(W);
	dpp(W->EB.x+CX(W)+1,W->EB.y+CY(W)+1);
	for(i=0;expr[i];i++)
		if(CX(W)+i<=MAXX(W) && nis_print((unsigned char)expr[i]))
			nopr((unsigned char)expr[i]-'A');
	flush_dpi();
}

static int ReplaceLow(NEF *W,char *s1,char *s2,int nstr,int beg)
{
	STR *S1,*S2,*S=NULL;
	FP f[1],*f1;
	int i=FAIL,arg=REPLACE,t=0;

	f->nstr=beg;
	if(beg==CS(W)) {
	        S1=GetMSTR(W->AS,beg);
	        for(f->poz=SP(W);f->poz>0 && S1->s[f->poz]!=' ';f->poz--);
	        CX(W)-=(SP(W)-f->poz);
	} else
		f->poz=0;
	S1=CreatSTR(s1);
	S2=CreatSTR(s2);
	while(1) {
		if((f1=FindLow(W->AS,f,S1->s,arg))==NULL || f1->nstr-nstr>beg)
			break;
		if(S==NULL) S=InitSTR();
		AddSTR(S,f1->S->rlen+1);
		strcpy(S->s,f1->S->s);
		S->rlen=f1->S->rlen;
		i=FOUND;
		if(arg==REPLACE) {
			DrawFound(W,f1,S1->s,arg);
			switch(y_n_m(W->EB.y+CY(W)<(l_y>>1)?(l_y>>1)+2:2,
			             pmes(110),NULL,&t,pmes(101),pmes(102),
				     pmes(109),pmes(103),NULL))
			{
			case C_STOP:
				goto END;
			case C_RETURN:
				switch(t) {
				case 0:
					break;
				case 2:
					arg=REPLSH;
					scrbufout();
					init_long_pause(10,pmes(123),1);
					break;
				case 1:
					memcpy(f,f1,sizeof(FP));
					f->poz+=S1->rlen;
					continue;
				case 3:
					goto END;
				}
				break;
			}
		}
		DeleteStrInStr(S,f1->poz,S1->rlen,1);
		PutStrInStr(S,f1->poz,S2,0,S2->rlen,1);
		PutSTR(W->AS,f1->nstr,S);
		memcpy(f,f1,sizeof(FP));
		f->poz+=S2->rlen;
		ChangeRed(W,1);
	}
END:
	if(arg==REPLSH) close_long_pause();
	FreeSTR(S); FreeSTR(S1); FreeSTR(S2);
	W->flscr=1;
	return(i);
}

static char *FindStr=NULL;

/* Выполняет поиск в файле, arg=1 - искать вперед */
int FindString(NEF *W,char *expr,int arg)
{
	FP f[1],*f1;

	if(FindStr==NULL && expr==NULL) {
		emess(pmes(47)); return(0);
	} else
		if(expr!=NULL) {
			if(FindStr!=NULL) Free(FindStr);
			FindStr=Strdup(expr);
		} else
			expr=FindStr;
	PutTitleEvent(W,pmes(44));
	scrbufout();
	f->nstr=CS(W); f->poz=SP(W);
	f1=FindLow(W->AS,f,expr,arg);
	ShowTitleEvent(W);
	if(f1==NULL) {
		emess(pmes(46)); return(FAIL);
	}
	DrawFound(W,f1,expr,arg);
	W->flscr=1;
	return(FOUND);
}

/* Замена подстроки */
void Replace(NEF *W)
{
	int nb,x,y,err=0,t;
	char str1[NAMESIZE+1],str2[NAMESIZE+1];

	W->nocom=1; x=15; y=3;
	nb=get_box(x,y,51,15);
	setcolor(COLMBGR);
	title(y,pmes(24));
	wind(x,y+5,49,6);
	dpp(x+2,y+6); dps(pmes(26));
	dpp(x+2,y+8); dps(pmes(25));
	dpp(x+2,y+9); setcolor(COLEDIT); dpn(45,' ');
	*str1=0; *str2=0;
	{
		char *ch;

		if((ch=GetCurrentWord(W))!=NULL) strncpy(str1,ch,NAMESIZE);
	}
	line_status(34);
	if(cmn.sfrom<0) cmn.sfrom=init_stk(LINESIZE);
	if(cmn.sto<0) cmn.sto=init_stk(LINESIZE);
	while(1) {
		int begstr=0,nstr=0;
ED1:
		setcolor(COLEDIT);
		switch(t=Edit(NULL,0,str1,NAMESIZE,45,x+2,y+7,0,cmn.sfrom)) {
#ifdef X11
		case C_MOUSE:
			if(ev.b==3)
				goto STOP;
			else
				if(ev.x>=x+2 && ev.x<=x+47 && ev.y==y+9)
					goto TO2;
#endif
		case C_RETURN:
REPL:
			put_l_s(cmn.sfrom,str1); put_l_s(cmn.sto,str2);
			if(!*str1) {
				dpo(bl); goto ED1;
			}
			if(t==C_RETURN) {
				begstr=CS(W); nstr=max(0,NMAX(W)-CS(W));
			}
			if(strcmp(str1,str2))
				err=ReplaceLow(W,str1,str2,nstr,begstr);
			goto END;
		case C_NEWSTR:
			*str1=0; goto ED1;
		case C_HOME:
HOM:
			begstr=0; nstr=NMAX(W); goto REPL;
		case C_STOP:
STOP:
			put_l_s(cmn.sfrom,str1); put_l_s(cmn.sto,str2);
			err=1;
			goto END;
		case C_PGDN:
TO2:
			dpp(x+2,y+7); dpn(45,' '); dpp(x+2,y+7); dps(str1);
			break;
		case C_INPSEVDO:
			InsPGraphInStr(str1);
			continue;
		default:
			dpo(bl); goto ED1;
		}
ED2:
		setcolor(COLEDIT);
		switch(t=Edit(W,0,str2,NAMESIZE,45,x+2,y+9,0,cmn.sto)) {
#ifdef X11
		case C_MOUSE:
			if(ev.b==3)
				goto STOP;
			else
				if(ev.x>=x+2 && ev.x<=x+47 && ev.y==y+7)
					goto TO1;
#endif
		case C_RETURN:
			goto REPL;
		case C_NEWSTR:
			*str2=0; goto ED1;
		case C_HOME:
			goto HOM;
		case C_STOP:
			goto STOP;
		case C_PGUP:
TO1:
			dpp(x+2,y+9); dpn(45,' '); dpp(x+2,y+9);dps(str2);
			goto ED1;
		case C_INPSEVDO:
			InsPGraphInStr(str2);
			goto ED2;
		default:
			dpo(bl); goto ED2;
		}
	}
END:
	W->nocom=0;
	put_box(x,y,nb); free_box(nb);
	if(err==FAIL) emess(pmes(46));
}

