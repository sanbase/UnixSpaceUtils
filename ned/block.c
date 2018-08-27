/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: block.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 * Операции над блоками
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define X11
#include "ned.h"


/* Фильтр для команд, разрешенных при пометке блока */
int CheckCommandMark(int c)
{
	if(nis_print(c)  || !(c<C_BACKSP || c==C_TAB  || c==C_DELBOX ||
	   c==C_BACKTAB  || c==C_HOME    || c==C_END  || c==C_NEWBOX ||
	   c==C_MARKSTR  || c==C_DEL     || c==C_STOP || c==C_NEWSTR ||
#ifdef X11

	   c==C_MOUSE ||
#endif
	    c==C_EMARKBOX || c==C_BACKWIND || c==C_SEARCH))
	{
		 emess(pmes(144)); return(-1);
	}
	return(0);
}

RMB mb;

/* Обозначить начало пометки блока */
void InitMark(NEF *W)
{
	mb.Tgrp=GP(W);
	mb.Tcy=CY(W);
	mb.Tcln=SP(W);
	mb.Tln=CS(W);
	W->copy=1;
}

/* Обозначить конец пометки блока */
void EndMark(NEF *W)
{
	mb.Bcln=SP(W);
	mb.Bln=CS(W);
	GP(W)=mb.Tgrp;
	CY(W)=mb.Tcy;
	CX(W)=mb.Tcln-mb.Tgrp;
	CS(W)=mb.Tln;
	W->copy=0;
}

/* Запомнить блок */
int PutMemBlock(NEF *W,int oper)
{
	ResetStrBuf(oper?&CB:&DB);
	if(CopyBufferInBuffer(oper?&CB:&DB,0,0,W->AS,mb.Tln,mb.Tcln,mb.Bln,mb.Bcln,0))
		return(-1);
	return(0);
}

/* Вставить блок */
int GetMemBlock(register NEF *W,int oper)
{
	NSB *B;

	B=oper?&CB:&DB;
	if(BMAX(B)==0) return(0);
	if(Insert && mb.Tcln==mb.Bcln)
		MoveMemPoz(W->wd,min(mb.Tln,mb.Bln),abs(mb.Tln-mb.Bln)+1);
	if(CopyBufferInBuffer(W->AS,CS(W),SP(W),
			      B,0,0,BMAX(B)-1,B->len,Insert))
		return(-1);
	W->flscr=1;
	ChangeRed(W,1);
	return(0);
}

/* Удалить блок */
int DeleteBlock(NEF *W)
{
	if(mb.Tcln-mb.Bcln==0)
		MoveMemPoz(W->wd,min(mb.Tln,mb.Bln),-abs(mb.Tln-mb.Bln)-1);
	if(DeleteBlockInBuffer(W->AS,mb.Tln,mb.Tcln,mb.Bln,mb.Bcln,Insert))
		return(-1);
	W->flscr=1;
	ChangeRed(W,1);
	return(0);
}

/* Вставить новый пустой блок */
int PutNewBlock(NEF *W)
{
	int err=0;

	ResetStrBuf(&DB);
	if(InsertStrInBuf(&DB,abs(mb.Tln-mb.Bln),0)) {
		err=-1; goto END;
	}
	if(mb.Tcln==mb.Bcln)
		MoveMemPoz(W->wd,min(mb.Tln,mb.Bln),abs(mb.Tln-mb.Bln)+1);
	if(CopyBufferInBuffer(W->AS,min(mb.Tln,mb.Bln),min(mb.Tcln,mb.Bcln),
			      &DB,0,0,BMAX(&DB),DB.len=abs(mb.Tcln-mb.Bcln),1))
	{
		err=-1; goto END;
	}
	W->flscr=1;
	ChangeRed(W,1);
END:
	return(err);
}

/* Функция нижнего уровня для вставки блока из одного NSB в другой */
int CopyBufferInBuffer(NSB *T,int Tline,int Tcolumn,NSB *S,int SBline,
		       int SBcolumn,int SEline,int SEcolumn,int arg)
{
	int err=0,numlines,numcolumns,i=0;

	InsertHistory(T,0,0,U_BEGIN);
	init_long_pause(10,pmes(120),1);
	if(SEline<SBline) {
		numlines=SEline; SEline=SBline; SBline=numlines;
	}
	if(SEcolumn<SBcolumn) {
		numcolumns=SEcolumn; SEcolumn=SBcolumn; SBcolumn=numcolumns;
	}
	numlines=SEline-SBline+1;
	numcolumns=SEcolumn-SBcolumn+1;
	if((numcolumns==1 && arg) || Tline>=BMAX(T)) {
		if(InsertStrInBuf(T,numlines+(Tline>=BMAX(T)?Tline-BMAX(T):0),Tline>=BMAX(T)?BMAX(T):Tline))
		{
			err=-1; goto END;
		}
	}
	else {
		if(Tline+numlines>=BMAX(T)) {
			if(InsertStrInBuf(T,Tline+numlines-BMAX(T),BMAX(T))) {
				err=-1; goto END;
			}
		}
	}
	if(numcolumns==1) {
		for(i=0;i<numlines;i++) {
			/*BufToBuf(T,Tline+i,S,SBline+i);*/
			PutSTR(T,Tline+i,GetSTR(S,SBline+i));
			draw_b();
		}
	}
	else {
		for(i=0;i<numlines;i++) {
			STR *SS,*ST;

			ST=DupSTR(GetMSTR(T,Tline+i));
			SS=GetMSTR(S,SBline+i);
			PutStrInStr(ST,Tcolumn,SS,SBcolumn,numcolumns-1,arg);
			PutSTR(T,Tline+i,ST);
			FreeSTR(ST);
			draw_b();
		}
	}
	T->len=numcolumns-1;
END:
	close_long_pause();
	InsertHistory(T,0,0,U_END);
	return(err);
}

