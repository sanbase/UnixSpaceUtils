/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: ned.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <tty_codes.h>
#include "ned.h"

static int Welcome(void);

NEF    *AllFiles;
int    LenAllFiles=0;
CMN    cmn;
char   curdir[LINESIZE+1];

int main(int ac,char **av)
{
	int i,j=0;
	char *buf=NULL;

	dpbeg("ned");

	errno=0;
	X=0;
	Y=0;
	L=l_x;
	H=l_y;
	Insert=1;
	cmn.NoColumn=0;
	cmn.NoSelFile=0;
	cmn.NoPlusFile=0;
	cmn.NoNewFile=0;
	cmn.UseMan=0;
	cmn.Rdonly=0;
	cmn.time=TIMESCREEN;
	cmn.RestProt=0;
	cmn.UseProt=1;
	cmn.UseColour=1;
	Fdp=0;
	cmn.NumAltName=-1;
	cmn.StrInPage=66;
	POZTAB=8;
	SCRTAB=8;
	bzero(&CB,sizeof(NSB));
	bzero(&DB,sizeof(NSB));
	cmn.sarg=-1;
	cmn.sfiltr=-1;
	cmn.sfrom=-1;
	cmn.sto=-1;
	make_sig_action();
	GetColors();
	getcwd(curdir,LINESIZE);
	errno=0;
	PaintBgr(); line_status(32);
	if(ac>1) {
		for(i=1;i<ac;i++)
			if(av[i][0]=='-') {
				switch(av[i][1]) {
				/* Команды, совместимые с vedом */
				case 'c':
					cmn.UseColour=0;
				case 'L':
					rus_lat=0; continue;
				case 'R':
					rus_lat=1; continue;
				case 'U':
					rus_lat=2; continue;
				case 'm':
					cmn.Rdonly=1; continue;
				case 'X':
					Y=12;
					H=l_y-Y;
					L=atoi(av[i]+2);
					if(L>l_x)
						L=l_x;
					else
						if(L<MXWIND) L=MXWIND;
					continue;
				case 'S':
					{
						int fd;

						if((fd=open(av[i]+2,O_RDONLY))<0) {
							errno=0; continue;
						}
						if((buf=Malloc(sizeof(union fpic)*(l_x+1)*(l_y+1)))==NULL)
							continue;
						read(fd,buf,sizeof(union fpic)*(l_x+1)*(l_y+1));
						close(fd);
						put_scrold(buf);
						cp_screenold();
						zero_scrold();
					}
					continue;
				case 'C':
					cmn.NoColumn=1;  /* не показывать номер колонки */
					continue;
				case 'P':
					cmn.NoSelFile=1; /* запрет выбирать файлы       */
					continue;
				case 'N':
					cmn.NoNewFile=1;  /* запрет создавать файлы     */
					continue;
				case 'O':
					cmn.NoPlusFile=1; /* запрет на полный просмотр каталога */
					continue;
				case 'T':
					cmn.time=(int)atoi(&av[i][2]); /* через сколько гасить экран */
					if(!cmn.time) cmn.time=TIMESCREEN;
					continue;
				case 'W':
					cmn.UseProt=0; /* отказаться от протокола */
					continue;
				case 'F':
					cmn.AltName=Strdup(av[i]+2); continue;
				case 'M': /* читать как man */
					cmn.UseMan=1; continue;
				case 'K': /* не записывать конфигурацию при выходе */
					cmn.UseConf=1; continue;
				case '\0':
					cmn.RestProt=1;
				default:
					continue;
				}
			}
		for(j=0,i=1;i<ac;i++)
			if(av[i][0]!='-') {
				if(InitEdit(norm_path(av[i]),NORMWIND)==-2) break;
				j=1;
			}
	}
#ifdef SHARED
	RestoreScreen();
#endif
	if(cmn.RestProt)
		RestoreP();
	else {
		if(ac==1 || !j) {
			PutTitleEvent(NULL,NULL);
			RestoreS();
		}
		if(cmn.UseProt) SaveP();
	}

	while(1) {
		if(LenAllFiles>0) {
			switch(RunEditFile(AllFiles+LenAllFiles-1)) {
			case C_NEXTWIND:
				RotateAllWind(-1); break;
			case C_BACKWIND:
				unscp(AllFiles+LenAllFiles-1);
				RotateAllWind(0);  break;
			case C_SELNFILE:
				SelectNewFile(); break;
			case C_CLOSE:
				CloseWind(LenAllFiles-1); break;
			case C_STOP:
				if(WriteAll(1)) continue;
			case C_QNOWRITE:
				if(ReallyQuit()) {
				        clean_temporary(); SaveS(); goto END;
				}
				break;
			case C_CASCAD:
				Cascade(); break;
			case C_MOZAIK:
				Mozaik(); break;
			case C_SELWIND:
				SelectWindName(); break;
			case RES1:
				continue;
			case C_HELP:
				CallSpecWindow(HELPWIND); continue;
			case C_COL:
				ChangeColor(); continue;
			case C_VIEWDB:
				CallSpecWindow(DBWIND); continue;
			case C_VIEWCB:
				CallSpecWindow(CBWIND); continue;
			default:
				dpo(bl); continue;
			}
		}
		else {
			int  c;

			line_status(32);
			dpp(l_x-1,l_y);
			if(!j) {
				c=ReadCommand(NULL,-1); j=1;
			}
			else
			        c=ReadCommand(NULL,-1);
SW:
			switch(c)
			{
			case C_ARG:
				InputArgument(NULL,0); continue;
#ifndef NO_SHELL
			case C_CALLSH:
				ExeUsrSh(); continue;
#endif
			case C_SELNFILE:
				SelectNewFile(); break;
			case C_STOP:
			case C_QNOWRITE:
				SaveS(); goto END;
			case C_HELP:
				CallSpecWindow(HELPWIND); continue;
			case C_MENU:
				if(!(c=RunMenu())) continue;
				goto SW;
			case C_COL:
				ChangeColor(); continue;
			case C_SETSINP:
				StringsInPage(NULL); continue;
			case C_VIEWDB:
				CallSpecWindow(DBWIND); continue;
			case C_VIEWCB:
				CallSpecWindow(CBWIND); continue;
			case C_INPSEVDO:
				InputPsevdoGraph(); continue;
			default:
				dpo(bl); continue;
			}
		}
	}
END:
	if(buf!=NULL) {
		put_scrold(buf); cp_screenold(); zero_scrold();
		Free(buf);
	}
	else
		ClrScr();
	dpend();
	exit(0);
}

void clean_temporary()
{
	int i;
	char str[NAMESIZE+1];

	sprintf(str,".%d.macro",getpid());
	unlink(str);
	for(i=0;i<LenAllFiles;i++) {
		if(AllFiles[i].typ==TEMPWIND)
			unlink(AllFiles[i].FileName);
		if(AllFiles[i].typ==MANWIND) {
			sprintf(str,MANNAME,getlogin(),get_tty(),AllFiles[i].FileName+4);
			unlink(str);
		}
	}
	DelBufFile();
}
