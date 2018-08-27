/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: editstr.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 * Редактирование строки
 */
#include <stdio.h>
#include <tty_codes.h>
#include <string.h>
#include "ned.h"

static int AlignStr(STR *,int);
static int FormHyphen(STR *,int *);

/* Редактирование отдельно взятой строки */
int EditStr(register NEF *W,int symbol)
{
	int err=0,c,Poz;
	STR *S=NULL;

	ChangeRed(W,1);
BEGIN:
	S=DupSTR(GetMSTR(W->AS,CS(W)));
	Poz=SP(W);
	while(1) {
		if(!symbol)
			c=ReadCommand(W,-1);
		else
			c=symbol;
		switch(c) {
		int flag;

		case C_STRRIGHT:
			Poz++;
			if(Poz>(flag=GP(W))+MAXX(W)) {
				GP(W)+=G_OFFS(W); W->flscr=1;
			}
			if(W->pgraph) {
				c=WhatDraw(W,S,&Poz,c); goto SYMB;
			}
			if(Poz<=flag+MAXX(W)) {
				CX(W)++; IndPoz(W); continue;
			}
			break;
		case C_STRLEFT:
			if(!Poz)
				continue;
			Poz--;
			if(Poz<(flag=GP(W))) {
				GP(W)-=(MAXX(W)>>1);
				if(GP(W)<0) GP(W)=0;
				W->flscr=1;
			}
			if(W->pgraph) {
				c=WhatDraw(W,S,&Poz,c); goto SYMB;
			}
			if(Poz>=flag) {
				CX(W)--; IndPoz(W); continue;
			}
			break;
		case C_BACKSP:
			if(Poz && NumBegSpace(S->s)==S->rlen && CS(W)) {
			        PutSTR(W->AS,CS(W),S);
				if(S!=NULL) FreeSTR(S);
				for(err=0,c=1;c<=CY(W) && CS(W)-c>=0;c++) {
					int t;

					S=GetMSTR(W->AS,CS(W)-c);
					t=NumBegSpace(S->s);
					if(S->rlen && t<Poz)
						err=max(err,t);
				}
				S=DupSTR(GetMSTR(W->AS,CS(W)));
				for(;err-GP(W)>MAXX(W);GP(W)+=G_OFFS(W),W->flscr=1);
				for(;err<GP(W) && GP(W)>=0;GP(W)-=G_OFFS(W),W->flscr=1);
				if(GP(W)<0) GP(W)=0;
				Poz=err; CX(W)=Poz-GP(W); err=0;
				break;
			}
			if(!Poz) {
				symbol=0; continue;
			}
			Poz--;
			if(Insert || Poz==S->rlen-1) {
				if(S->rlen-Poz>0)
					memcpy(S->s+Poz,S->s+Poz+1,(S->rlen--)-Poz);
			}
			else
				if(S->rlen-Poz>0)
				        S->s[Poz]=' ';
			if(Poz<GP(W)) {
				GP(W)-=(MAXX(W)>>1);
				if(GP(W)<0) GP(W)=0;
				W->flscr=1;
			}
			break;
		case C_STRDEL:
			if(S->rlen-Poz>0) {
				memcpy(S->s+Poz,S->s+Poz+1,(S->rlen--)-Poz);
			        break;
			}
			else {
				c=C_STRSTICK; symbol=0;
			}
			/* NO BREAK */
		default:
			if(nis_print(c) || symbol) {
SYMB:
				PutChSTR(S,Poz,c,W->pgraph?0:Insert);
				if(!W->pgraph ||
				   (!nis_graph(c) && W->pgraph!=3)) Poz++;
				else
					if(W->pgraph==3 && cmn.last!=c) {
					        cmn.last=c; ShowTitleEvent(W);
					}
				if(GP(W)+MAXX(W)<Poz) {
					GP(W)+=(MAXX(W)>>1); W->flscr=1;
				}
			        if(c==' ' && (W->af1 || W->af2  || W->af3) && S->rlen>W->PozAF+1)
			        {
				        int i;

				        if(Poz>(i=FormatOneString(W,S,CS(W),&Poz,1))) {
					        CS(W)++;
					        if(CY(W)!=MAXY(W))
						        CY(W)++;
					        else
						        use_scroll(W,0,CY(W));
					        CX(W)=Poz-i-1+(RP(W)>=0?RP(W):0);
					        GP(W)=0;
					        while(CX(W)>MAXX(W)) {
						        CX(W)-=G_OFFS(W); GP(W)+=G_OFFS(W);
					        }
				        }
				        if(W->flscr) {
					        OutEditBox(W); W->flscr=0;
				        }
				        symbol=0;
				        goto BEGIN;
			        }
			        else
				        break;
			}
			ClearEndSTR(S);
			PutSTR(W->AS,CS(W),S);
			err=c;
			goto END;
		}
		CX(W)=Poz-GP(W);
		if(W->flscr) {
			PutSTR(W->AS,CS(W),S);
			if(S!=NULL) FreeSTR(S);
			OutEditBox(W);
			S=DupSTR(GetSTR(W->AS,CS(W)));
			W->flscr=0;
		}
		else {
			setcolor(W->EB.colortext);
	                Dpp(&W->EB,Poz-GP(W)+((c==C_BACKSP || !Poz)?0:-1),CY(W));
			for(c=Poz+((c==C_BACKSP || !Poz)?0:-1);c<S->rlen && c-GP(W)<=MAXX(W);c++) {
		                if(nis_print((unsigned char)S->s[c]))
			                dpo(S->s[c]);
		                else
			                nopr(S->s[c]);
			}
			if(c-GP(W)<=MAXX(W)) dpo(' ');
#ifdef NOBORDIND
			else {
				setcolor(W->EB.colorborder);
			        if(S->s[c])
				        dpo('>');
			        else
				        dpo('║');
			}
#endif
			ColourWords(W,S,CY(W),1);
			IndPoz(W);
		}
		symbol=0;
	}
END:
	if(S!=NULL) FreeSTR(S);
	return(err);
}