/* Функция нижнего уровня для вставки строки в строку */
int PutStrInStr(STR *s1,int poz1,STR *s2,int poz2,int len,int arg)
{
	int locklen;

	AddSTR(s1,(locklen=(arg?max(s1->rlen,poz1):poz1+max(0,s1->rlen-len-poz1))+len)+1);
	if(s1->rlen<poz1)
		memset(s1->s+s1->rlen,' ',poz1-s1->rlen);
	else
		if(arg) memmove(s1->s+poz1+len,s1->s+poz1,s1->rlen-poz1);
	if(s2->rlen>poz2) memcpy(s1->s+poz1,s2->s+poz2,min(len,s2->rlen-poz2));
	if(len+poz2>s2->rlen)
		memset(s1->s+poz1+len-min(len,len-s2->rlen+poz2),' ',min(len,len-s2->rlen+poz2));
	s1->s[s1->rlen=locklen]=0;
	ClearEndSTR(s1);
	return(0);
}

/* Удаление блока */
int DeleteBlockInBuffer(NSB *S,int SBline,int SBcolumn,int SEline,int SEcolumn,int arg)
{
	int numlines,numcolumns,i=0;

	InsertHistory(S,0,0,U_BEGIN);
	init_long_pause(10,pmes(121),1);
	if(SEline<SBline) {
		numlines=SEline; SEline=SBline; SBline=numlines;
	}
	if(SEcolumn<SBcolumn) {
		numcolumns=SEcolumn; SEcolumn=SBcolumn; SBcolumn=numcolumns;
	}
	numlines=SEline-SBline+1;
	numcolumns=SEcolumn-SBcolumn+1;
	if(SBline>=BMAX(S)) goto END;
	if(numcolumns==1)
		DeleteStrInBuf(S,(SBline+numlines>=BMAX(S)?BMAX(S)-SBline:numlines),SBline);
	else {
		for(i=0;i<numlines && SBline+i<BMAX(S);i++) {
			STR *SS;

			SS=GetMSTR(S,SBline+i);
			DeleteStrInStr(SS,SBcolumn,numcolumns-1,arg);
			PutSTR(S,SBline+i,SS);
			draw_b();
		}
	}
END:
	close_long_pause();
	InsertHistory(S,0,0,U_END);
	return(0);
}

/* Функция нижнего уровня для удаления подстроки из строки */
int DeleteStrInStr(STR *s,int poz,int len,int arg)
{
	if(s->s[0]) {
		if(s->rlen>poz+len) {
			if(arg) {
				memcpy(s->s+poz,s->s+poz+len,s->rlen-len-poz);
				s->rlen-=len;
			}
			else
				memset(s->s+poz,' ',len);
		}
		else
			s->rlen=poz;
		s->s[s->rlen]=0;
		ClearEndSTR(s);
	}
	return(0);
}

int FormatBlock(NEF *W)
{
	int err=0,i;
	STR *S=NULL;

	InsertHistory(W->AS,0,0,U_BEGIN);
	if(mb.Bcln-mb.Tcln) {
		err=-1; goto END;
	}
	if(!(W->af1 || W->af2 || W->af3)) {
		err=-1; goto END;
	}
	err=min(max(mb.Bln,mb.Tln),NMAX(W));
	for(i=min(mb.Bln,mb.Tln);i<err;i++) {
		if((S=GetMSTR(W->AS,i+1))!=NULL &&
		    NumBegSpace(S->s)==(RP(W)>=0?RP(W):0)) {
		        if((S=GetMSTR(W->AS,i))==NULL) continue;
			NormStr(S,NULL);
		        if(S->rlen<=W->PozAF) {
				PutSTR(W->AS,i,S); S=GetMSTR(W->AS,i);
			        if(StickStrInBuf(W,i,S->rlen+1)) {
				        err=-1; goto END;
			        }
				err--; S=NULL;
			}
		} else
			S=NULL;
		if(S==NULL && (S=GetMSTR(W->AS,i))==NULL) {
			err=-1; goto END;
		}
		NormStr(S,NULL);
		if(S->rlen>=W->PozAF) {
			int l;

		        l=NMAX(W);
		        FormatOneString(W,S,i,NULL,0);
		        if(l<NMAX(W)) err++;
		}
	}
	err=0;
END:
	W->flscr=1;
	InsertHistory(W->AS,0,0,U_END);
	return(err);
}
