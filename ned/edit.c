/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: edit.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 * Основная обработка файла
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <tty_codes.h>
#include "ned.h"

//#define X11

/* Основной цикл обработки окна */
int RunEditFile(register NEF *W)
{
	int c;
	int NewPozX,err=0;

	NewPozX=SP(W);
	while(1) {
		if(W->copy)
			line_status(142);
		else
			if(W->pgraph)
				line_status(131);
			else
			        if(W->typ==TEMPWIND || W->typ==HELPWIND ||
			           W->typ==CBWIND || W->typ==DBWIND || W->typ==MANWIND)
				        line_status(42);
			        else
				        line_status(31);
READCOM:
		c=ReadCommand(W,-1);
		if(W->flscr)  /*для исправления экрана после операций
				поиска или замены*/
		{
			OutEditBox(W);
			W->flscr=0;
		}
#define  IF_OP  if(W->move || W->size || W->copy){emess(pmes(144));continue;}

INSYMB:
		if(c==C_MENU) {
			c=RunMenu();
			if(!W->copy || W->typ==DBWIND || W->typ==CBWIND)
				unscp(W);
			switch(c&0x0000ffff) {
			case 0:
				continue;
			case C_BMARKBOX:
				if(W->typ==CBWIND || W->typ==DBWIND)
					goto ERR2;
				PutTitleEvent(W,pmes(12));
				break;
			case C_RETPOZ:
			case C_AUTOF1:
			case C_AUTOF2:
			case C_AUTOF3:
				c|=0x00010000; break;
			case RES1:
				continue;
			default:
				break;
			}
		}
		if(W->copy)
		{
			if(CheckCommandMark(c))
				continue;
			switch(c)
			{
			case C_MARKSTR:
				c=C_EMARKBOX; break;
			case C_DEL:
				c=C_DELBOX; break;
			case C_NEWSTR:
				c=C_NEWBOX; break;
			case C_BACKWIND:
				c=C_FORMAT; break;
			}
		}
		switch(c)
		{
		case C_NEXTWIND:
		case C_CLOSE:
		case C_COL:
		case C_CASCAD:
		case C_MOZAIK:
		case C_SELWIND:
		case C_VIEWDB:
		case C_VIEWCB:
		case C_QNOWRITE:
			unscp(W);
		case C_BACKWIND:
		case C_HELP:
			IF_OP; goto END;
		case C_SELNFILE:
			IF_OP; goto END;
		}
		if(W->rdonly)
			switch(c&0x0000ffff)
			{
			case C_DELBOX:
			case C_NEWBOX:
				EndMark(W);
			case C_PUTDEL:
			case C_STRSTICK:
			case C_COPYBOX:
			case C_EXEFILTR:
			case C_AUTOF1:
			case C_AUTOF2:
			case C_AUTOF3:
			case C_RETPOZ:
			case C_CLRETPOZ:
			case C_CLAF:
			case C_BRSTR:
			case C_REPLACE:
				ShowTitleEvent(W);
			case C_RETURN:
			case C_STRDEL:
			case C_BACKSP:
			case C_QWRITE:
			case C_DEL:
			case C_INPSEVDO:
			case C_NEWSTR:
ERRNOWRITE:
				if(W->typ!=HELPWIND && W->typ!=TEMPWIND && W->typ!=MANWIND &&
				   W->typ!=DBWIND && W->typ!=CBWIND)
					if(W->block)
						emess(pmes(116));
					else
						emess(pmes(18));
				else
					dpo(bl);
				unscp(W);
				continue;
			}
INSYMB1:
		switch(c&0x0000ffff)
		{
		case C_STOP:
			if(W->copy) {
		case C_BRCOM:
				if(W->copy) {
					W->copy=0; W->flscr=1;
				}
				ShowTitleEvent(W);
				unscp(W);
				if(W->flscr)
					break;
				else
					continue;
			}
			else
			{
				if(W->pgraph) {
				        W->pgraph=0; PutTitleAll(W,NULL,2);
					ShowTitleEvent(W); continue;
				}
			        if(W->typ==TEMPWIND || W->typ==HELPWIND ||
			           W->typ==CBWIND || W->typ==DBWIND || W->typ==MANWIND)
					c=C_CLOSE;
				if(cmn.UseProt) CloseUseProt();
				goto END;
			}
#ifndef NO_SHELL
		case C_CALLSH:
			ExeUsrSh(); continue;
#endif
		case C_QWRITE:
			if(WriteEditFile(W,0)) {
				err=-1; goto END;
			}
			unscp(W);
			if(cmn.UseProt) CloseUseProt();
			break;
		case C_RETPOZ:
			SetPozAF(W,c&0x0000ffff,c&0x00010000); W->flscr=1;
			break;
		case C_CLRETPOZ:
			unscp(W); RP(W)=-1; W->flscr=1; PutTitleAll(W,NULL,2);
			break;
#ifdef X11
		case C_MOUSE:
#ifndef NOBORDIND
			if(ev.b==3 && W->copy) {
				c=C_STOP; goto INSYMB1;
			}
			if(IF_RB(W,ev.x,ev.y))  {
				int p;

				if((p=get_bord_vert(ev.x,ev.y))>=0) {
					if(p<CY(W))
						CY(W)=CS(W)=p;
					else
					        CS(W)=p;
				        W->flscr=1;
					break;
				}
				else
					if(p<-1) {
					        if(p==-2)
							c=C_PGUP;
					        else
						        c=C_PGDN;
						goto INSYMB1;
					}
				continue;
			}
			if(IF_BB(W,ev.x,ev.y)) {
				int p;

				if((p=get_bord_gor(ev.x,ev.y))>=0) {
					NewPozX=p; break;
				}
				else {
					if(p<-1) {
					        if(p==-2)
						        NewPozX--;
						else
							NewPozX++;
					        W->flscr=1;
					        break;
					}
				}
				continue;
			}
#endif
			if(IF_RL(W,ev.x,ev.y)) {
				rus_lat++;
				if(rus_lat==3) rus_lat=0;
				W->flscr=1;
				break;
			}
			if(IF_FS(W,ev.x,ev.y)) {
				c=C_FULLSCR; goto INSYMB1;
			}
			if(IF_BO(W,ev.x,ev.y)) {
				c=C_MSWIND; goto INSYMB1;
			}
			if(IF_IN(W,ev.x,ev.y)) {
				if(!W->copy && ev.x==x_c && ev.y==y_c) goto BEGINMARK;
				CX(W)=ev.x-W->EB.x-1;
				CS(W)=ev.y-W->EB.y-1+CS(W)-CY(W);
				CY(W)=ev.y-W->EB.y-1;
				NewPozX=SP(W);
				Dpp(&W->EB,CX(W),CY(W));
				break;
			}
			if((c=SelectWindMouse(ev.x,ev.y))>=0) {
				RotateAllWind(c); c=RES1; goto END;
			}
			continue;
#endif
		case C_PGRAPH:
			switch(W->pgraph) {
			case 0:
				c=0; break;
			case 1:
				c=2; break;
			case 2:
				c=1; break;
			case 3:
				c=2; break;
			}
	                switch(y_n_m(8,pmes(153),NULL,&c,"├─┤","╠═╣",pmes(154),pmes(103),NULL)) {
			case C_STOP:
				PutTitleAll(W,NULL,2); c=0; break;
			case C_RETURN:
				switch(c) {
				case 0:
				case 1:
				case 2:
				        W->pgraph=c+1;
				        line_status(131);
					if(c!=2)
					        c='─';
					else
						c=0;
					ShowTitleEvent(W);
					goto EDSTR;
				case 3:
					PutTitleAll(W,NULL,2);
					W->pgraph=0; c=0; break;
				}
			}
			break;
		case C_BRSTR:
			unscp(W); DelPartStr(W); break;
		case C_BMACRO:
			unscp(W);
			if(if_macro()) {
				emess(pmes(50)); continue;
			}
			else {
				char str[SMALLSTR+1];

				sprintf(str,".%d.macro",getpid());
				creat_macro(str);
			}
			line_status(49);
			goto READCOM;
		case C_WMACRO:
			{
				char str[SMALLSTR+1];

				unscp(W);
				if(if_macro()) {
					emess(pmes(50)); continue;
				}
				sprintf(str,".%d.macro",getpid());
				if(access(str,R_OK)) {
					emess(pmes(48)); continue;
				}
				macro(str);
			}
			continue;
		case C_PGUP:
			if(CS(W)<=MAXY(W)+1)
				CS(W)=CY(W)=0;
			else
			{
				CS(W)-=(MAXY(W));
				if(CS(W)-CY(W)<0) CY(W)=CS(W);
			}
			W->flscr=1;
			break;
		case C_SETSINP:
			StringsInPage(W);
			break;
		case C_PGDN:
			CS(W)+=(MAXY(W));
			W->flscr=1;
			break;
		case C_UP:
			if(!CY(W)) {
				if(CS(W))
					CS(W)--;
				else
					continue;
				use_scroll(W,MAXY(W),0); W->flscr=1;
			}
			else {
				CY(W)--; CS(W)--;
			}
			break;
		case C_BACKSEAR:
			unscp(W);
		case C_SEARCH:
			FindString(W,NULL,c==C_SEARCH?1:0); NewPozX=SP(W);
			continue;
		case C_FULLSCR:
			InitEditBoxForFile(W,FULLWIND);
			unscp(W); W->flscr=1;
			break;
		case C_NEXTWORD:
			NewPozX=NextWord(W); break;
		case C_BACKWORD:
			NewPozX=BackWord(W); break;
		case C_EXEFILTR:
			c=Exec_Filtr(W);
			switch(c) {
			case 1:
				err=RES1; goto END;
			case 0:
				unscp(W); break;
			default:
				unscp(W); continue;
			}
			break;
		case C_DOWN:
			CS(W)++;
			if(CY(W)!=MAXY(W)) {
				CY(W)++;
			}
			else {
				use_scroll(W,0,CY(W)); W->flscr=1;
			}
			break;
		case C_POZUP:
			CY(W)=0; W->flscr=1; unscp(W); break;
		case C_POZDOWN:
			CY(W)=MAXY(W);
			if(CS(W)-CY(W)<0) CY(W)=CS(W);
			W->flscr=1; unscp(W);
			break;
		case C_STRRIGHT:
			if(W->pgraph) goto EDSTR;
			NewPozX++; break;
		case C_STRLEFT:
			if(W->pgraph) goto EDSTR;
			NewPozX--; break;
		case C_STRHOME:
			NewPozX=RP(W)>=0?RP(W):0; break;
		case C_STREND:
			if(CS(W)>=NMAX(W))
				NewPozX=0;
			else {
				STR *S;

				S=GetMSTR(W->AS,CS(W));
				NewPozX=max(RP(W),S->rlen);
			}
			break;
		case C_TAB:
			NewPozX=dnext(SP(W),POZTAB); break;
		case C_BACKTAB:
			NewPozX=SP(W)-((SP(W)%POZTAB)?(SP(W)%POZTAB):POZTAB);
			break;
		case C_AUTOF1:
		case C_AUTOF2:
		case C_AUTOF3:
			SetPozAF(W,c&0x0000ffff,c&0x00010000);
			break;
		case C_CLAF:
			unscp(W);
			W->af1=0; W->af2=0; W->af3=0; W->flscr=1; W->PozAF=0;
			break;
		case C_NEWSTR:
			if(CS(W)<NMAX(W)) {
				MoveMemPoz(W->wd,CS(W),1);
				if(InsertStrInBuf(W->AS,1,CS(W)))
				{
					err=-1; goto END;
				}
				W->flscr=1; ChangeRed(W,1);
			}
			break;
		case C_DEL:
			if(CS(W)>=NMAX(W)) {
				unscp(W); continue;
			}
			InitMark(W);
			EndMark(W);
			if(PutMemBlock(W,0)) {
				err=-1; goto END;
			}
			MoveMemPoz(W->wd,CS(W),-1);
			DeleteStrInBuf(W->AS,1,CS(W));
			use_scroll(W,CY(W),MAXY(W));
			W->flscr=1;
			ChangeRed(W,1);
			unscp(W);
			break;
		case C_MSWIND:
			W->copy=0;
			MoveSizeWind(W);
			NewPozX=SP(W);
			unscp(W);
			continue;
		case C_REPLACE:
			unscp(W);
			Replace(W);
			NewPozX=SP(W);
			break;
		case C_HOME:
			CY(W)=0; CS(W)=0; W->flscr=1; break;
		case C_STRSTICK:
			if(StickStrInBuf(W,CS(W),SP(W))) {
				err=-1; goto END;
			}
			unscp(W);
			break;
		case C_RETURN:
			 if(BreakStrInBuf(W,CS(W),SP(W))) {
				 err=-1; goto END;
			 }
			 CS(W)++;
			 if(CY(W)<MAXY(W))
				 CY(W)++;
			 else
				 W->flscr=1;
			 if(RP(W)>=0)
				 NewPozX=RP(W);
			 else
				if(CS(W)<NMAX(W)) {
					 STR *S;

					 S=GetMSTR(W->AS,CS(W)-1);
					 NewPozX=NumBegSpace(S->s);
				 }
				 else
					 NewPozX=0;
			break;
		case C_END:
			if(ClearTail(W)) {
				err=-1; goto END;
			}
			CS(W)=NMAX(W)?NMAX(W):0;
			if(CS(W)<(MAXY(W)>>1))
				CY(W)=CS(W);
			else
				CY(W)=(MAXY(W)>>1);
			W->flscr=1;
			ShowTitleEvent(W);
			unscp(W);
			break;
		case C_INS:
			Insert=!Insert;
			ShowTitleEvent(W);
			continue;
		case C_ARG:
			c>>=16;
			if(!c || nis_print(c)) {
				int k;

				unscp(W);
				k=InputArgument(W,c);
				switch(k) {
				case C_STOP:
					ShowTitleEvent(W);
					Dpp(&W->EB,CX(W),CY(W));
					continue;
				case C_HOME:
					OutEditBox(W); continue;
				case C_MARKSTR:
					c=C_EMARKBOX; goto INSYMB1;
				case C_DEL:
					unscp(W); c=C_DELBOX; goto INSYMB1;
				case C_NEWSTR:
					c=C_NEWBOX; goto INSYMB1;
				case C_BACKSEAR:
				case C_SEARCH:
					unscp(W); NewPozX=SP(W); continue;
				case RES1:
					c=k; goto END;
				case C_BACKTAB:
					break;
				default:
					continue;
				}
			}
			break;
		case C_BMARKBOX:
			if(W->typ==DBWIND || W->typ==CBWIND) {
				unscp(W);
ERR2:
				W->copy=0;
				ShowTitleEvent(W);
				emess(pmes(144));
				continue;
			}
BEGINMARK:
			if(W->pgraph) {
				W->pgraph=0; ShowTitleEvent(W);
			}
			c>>=16;
			InitMark(W);
			if(!c) {
				scp(W); ShowTitleEvent(W); continue;
			}
			goto INSYMB1;
		case C_MARKSTR:
			InitMark(W);
			EndMark(W);
			if(PutMemBlock(W,1)) {
				err=-1; goto END;
			}
			continue;
		case C_EMARKBOX:
			EndMark(W);
			if(PutMemBlock(W,1)) {
				err=-1; goto END;
			}
			NewPozX=SP(W);
			W->flscr=1;
			ShowTitleEvent(W);
			unscp(W);
			break;
		case C_NEWBOX:
			if(!W->copy) {
				c=C_NEWSTR; goto INSYMB1;
			}
			EndMark(W);
			if(PutNewBlock(W)) {
				err=-1; goto END;
			}
			unscp(W);
			break;
		case C_FORMAT:
			EndMark(W);
			FormatBlock(W);
			NewPozX=SP(W);
			ShowTitleEvent(W);
			unscp(W);
			break;
		case C_DEFTAB:
			ChangeTab();
			continue;
		case C_COPYBOX:
		case C_PUTDEL:
			if(GetMemBlock(W,c==C_COPYBOX?1:0)) {
				err=-1; goto END;
			}
			unscp(W);
			break;
		case C_DELBOX:
			if(!W->copy) {
				c=C_DEL; goto INSYMB1;
			}
			EndMark(W);
			if(PutMemBlock(W,0)) {
				err=-1; goto END;
			}
			if(DeleteBlock(W)) {
				err=-1; goto END;
			}
			NewPozX=SP(W);
			unscp(W);
			break;
		case C_COLOUR:
			cmn.UseColour=!cmn.UseColour;
			W->flscr=1;
			break;
		case C_UNDO:
			Undo(W); break;
		default:
			if(W->copy || (!nis_print(c) &&
			   c!=C_BACKSP && c!=C_STRDEL && c!=C_INPSEVDO)) {
				dpo(bl); continue;
			}
			if(W->rdonly) goto ERRNOWRITE;
			if(c==C_INPSEVDO) {
				if((c=InputPsevdoGraph())==-1) {
					unscp(W); continue;
				}
				if(!c) c=-1;
				line_status(31);
			}
EDSTR:
			if((c=EditStr(W,c))==-1) {
				err=-1; goto END;
			}
			NewPozX=SP(W);
			goto INSYMB;
		}
		if(NewPozX<0) NewPozX=0;
		if(GP(W)<0) {
			W->flscr=1; GP(W)=0;
		}
		else
		{
		        for(;NewPozX-GP(W)>MAXX(W);GP(W)+=G_OFFS(W),W->flscr=1);
		        for(;NewPozX<GP(W) && GP(W)>=0;GP(W)-=G_OFFS(W),W->flscr=1);
		}
		if(GP(W)<0) GP(W)=0;
		CX(W)=NewPozX-GP(W);
		if(W->flscr || W->copy) {
			W->flscr=0; OutEditBox(W);
		}
		else
			IndPoz(W);
		if(W->pgraph) {
			if(c==C_DOWN || c==C_UP) {
			        c=WhatDraw(W,NULL,&NewPozX,c);goto EDSTR;
			}
		}
	}
END:
	return(!err?c:err);
}

int SetPozAF(NEF *W,int oper,int arg)
{
	if(arg) {
		int k;

		if((k=InputPoz(W))<0) return(-1);
		if(oper==C_RETPOZ)
			RP(W)=k;
		else
			W->PozAF=k;
	}
	else
		if(oper==C_RETPOZ)
			RP(W)=SP(W);
		else
			W->PozAF=SP(W);
	if(oper!=C_RETPOZ) {
		W->af1=0; W->af2=0; W->af3=0;
	}
	W->flscr=1;
	unscp(W);
	switch(oper) {
	case  C_AUTOF1:
		W->af1=1; break;
	case  C_AUTOF2:
		W->af2=1; break;
	case  C_AUTOF3:
		W->af3=1; break;
	}
	return(0);
}