void NormStr(STR *S,int *poz)
{
	char flag;
	int i;

	ClearEndSTR(S);
	for(flag=i=0;i<S->rlen;i++) {
		if(S->s[i]==' ') {
			if(flag && S->s[i+1]==' ') {
				memcpy(S->s+i,S->s+i+1,S->rlen---i); i--;
				if(poz!=NULL && i<*poz) (*poz)--;
			}
		}
		else
			flag=1;
	}
}


/* Выравнивание строки */
static int AlignStr(STR *S,int size)
{
	char flag;
	int i;

	ClearEndSTR(S);
	if(S->rlen>=size || !S->rlen) return(-1);
	flag=0;
	while(1) {
		for(i=S->rlen-1;i;i--) {
			if(S->s[i+1] && S->s[i+1]!=' ' && S->s[i]==' ') {
				int j;

				for(j=i;j>=0 && S->s[j]==' ';j--);
				if(j<0) break;
				AddSTR(S,S->rlen+2);
				memmove(S->s+i+1,S->s+i,S->rlen-i+1);
				S->rlen++;
				S->s[i]=' ';
				flag=1;
			}
			if(S->rlen>=size) goto P1;
		}
		if(!flag) break;
	}
P1:
	return(0);
}

static char glas[]="AEIOUaeiouАЕИОУ°ЎыЮЯаеиоу∙ўыюя";

/* ищет место для переноса и формирует его */
static int FormHyphen(STR *S ,int *poz)
{
	int i;
	char flag=0;

	for(i=*poz;i;i--) {
		if(!S->s[i+1] || S->s[i+1]==' ') continue;
		if(S->s[i]==' ') {
			if(flag) *poz=i;
			i=0; goto END;
		}
		else
			flag=1;
		if((strchr(glas,S->s[i]) && !strchr(glas,S->s[i+1])) ||
		   (!strchr(glas,S->s[i]) && strchr(glas,S->s[i+1]))) {
			int j;

			for(j=i-1;j>=0 && j>=i-4 && S->s[j]!=' ';j--);
			if(j<0 || j>i-4) continue;
			AddSTR(S,S->len+2);
			S->rlen++;
			memmove(S->s+i+1,S->s+i,S->rlen-i+1);
			S->s[i]='-'; *poz=i+1; i=1;
			goto END;
		}
	}
	i=0;
END:
	return(i);
}

int FormatOneString(NEF *W,STR *S,int n,int *poz,int arg)
{
	int i;

	NormStr(S,poz);
	if(W->af3) {
		i=W->PozAF;
		if(FormHyphen(S,&i) && poz!=NULL && *poz>=i) *poz+=2;
	}
	else {
		for(i=min(S->rlen-1,W->PozAF);i && S->s[i]!=' ';i--);
		if(!i || (RP(W)>=0 && i<RP(W)))
			i=min(S->rlen-1,W->PozAF);
	}
	PutSTR(W->AS,n,S);
	if(arg && S!=NULL) FreeSTR(S);
	BreakStrInBuf(W,n,i);
	if(W->af2 || W->af3) {
		S=GetMSTR(W->AS,n);
		AlignStr(S,W->PozAF+1);
		PutSTR(W->AS,n,S);
	}
	return(i);
}

