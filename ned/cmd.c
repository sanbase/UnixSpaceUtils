/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: cmd.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
#include <unistd.h>
#include <stdio.h>
#include <tty_codes.h>
#include "ned.h"

extern int LenAllFiles;


int ReadCommand(register NEF *W,int symbol)
{
	int c,d,mes=0;

	if(cmn.RestProt) {
		if((c=ReadComInProt(NULL))>=0) {
			scrbufout(); return(c);
		}
	}
	while(1) {
		if(mes) line_status(mes);
		if(symbol<0)
			c=Dpi();
		else {
			c=symbol; symbol=-1;
		}
		switch(c)
		{
#ifdef X11
		case 0:
			c=C_MOUSE; break;

#endif
		case 1:
			RestoreScreen(); continue;
		case 2:
			c=C_PGRAPH; break;
		case CtrlW:
			c=C_NEXTWORD; break;
		case CtrlD:
			c=C_BACKWORD; break;
		case CtrlE:
			c=C_UNDO; break;
		case F1:
			c=C_HELP; break;
		case F2:
			if(W) {
				if(W->size || W->move) {
					dpo(bl); continue;
				}
				if(!W->copy) {
					scp(W); PutTitleEvent(W,pmes(20));
				}
			}
			if(W) {
				if(W->copy)
					mes=line_status(143);
				else
					mes=line_status(137);
			}
			else
				if(!LenAllFiles)
					mes=line_status(138);
				else
					mes=line_status(107);

			d=Dpi();
			if(W && W->copy) {
				if(d!=F8 && d!=F10 && d!=F2) {
					dpo(bl); continue;
				}
			}
			switch(d)
			{
			case F1:
				c=C_QWRITE; break;
			case F2:
				c=C_MENU; break;
			case F3:
				c=C_BACKSEAR; break;
			case F4:
				c=C_FULLSCR; break;
			case F5:
				c=C_BMACRO; break;
			case F6:
				c=C_WMACRO; break;
			case F7:
				c=C_REPLACE; break;
			case F8:
				c=C_DEL; break;
			case F9:
				c=C_COPYBOX; break;
			case F11:
				c=C_EXEFILTR; break;
			case F12:
				c=C_QNOWRITE; break;
			case F10:
				if(W && !W->copy) unscp(W);
				ShowTitleEvent(W);
				continue;
			case cu:
				c=C_POZDOWN; break;
			case cd:
				c=C_POZUP; break;
			default:
				dpo(bl); ShowTitleEvent(W); unscp(W); continue;
			}
			ShowTitleEvent(W);
			break;
		case F3:
			c=C_SEARCH;   break;
		case F4:
			c=C_HOME;     break;
		case F5:
			c=C_SELNFILE; break;
		case F6:
			c=C_INPSEVDO; break;
		case F9:
			c=C_MARKSTR;  break;
		case F7:
			c=C_BACKWIND; break;
		case F8:
			c=C_NEWSTR;   break;
		case F10:
			c=C_STOP;     break;
		case F11:
			c=C_BACKTAB;  break;
		case F12:
			if(W) {
				if(W->nocom) {
					dpo(bl); continue;
				}
				mes=line_status(139);
				if(!W->copy) {
					PutTitleEvent(W,pmes(15)); scp(W);
				}
			}
			else {
				c=C_ARG; break;
			}
			switch(d=Dpi())  {
			case de:
				c=C_BRSTR;    break;
			case F3:
				c=C_NEXTWIND; break;
			case F7:
				c=C_SELWIND;  break;
			case F4:
				c=C_END;      break;
			case F8:
				ShowTitleEvent(W); unscp(W); dpo(bl); continue;
			case F5:
				c=C_MSWIND;   break;
			case F2:
				if(!W) {
					dpo(bl); continue;
				}
				if(W->copy) {
					dpo(bl); continue;
				}
				line_status(140);
				switch(Dpi()) {
				case F1:
					c=C_CLAF;     break;
				case F5:
					c=C_AUTOF1;   break;
				case F6:
					c=C_AUTOF2;   break;
				case F7:
					c=C_AUTOF3;   break;

				case F4:
					c=C_CLOSE;    break;
				case F3:
					c=C_RETPOZ;   break;
				case F2:
					c=C_CLRETPOZ; break;
				case F8:
					c=C_STRSTICK; break;
				case F9:
					c=C_PUTDEL;   break;
				case F10:
					ShowTitleEvent(W); unscp(W); continue;
				case F11:
					c=C_VIEWCB;   break;
				case F12:
					c=C_VIEWDB;   break;
				default:
					ShowTitleEvent(W); unscp(W); dpo(bl);
					continue;
				}
				break;
			case F9:
				c=C_CASCAD; break;
			case F6:
				c=C_MOZAIK; break;
			case F10:
				c=C_BRCOM;  ShowTitleEvent(W); break;
			default:
				if(d==F1) d=' ';
				if(nis_print(d)) {
					c=((d<<16)|C_ARG); break;
				}

				if(W->size || W->move) {
					dpo(bl); continue;
				}
				{
					char flag=0;

					if(cmn.UseProt) {
						flag=1; cmn.UseProt=0;
					}
#ifdef X11
					if(d) {
#endif
						W->copy=1;
						PutTitleEvent(W,pmes(12));
						c=ReadCommand(W,d);
						W->copy=0;
#ifdef X11
					}
					else
						c=C_MOUSE;
#endif
					if(flag) cmn.UseProt=1;
				}
				if(c==C_BRCOM) break;
				if(CheckCommandMark(c)<0) {
					ShowTitleEvent(W); unscp(W); continue;
				}
				c=((c<<16)|C_BMARKBOX);
			}
			break;
		case ta:
			c=C_TAB; break;
		case ru:
			c=C_STRHOME; break;
		case rd:
			c=C_STREND;  break;
		case cl:
			c=C_STRLEFT; break;
		case Cr:
			c=C_STRRIGHT; break;
		case cu:
			c=C_UP; break;
		case cd:
			c=C_DOWN; break;
		case ic:
			c=C_INS; break;
		case dl:
			c=C_PGUP; break;
		case il:
			c=C_PGDN; break;
		case dc:
			c=C_BACKSP; break;
		case de:
			c=C_STRDEL; break;
		case rn:
			c=C_RETURN; break;
		case lf:
			c=C_CTRLENT; break;
		default:
			if(!nis_print(c)) {
				dpo(bl); continue;
			}
		}
		break;
	}
	if(c!=C_QNOWRITE) WriteInProt(c,NULL);
	if(mes) line_status(mes);
	return(c);
}
