/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: filtr.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 *  Запуск фильтров для текущего файла
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <errno.h>
#include <tty_codes.h>
#include "ned.h"

static char SHELL[]="/bin/sh";

static int exe_filtr(NEF *,int,int,char *);

static char break_flag=0;

/* Обработка запроса на запуск фильтра */
int Exec_Filtr(NEF *W)
{
	int nb;
	char str[7],sss[LINESIZE+1];
	int err=0;

	W->nocom=1;
	PutTitleEvent(W,NULL);
	nb=get_box(0,3,l_x,12);
	setcolor(COLMBGR);
	title(3,pmes(23));
	wind(15,8,49,6);
	dpp(17,9);
	dps(pmes(22));
	dpp(17,11);
	dps(pmes(141));
	dpp(17,12);
	setcolor(COLEDIT);
	if(NMAX(W)-CS(W)<=1)
		strcpy(str,"1");
	else
		sprintf(str,"%d",NMAX(W)-CS(W));
	dpn(6,' ');
	dpp(17,12);
	dps(str);
	*sss=0;
	line_status(36);
	if(cmn.sfiltr<0)
		cmn.sfiltr=init_stk(LINESIZE);
	while(1) {
		int begstr=0,nstr=0,t;

		setcolor(COLEDIT);
		switch(t=Edit(W,0,sss,NAMESIZE,45,17,10,0,cmn.sfiltr)) {
		case C_RETURN:
EXEFILTR:
			if(!*sss) {
				dpo(bl); continue;
			}
			put_l_s(cmn.sfiltr,sss);
			if(t==C_RETURN) {
				begstr=CS(W); nstr=atoi(str);
			}
			err=exe_filtr(W,begstr,nstr,sss);
			goto END;
		case C_HOME:
HOM:
			begstr=0; nstr=NMAX(W); goto EXEFILTR;
		case C_MARKSTR:
CUR:
			begstr=CS(W); nstr=NMAX(W)-CS(W); goto EXEFILTR;
		case C_STOP:
STOP:
			put_l_s(cmn.sfiltr,sss); goto END1;
		case C_PGDN:
TO2:
			dpp(17,10); dpn(45,' '); dpp(17,10); dps(sss);
			break;
#ifdef X11
		case C_MOUSE:
			if(ev.b==3)
				goto STOP;
			else
				if(ev.x>=17 && ev.x<=39 && ev.y==12)
					goto TO2;
#endif
		default:
			dpo(bl); continue;
		}
EDNUMSTR:
		setcolor(COLEDIT);
		switch(t=Edit(W,0,str,6,6,17,12,'c',-1)) {
		case C_RETURN:
			if(atoi(str)<1) {
				dpo(bl); goto EDNUMSTR;
			}
			goto EXEFILTR;
		case C_HOME:
			goto HOM;
		case C_MARKSTR:
			goto CUR;
		case C_STOP:
			goto END1;
		case C_PGUP:
TO1:
			dpp(17,12); dpn(6,' '); dpp(17,12); dps(str);
			break;
#ifdef X11
		case C_MOUSE:
			if(ev.b==3)
				goto STOP;
			else
				if(ev.x>=17 && ev.x<=62 && ev.y==10)
					goto TO1;
#endif
		default:
			dpo(bl); continue;
		}
	}
END:
	if(!err) {
		W->flscr=1; ChangeRed(W,1);
	}
END1:
	if(err!=1) {
		W->nocom=0; put_box(0,3,nb);
	}
	free_box(nb);
	return(err);
}

static void break_filtr(int sig)
{
	break_flag=1;
}

/* Запуск фильтра */
static int exe_filtr(NEF *W,int begstr,int nstr,char *name)
{
	FILE *fdin=NULL,*fdout=NULL;
	int err=0,j;
	pid_t pid;
	void (*oldtrap)();
	STR *S=NULL;

	InsertHistory(W->AS,0,0,U_BEGIN);
	S=InitSTR();
	AddSTR(S,LINESIZE);
	sprintf(S->s,F_NAMEIN,TMPDIR,pid=getpid());
	if((fdin=fopen(S->s,"w"))==NULL) {
		errno=0; err=-1; goto END;
	}
	nstr=min(nstr,NMAX(W)-begstr);
	if(WriteFile(W,begstr,nstr,fdin)) {
		sprintf(S->s,F_NAMEIN,TMPDIR,pid);
		unlink(S->s);
		goto END;
	}
	fclose(fdin);
	oldtrap=signal(SIGINT,break_filtr);
	if(fork()==0) {
		close(0);
		sprintf(S->s,F_NAMEIN,TMPDIR,pid);
		if(open(S->s,O_RDONLY)<0) goto NOFIND;
		sprintf(S->s,F_NAMEOUT,TMPDIR,pid);
		close(1);
		if(creat(S->s,0600)<0) goto NOFIND;
		sprintf(S->s,F_NAMEERR,TMPDIR,pid);
		close(2);
		if(creat(S->s,0600)<0) goto NOFIND;
		ExeCommand(name);
NOFIND:
		exit(RETNOFIND);
	}
	wait(&j);
	/* Удаляем входной файл для фильтра */
	sprintf(S->s,F_NAMEIN,TMPDIR,pid);
	unlink(S->s);
	signal(SIGINT,oldtrap);
	if(break_flag) {
		emess(pmes(6)); RunMess(W,0,pid); err=-1; goto END1;
	}
	j>>=8;
	if(j==RETNOFIND) {
		emess(pmes(16)); RunMess(W,0,pid); err=-1; goto END1;
	} else {
		if(err=RunMess(W,j,pid)) goto END1;
	}
	/* Открываем файл с тем, что наделал фильтр */
	sprintf(S->s,F_NAMEOUT,TMPDIR,pid);
	if((fdout=fopen(S->s,"r"))==NULL) {
		errno=0; err=-1; goto END1;
	}
	/* Записываем старое содержимое как удаленное */
	j=CS(W); CS(W)=begstr;
	InitMark(W);
	CS(W)=begstr+nstr;
	EndMark(W);
	CS(W)=j;
	if(PutMemBlock(W,0)) {
		err=-1; goto END1;
	}
	MoveMemPoz(W->wd,begstr,-nstr);
	DeleteStrInBuf(W->AS,nstr,begstr);
	W->flscr=1;
	ChangeRed(W,1);
	/* Читаем, чего наделал фильтр */
	if(NMAX(W)<=CS(W))
		InsertStrInBuf(W->AS,CS(W)-NMAX(W),NMAX(W));
	if(ReadFile(W,begstr,fdout,1)) {
		/* Ошибка,  пытаемся восстановить старое */
		j=CS(W);
		CS(W)=begstr;
		GetMemBlock(W,0);
		CS(W)=j;
		ResetStrBuf(&DB);
		err=-1;
	}
	fclose(fdout);
END1:
	sprintf(S->s,F_NAMEOUT,TMPDIR,pid);
	unlink(S->s);
END:
	if(S!=NULL)
		FreeSTR(S);
	InsertHistory(W->AS,0,0,U_END);
	return(err);
}

#ifndef NO_SHELL

void ExeUsrSh()
{
	struct passwd *p;
#ifdef X11
	static char *ch="xterm -geometry 80x18+0+500 &";
#else
	char *ch;
#endif
	int k;
	int nb;

	stor_poz();
#ifndef X11
	for(p=getpwent();p!=NULL && strcmp(p->pw_name,getlogin());p=getpwent());
	endpwent();
	if(p==NULL || !Strlen(p->pw_shell))
		ch=SHELL;
	else
		ch=p->pw_shell;
#endif
	nb=get_box(0,0,l_x+1,l_y+1);
	ClrScr(); scrbufout();
	Ttyreset();
#ifndef X11
	do {
#endif
		if(fork()==0) {
			ExeCommand(ch);
#ifndef X11
			exit(RETNOFIND);
#endif
		}
#ifndef X11
		wait(&k);
		if((k>>8)!=RETNOFIND)
			break;
		else
			if(ch!=SHELL) {
				ch=SHELL; continue;
			}
			else
				emess(pmes(111));
	}
	while(ch!=SHELL);
#endif
	Ttyset();
	put_box(0,0,nb);
	free_box(nb);
	flush_dpi();
	RestoreScreen();
	rest_poz();
}

#endif

extern  int     errno;
extern  char   *getenv();

static char  cshell[] = "/bin/csh";
extern char **environ;

int Execlp( char *name,  char *argv )
{
	return(Execvp(name, &argv));
}

int Execvp( char *name, char **argv )
{
	register char *cp;
	char fname[NAMESIZE];
	char *newargs[LINESIZE];
	register int i;
	char c;
	cp=getenv("PATH");
	if(strchr(name,'/') )  cp = "";
	do {
		cp=execat(cp,name,fname);
		execve(fname,argv,environ);
		if(errno==ENOEXEC) {
			if((i=open(fname,0))<0 ) return(-1);
			c=0; read(i,&c,1);
			close(i);
			newargs[0]=(c=='#')?basen(cshell):basen(SHELL);
			newargs[1]=fname;
			for(i=1;newargs[i+1]=argv[i];i++) {
				if (i==LINESIZE-1) return(-1);
			}
			execve(c=='#'?cshell:SHELL,newargs,environ);
			return(-1);
		}
	}
	while(cp);
	return(-1);
}

char *execat(char *s1, char *s2, char *si)
{
	register char *s;

	s=si;
	while(*s1 && *s1!=':' && *s1!='-' ) *s++=*s1++;
	if(si!=s) *s++='/';
	strcpy(s,s2);
	return(*s1?++s1:0);
}

static char metasymb[]="\\;!\"#$%&'()|*?^<>{}:@";

int ExeCommand(char *name)
{
	int i,j;
	char use_sh=0,**arg;

	for(;*name==' ';name++);
	if(!*name) return(-1);
	for(j=i=0;name[i];i++)
		if(strchr(metasymb,name[i])!=NULL) {
			use_sh=1; break;
		}
	if(!use_sh)
		for(i=0,j=1;name[i];i++)
			if(name[i]==' ') j++;
	arg=(char **)Malloc(((!use_sh?j+1:4)*sizeof(char *)));
	if(use_sh){
		arg[0]=strrchr(SHELL,'/')+1;
		arg[1]="-c";
		arg[2]=Strdup(name);
		arg[3]=NULL;
	}
	else {
		char *ch;

		for(j=0,ch=name;name=strtok(ch," \t");ch=NULL)
			arg[j++]=Strdup(name);
		arg[j]=NULL;
	}
	def_sig_action();
	return(Execvp(use_sh?SHELL:arg[0],arg));
}
