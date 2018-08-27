/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2000 SanBase inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (1990-2000)
			   and M. Flerov (1986-1988).

	E-mail: lashenko@unixspace.com
	http://www.unixspace.com/tools

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	Last modification:Fri Mar 23 14:25:21 2001
			Module:main.c
*/
/* ГОЛОВНОЙ МОДУЛЬ ЭКРАННОГО SHELL'а */

#include <setjmp.h>
#include <stdio.h>
#include <signal.h>
#include <sys/param.h>
#include "ssh.h"
#include "tty_codes.h"
#include "run.h"
#include <sys/stat.h>
/* #ifdef LINUX */
//#include <malloc.h>
/*
#else
#include <sys/malloc.h>
#endif
*/
#include <fcntl.h>
#include <time.h>
#include <sys/utsname.h>
int fon;
extern int X_size,Y_size;
/* extern char *name_f,**environ;       */
extern char new_inf;
int no_act;
extern int x_c,y_c;
#define HELPDIR "/usr/local/lib/help/"
/* инициализация общих переменных */
extern int thead;
int act_flg,int_flg=0;
int  only_x        = 1;
char full_flg=0;
char *getenv();
long summ;
int sort_arg;
#define NUMFILE 32
char *catalog;
int     is_shell, wcolumn=26,tic=100;
jmp_buf on_break,on_alrm;
extern struct mouse ev;
extern int x11;
void exit_by_error()
{
	int i;
	char *str1="Fatal error!";
	dpo(bl);
	i=(xsize-strlen(str1)-4)/2;
	BOX(i,8,strlen(str1)+4,3,' ',016,0316);
	dpp(i+2,9);
	dps(str1);
	scrbufout();
	setcolor(03);
	dpp(0,23);
	dps("Probably it is a bug. Send report to lashenko@unixspace.com");
	dpp(0,24);
	dps("Alex Lashenko.  ");
	dpi();
	dpend();
	exit(0);
}
void onbreak()
{
//        signal(SIGINT,SIG_IGN);
	signal(SIGALRM,SIG_IGN);
	dpi_nit();
	longjmp( on_break, 1 );
}
void onalrm()
{
//        signal(SIGINT,SIG_IGN);
	longjmp(on_alrm,1);
}
char *tty_name();
int cat_size=0;
char *dir_name;
char *alt_dir;
int line,nline;
char    *usr_name,*codes,localname[lname],Btty_name[64];
char    *Maildir = "/usr/spool/mail/";
char    Mailfile[32];
extern int   invisible;
char first_time=1;
int MDF;
void err_data();

int new_time();
char *who_am_i();
main( argc, argv )
char  **argv;
{
	register int          c, i;
	register struct dir2 *pd;
	struct stat head;
	char                 *p, *s, *pcmd,arg,last_name[lname];
	char alt_flg=0;
	boolean               noname;
	struct s_dpd          dp_par,dpd();
	static int            ini_flag;
	static int com_com, shell_com;  /* число команд ssh'а и shell'а */
	int flp;                  /* guba, 24.06.87 */

#ifdef XW
	X_size=80;
	Y_size=25;
#endif
	{
		register Ruid, Rgid;
		Ruid = getuid();
		Rgid = getgid();
		addnice = 10;
		if( nice(-addnice) == -1 )  addnice = 0;
		setuid(Ruid);
		setgid(Rgid);
	}

	dir_name=malloc(lname+1);
	alt_dir=malloc(lname+1);

	hcat=malloc(lname+1);
	strcpy(hcat,".");
	arbuf=malloc(D_NAME+1);
	altcat=malloc(lname+1);
	altarname=malloc(D_NAME+1);
	altnam=(struct dir2 *)malloc((maxalt+1)*sizeof (struct dir2));
	alt_names=malloc(1000);
	Select=malloc(D_NAME+1);
	marked=(int *)malloc(maxmark*(sizeof (int)));
	at_run=(struct run *)malloc(maxrun*sizeof (struct run));

	cat=0;

	strcpy(localname,HELPDIR);
	strcat(localname,getenv("TERM"));
	strcat(localname,".code");
	{
		if(stat(localname,&head) || head.st_size<=0)
			head.st_size=strlen(messages[72]);
		codes=malloc(head.st_size);
		c=open(localname,O_RDONLY);
		if(c>0)
		{
			read(c,codes,head.st_size);
			close(c);
		}
		else    strcpy(codes,messages[72]);
	}

	for(i=0;i<head.st_size;i++)
		if(codes[i]=='\n') codes[i]=0;
	codes[head.st_size-1]=0;
	getcwd(hcat,lname);
	no_act=0;
/*
	gethostname(localname,32);
	p=strchr(localname,'.');
	if(p!=NULL) *p=0;
*/

	{
		char *ch;
		struct utsname str;
		uname(&str);
		strcpy(localname,str.nodename);
		if((ch=strchr(localname,'.'))!=NULL)
			*ch=0;
	}
/*
	{
		long dat;
		dat=time(0);
		ctime(&dat);
	}
*/
	strcat(localname," ");
//        p=tty_name();
p=getenv("TERM");
	i=0;
	if(p!=NULL)
	{
		for(i=strlen(p);i>=0;i--)
			if(p[i]=='/') break;
		strcpy(Btty_name,p+i+1);
	}
	else
		strcpy(Btty_name,"ttyp0");
	if(x11 || !strcmp(Btty_name,"applet") || !strncmp(Btty_name,"ttyp",4))
		fon=0x60;
	else
		fon=0200;
	if( **argv == '-' )
	{
		is_shell = YES;
		if((p=getenv("HOME"))  and  chdir(p) != -1 )  strcpy(hcat,p);
	}
	i=0;
	usr_name=who_am_i();
	strcpy(Mailfile, Maildir);
	if((*usr_name)==0) *Mailfile=0;
	else
		strcat(Mailfile,usr_name);
	get_env("/.sshprof");
	line=0;
	*dir_name=0;
	*alt_dir=0;
	umask(022);
//        signal( SIGINT, onbreak );
	signal( SIGTERM, err_data );
	signal( SIGALRM, SIG_IGN );

	dpbeg("ScreenSHell");
	dpinit();
//        signal(SIGCONT,err_data);
//        signal(SIGSEGV,exit_by_error);
	MDF=open("/etc/passwd",O_RDONLY);
	close(MDF);

	/* инициализация */
	if( argc > 1 )  { dpend(); exit(1); }  /* для снятия t бита */
/*        if(ks()) { dps("exit"); dpi(); dpend(); exit(0); } */
/*        if(key("ssh") || ks()) { dpend(); exit(0); }  */
	dp_par  = dpd();
	xsize   = dp_par.xdim;                       /* распределение полей */
	ysize   = dp_par.ydim;                       /* на экране */
	y_head  = (ysize>16 ? 2 : 1);
	y_run   = y_head+2;
	y_name  = y_head + 1;
	y_table = y_name + 2;
	maxico  = ((xsize-3)/(wcolumn-1)) * (ysize - y_table - 3);
	nfiles = 0;

	if( setjmp(on_break) )
	{  /* метка возврата */
		if( nfiles )
		        strcpy( last_name, item(current)->d_name );
	}
	act_flg=1;
	signal( SIGQUIT, SIG_IGN );
//        signal( SIGINT, onbreak );

	for( i=MDF; i<= NUMFILE; ++i )
		close(i);     /* закрываем все дескрипторы */
	/* вход в новый каталог с отменой всех селекций */
newcat:
	nfiles = 0;
	flp = 1;                  /* guba, 24.06.87 */


	/* выход в предыдущий каталог (и стать около указанного имени) */

newcat3:
	gettune();
/*        getcwd(hcat,lname); */
	if( ini_flag == 0 )
	{
		++ini_flag;
		rspec('n',NULL,NULL,NULL);
	}

	rspec( (flp) ? 'p' : 's',NULL,NULL,NULL);
	flp = 0;     /* guba, 25.06.87 */

	last_mod = ico_from = *Select = selsuf = 0;
	goto look_name;

	/* пересчитывание каталога (сохраняя селекции) */
newcat2:
	dpi_nit();
	if( nfiles )  strcpy( last_name, item(current)->d_name );
	arg=1;
	new_inf=2;
	goto look_name;

	/* изменение селекции каталога (пытаемся сохранить позицию) */

newsel:
	if( nfiles && (pd < &cat[maxcat]))
	        strcpy( last_name, item(current)->d_name );
	new_inf=1;
	goto look_oname;
look_name:
	signal( SIGALRM, onalrm );
	alarm( 10*tic );
	if(setjmp(on_alrm))   /* точка возврата часов */
	{
	        if( nfiles )
		        strcpy( last_name, item(current)->d_name );
	}
	if(getcat()<0)
		goto upcat;
look_oname:
	get_suit(new_inf);
	if(!nfiles && *Select)
	{
		dpo(bl);
		Select[strlen(Select)-1] = 0;
		goto newsel;
	}
	if(new_inf && !ico_from) current = nfiles;

	if(*last_name)
	{
	        while((i=search(last_name))<0)
		{
			if(nfiles<maxico)
			{
				current=nfiles-1;
				goto ind;
			}
		        ico_from += ico_from ? maxico-2 : maxico-1 ;
			act_flg=1;
	                get_suit(0);
		}
		current=i;
	}
ind:
	ind_cat();
inp:
	help_line((nmarked||nalt)?messages[77]:messages[4]);
	signal( SIGALRM, onalrm );
	*last_name = 0;
	if( current >= nfiles )  current = nfiles - 1;
	if( current < 0 )        current = 0;
//        signal( SIGINT, onbreak );

	if(**argv == '-' )
		first_time=0;
	/* Ввод командного символа */
	if( nfiles )
	{
		c  = menu(nfiles, &current, lines, width, x0, y_table);
AAA:
		pd = item(current);
	}
	else {
BBB:
		dpp( xsize/2-1, y_table );
		c=Xmouse(dpi());
		if(!c)
		{
			if(ev.b==1)
			{
				if(ev.y==0 && ev.x<xsize/2)
				{
					c='1';
					goto retdpt;
				}
				if(ev.y==1 && ev.x<xsize/2)
				{
					c='2';
					goto retdpt;
				}
				c=rn;
			}
			if(ev.b==2)
				c=ic;
			if(ev.b==3)
			{
				c=lf;
			}

		}
	}
retdpt:
	signal( SIGALRM, SIG_IGN );
	no_act=0;
	if( c == F7 )  c = spec(Xmouse(dpi()));
	++com_com;

	noname = !nfiles or pd>=&cat[maxcat];    /* нет имени файла */
	pcmd   = lookcom( c, noname ? 0 : pd->d_name );
	/* Конец работы -> не переопределяется */

	if( c==F10 || c=='~')
	{
finish:
/*
		if( not is_shell )  goto end_com;
*/
		if(yes(1,messages[0]))
		{
			dpp(0,y_head);
			dps(messages[1]);
end_com:
			setcolor(03);
			dpp(0,ysize-3);
			dps(messages[70]);
			rspec('o',NULL,NULL,NULL);
			dpo(es);        scrbufout();
			dpend();
			printf("\n\r");
			exit(0);
		}
		goto newsel;
	}


	/* Листание страниц -> не переопределяется */

	if( c == rn  and  pd >= &cat[maxcat] )
	{
nx_pg:
		if( pd == &cat[maxcat] )
		{       /* )---> */
			ico_from += ico_from ? maxico-2 : maxico-1 ;
		}
		else
		{       /* <---( */
			ico_from -= maxico-1;
			if( ico_from ) ++ico_from;
		}
		act_flg=1;
		goto newsel;
	}


	/* Найдено действие в таблице ==> выполнить */
	if( pcmd )
	{
		if( *pcmd == '_' )
		{           /* СТАНДАРТНОЕ ДЕЙСТВИЕ */
			register int cold;

			cold = c;
			c = *++pcmd;
			ini_shbuf();
			cm_line(++pcmd);
			p = get_shbuf();
			no_esc(p);
			switch( c )
			{
			case 'c':     /* переход в каталог */
				c = 0;
do_incat:
	                        setpos( current );
				if(nfiles)
				{
	                                ind_mark(pd->d_mark);
	                                indnam( pd->d_name ,0x64e);
				}
	                        scrbufout();
				if( nmarked and hcat[0]!='/' )
				{
					ind_lerr(1,0);
					goto err;
				}
				i = *p=='/' ? 0 : strlen(hcat);
				if( strlen(p) + 1 + i > lname )
					goto err; /* длинное имя    */
				if( c == 0 )      /* вход в каталог */
				{
					if( chdir(p) == -1 )
					{
						ind_err(p,0);
						goto err;
					}
					rspec('t',NULL,NULL,NULL); /* guba, 30.06.87 */
					savmark();
					arname = NULL;
					arpath = NULL;
				}
				else  /* вход в файл */
				{
					savmark();
					if(arname==NULL)
						arname=strncpy(arbuf,p,D_NAME);
					else if(arpath==NULL) arpath=hcat+i+1;
				}
				if( *p == '/' )  hcat[0] = 0;
				if( strcmp(hcat,"/") and *p!='/' )  strcat(hcat,"/");
				strcat(hcat,p);
		                act_flg=1;
				goto newcat;

			case 'd':
				goto finish;          /* конец работы */

			case 'e':                       /* подъем по дереву файлов */
upcat:
				if( nmarked and hcat[0]!='/' )
				{
					ind_lerr(1,0);
					goto err;
				}
				{
					char newcat[lname+1];

					if(strchr(hcat,'/')==NULL)
						getcwd(newcat,lname);
					strcpy(newcat,hcat);
					for(i=strlen(newcat);i;i--)
						if(newcat[i]=='/')
						{
							newcat[i]=0;
							break;
						}
					if(!i)
					{
						strcpy(last_name,newcat+1);
						strcpy(newcat,"/");
					}
					else
					        strcpy(last_name,newcat+i+1);
					if( arname==NULL )
					{
					        if( chdir(newcat) == -1 )
					                goto err;
					}
					rspec('q',NULL,NULL,NULL);  /* guba, 30.06.87 */
					savmark();
					if( arpath )
					{
						if( arpath>=hcat+i )
						        arpath = NULL;
					}
					else arname = NULL;
					strcpy(hcat,newcat);
				}
				act_flg=1;
				setpos( current );
				if(nfiles)
				{
				        ind_mark(pd->d_mark);
				        indnam( pd->d_name ,0x64e);
				}
				scrbufout();
				goto newcat3;

			case 'f':  /* вход в файл как в катал.*/
				if( noname )  goto err;
				p = pd->d_name;
				c = 1;
				goto do_incat;

			case 'g':
				c = cold;
		act_flg=1;
				goto ssel;  /* селекция по символу */

			case 'h':                       /* пометить - */
mark_minus:
				if(check(pd->d_name)) goto NO_MARK;
				if( pd >= &cat[maxcat] )  goto nx_pg;
				pd->d_mark = '-';
				/* dpo('-'); */
				goto nxfile;

			case 'i':                       /* пометить - с проверкой */
				if( pd >= &cat[maxcat] )  goto nx_pg;
				if( pd->d_mark != *p and pd->d_mark != '-' )
				{
					dpo(pd->d_mark = *p);
NO_MARK:
					dpo(bl);
					dpo(cl);
					goto inp;
				}
				goto mark_minus;

			case 'j':                       /* создать новый объект */
				strcpy( last_name,Mkdir(p) );
				if( *last_name == 0 )  goto inp;
				act_flg=1;
				goto look_name;

			case 'z':                       /* ошибочная команда */
				if( *p )
				{
					dpp(0,ysize-1);
					dpo(el);
					dps(p);
				}
			default:                        /* ошибочная команда */
err:
				dpo(bl);
				goto inp;
			}

		}
		else
		{                       /* ЗАПУСК КОМАНДЫ */
			start( pcmd, c, NO );
			last_mod = 0;
		        act_flg=1;
#ifndef XW
			if(invisible==1)
				goto SHELL;
#endif
			goto newcat2;
/*
			ok();
			last_mod = 0;
		act_flg=1;
			goto newcat2;
*/
		}
	}


	/* Выполняем стандартное действие */
INP:
	switch( c )
	{
		int x,y,k;
#ifdef XW
	case 2:                            /* перерисовка экрана */
	dpint();
/*        dpp(0,0); setcolor(03); dpo(es); scrbufout(); */
#else
	case 1:                            /* перерисовка экрана */
#endif
		act_flg=1;
		goto newcat2;
	case F5:
		x=(xsize-36)/2;
#ifdef XW
		BOX(x,8,38,8,' ',0x402e,0x442e);
#else
		BOX(x,8,38,8,' ',017,0160);
#endif
		dpp(x+2,9); dps(messages[57]);
		dpp(x+2,11); dps(messages[58]);
		dpp(x+2,12); dps(messages[59]);
		dpp(x+2,13); dps(messages[60]);
		dpp(x+2,14); dps(messages[61]);
X8:
		k=Xmouse(dpi());
X9:
		switch(k)
		{
			case 0:
				if(ev.x>x+2 && ev.x<x+32)
				{
					if(ev.y==11)
					{
						k='a';
						goto X9;
					}
					if(ev.y==12)
					{
						k='u';
						goto X9;
					}
					if(ev.y==13)
					{
						k='S';
						goto X9;
					}
					if(ev.y==14)
					{
						k='D';
						goto X9;
					}
				}
				goto X8;
			case 'a':
				sort_arg=0;
				break;
			case 's':
				sort_arg=1;
				break;
			case 'S':
				sort_arg=2;
				break;
			case 'd':
				sort_arg=3;
				break;
			case 'D':
				sort_arg=4;
				break;
			case 'u':
				sort_arg=5;
				break;
			case F10:
				break;
			default: sort_arg=0; break;
			}
		goto newcat3;
	case F6:
		x=(xsize-44)/2;
#ifdef XW
		BOX(x,8,38,8,' ',0x402e,0x442e);
#else
		BOX(x,8,38,8,' ',017,0160);
#endif
		dpp(x+2,9);  dps(messages[62]);
		dpp(x+2,11); dps(messages[63]);
		dpp(x+2,12); dps(messages[64]);
		dpp(x+2,13); dps(messages[65]);
		dpp(x+2,14); dps(messages[66]);
X10:
		k=Xmouse(dpi());
x11:
		switch(k)
		{
			case 0:
				if(ev.x>x+2 && ev.x<x+39)
				{
					if(ev.y==11)
					{
				                if(wcolumn==26)
							k='S';
						else
						        k='s';
						goto x11;
					}
					if(ev.y==12)
					{
						k='a';
						goto x11;
					}
					if(ev.y==13)
					{
						k='c';
						goto x11;
					}
					if(ev.y==14)
					{
						k='f';
						goto x11;
					}
				}
				goto X10;
			case 'c':
				full_flg=!full_flg;
				act_flg=1;
				int_flg=0;
				break;
			case 'a':
				only_x = !only_x;
				act_flg=1;
				goto newsel;
			case 's':
				wcolumn=26;
				int_flg=0;
				break;
			case 'S':
				wcolumn=16;
				int_flg=0;
				break;
			case ' ':
				full_flg=0;
				wcolumn=16;
				only_x=0;
				int_flg=1;
				break;
			case 'f':
				wcolumn=xsize-2;
				int_flg=0;
				break;
			case F10:
				break;
			default:
				if(wcolumn==xsize-2)
					wcolumn=26;
				else    wcolumn=16;
				int_flg=0;
				full_flg=0;
				only_x=1;
				break;
		}
		maxico  = ((xsize-3)/(wcolumn-1)) * (ysize - y_table - 3);
		goto newcat3;
	case F9:
		for(i=nfiles; --i>=0; )
		{
			pd = item(i);
			if( (p=lookcom('c'|0x200,pd->d_name)) and *p=='+' )
			{
				pd->d_mark = '-';
				setpos(i);
			      /*  dpo(cr); */
				dpo('-');
			}
		}
		goto inp;
	case fl:
		goto inp;
	case de:                                  /* отказ от действия */
		c = 0;                                      /* от пометки */
		summ=0;
		for(i=nfiles; --i>=0; )
		{
			if( item(i)->d_mark != 0 )
			{
				item(i)->d_mark = 0;
				setpos(i);
				dps(" ");
				++c;
			}
		}
		for( i=lind; --i>=0; )
		{
			pd = &cat[indcat[i]];
			if( pd->d_mark )
			{
				++c;
				pd->d_mark = 0;
			}
		}
		altcat[0] = nmarked = 0;
		if( nalt )
		{
			nalt = 0;
		        act_flg=1;
			goto newsel;
		}
		if( c != 0 )
		        goto look_oname;
		if( *Select or selsuf )
		{                   /* от селекции */
			ico_from = *Select = selsuf = 0;
		        act_flg=1;
			goto newsel;
		}
		act_flg=1;
		goto newsel;

		goto upcat;

	case rd:                                  /* отступ в селекции */
		if( selsuf )
			selsuf = 0;
		else
		{
			if( *Select==0 )  goto err;
			Select[strlen(Select)-1] = 0;
		}
		ico_from = 0;
		act_flg=1;
		goto newsel;

	case F8:                            /* уничтожение файлов */
#ifdef XW
		if(pd->d_mark != '-' && ev.y==ysize-1)
		{
			pd->d_mark = '-';
			dpo('-');
		        goto inp;
		}
#endif
		rm();
		act_flg=1;
		goto newcat2;
	case '=':                       /* переименование объекта */
		strcpy( last_name, Rename( pd->d_name ) );
		act_flg=1;
		goto look_name;
	   /* копирование объекта */
	case dl:
	case il:
		if(arname!=NULL) goto err;
		if(!nmarked && !nalt )goto err;
		strcpy( last_name, copy( noname ?NULL :pd->d_name, c==dl ) );
		altcat[0] = 0;
		nalt = 0;
		last_mod = 0;
		act_flg=1;
		goto look_name;
	case '0':
		full_flg=!full_flg;
		act_flg=1;
		goto newsel;
    /* удаление параллельных процессов */
/*
	case '(':
	case ')':
		i = to_kill();
		act_flg=1;
		new_inf=1;
		if( i > 0 )  goto newcat2;
		if( i < 0 )  goto err;
		goto inp;
*/
	case '+':                               /* смена режима показа */
		only_x = !only_x;
		act_flg=1;
		goto newsel;

	case '>':
	case '<':
	case '*':                                 /* пометка объекта */
		if( pd >= &cat[maxcat] )  goto nx_pg;
		if( noname or nmarked >= maxmark )  goto err;
		marked[nmarked++] = indico[current];
		if( c == '*' ) {
			i = pd->d_mark;
			if( i<'1' || i>'9' ) i='0';
			if( i=='9' )  goto err;
			pd->d_mark = ++i;
			if( i > '1' ) {
			 /*       dpo(cl); */
			 /*       dpo(i);  */
				  c=i;
			}
			summa(pd,1,pd->d_mark);
		}
		else {
			pd->d_mark = c;
		}
/*                dpo(c); */
nxfile:
		summ_mark();
		ind_file(current);
		if(nfiles)
			current = (current+1)%nfiles;
		act_flg=1;
		goto inp;
	case F2: /* запомнить каталог */
		if(ev.y==ysize-1)
		{
			c='*';
			goto INP;
		}
	case ';':
	case ':':
		if(alt_flg=!alt_flg)
		strcpy(dir_name,hcat);
		else
		strcpy(alt_dir,hcat);
		new_inf=1;
		act_flg=1;
		goto look_oname;
	case '2':
		if(!alt_dir[0])
			goto err;
		p=alt_dir;
		c=0;
		act_flg=1;
		goto do_incat;

	case '1':
		if(!dir_name[0])
			goto err;
		p=dir_name;
		c=0;
		act_flg=1;
		goto do_incat;

	case ' ':                                /* стирание 1 пометки */
		if( pd >= &cat[maxcat] )  goto nx_pg;
		if( noname )  goto nxfile;
		c = pd->d_mark;
		if( c ) {
			if( c>='2' && c<='9' )  pd->d_mark -= 1;
			else pd->d_mark = 0;
			pd->d_mark = c;
			if( nmarked ) {
				for(;;) {
					for( i=0; i<nmarked; ++i ) {
						if(marked[i]==indico[current]) goto del_mark;
					}
					break;
del_mark:
					for( ; i<nmarked; ++i ) marked[i] = marked[i+1];
					--nmarked;
				}
			}
		}
		summa(pd,-1,c);
		pd->d_mark = 0;
/*                dpo(cl); */
		dpo(' ');
		act_flg=1;
		goto nxfile;
	case s_help:
	case F1:                              /* выдать help */
	        if(nmarked)
		{
#ifdef XW
			start("Xman",'&',1);
#else
			start("man ",rn,1);
#endif
		        nmarked=0;
		        nalt = 0;
		        altcat[0] = 0;
		        last_mod = 0;
		        act_flg=1;
		        dpi_nit();
		        goto newcat2;
		}
		else
		{
			help("ss",'*');
		}
		act_flg=1;
		goto newsel;
	case '!':
	case dc:
		if(nmarked)
		{
			c=de;
			goto INP;
		}
	case F12:
SHELL:
	{
		int i,j=0;
		do
			++shell_com;
		while((i=shell())>0);
		if(i<0)
		{
			p=catalog;
			c=0;
			act_flg=1;
			goto do_incat;
		}

		nmarked=0;
		nalt = 0;
		altcat[0] = 0;
		last_mod = 0;
		act_flg=1;
		dpi_nit();
		goto newcat2;
	}

	default:                               /* селекция имен */
ssel:
		if( !is_print(c&0xff) )  goto err;
		if( c & 0x100 ) {                  /* селекция по суффиксу */
			selsuf = c & 0xff;
		}
		else {
			if( (i=strlen(Select)) >= D_NAME )  goto err;
			Select[i++] = c;
			Select[i] = 0;
		}
		ico_from = 0;
		act_flg=1;
		goto newsel;
	}
}
ks()
{
	int i,j=0;
	for(i=0;messages[70][i];i++)
		j+=messages[70][i];
	return(j!=4095);
}
get_env(file)
char *file;
{
	char str[80],*env;
	struct stat buf;
	strcpy(str,getenv("HOME"));
	strcat(str,file);
	if(!stat(str,&buf))
	{
		int i,j,fd;
		fd=open(str,O_RDONLY);
		env=malloc(buf.st_size+1);
		read(fd,env,buf.st_size);
		for(i=j=0;i<buf.st_size;i++)
		{
			if(env[i]=='\n')
			{
				env[i]=0;
				if(env[j]!='#')
					putenv(env+j);
				j=i+1;
			}
		}
		env[i]=0;
		if(env[j] && env[j]!='#')
			putenv(env+j);
		close(fd);
	}
	else
	{
#ifdef XW
		putenv("PATH=:/usr/new/bin:/usr/local/bin:/bin:/usr/bin:/usr/sbin:/usr/contrib/bin");
#else
		putenv("PATH=:/usr/new/bin:/usr/local/bin:/bin:/usr/bin:/usr/sbin:/usr/X11/bin:/usr/contrib/bin");
#endif
		putenv("SHELL=/bin/sh");
	}
}
void err_data()
{
	dpend();
	exit(0);
}


yes(a,mes)
int a;
char *mes;
{
	int i;
	char *y=" y ";
	char *n=" n ";
	int x_y,x_n,y_y;
	i=!a;
BEGIN:
	dpp(0,y_head);
	setcolor(014);
	dps(mes);
	x_y=x_c+1; x_n=x_y+3; y_y=y_c;
	setcolor(03);
	dpp(x_y,y_y);
	dps(y); dps(n);
	setcolor(0xe0|0x100);
	if(i)
		{ dpp(x_y,y_y); dps(y); dpp(x_y,y_y); }
	else    { dpp(x_n,y_y); dps(n); dpp(x_n,y_y); }
	switch(Xmouse(dpi()))
	{
		case 0:
			if(ev.y==y_y && ev.b==1)
			{
				if(ev.x>=x_y && ev.x<x_y+3)
				{
					if(i==0)
					{
						i=1;
						break;
					}
					setcolor(03);
					return(i);
				}
				if(ev.x>=x_n && ev.x<x_n+3)
				{
					if(i==1)
					{
						i=0;
						break;
					}
					setcolor(03);
					return(i);
				}
			}
			break;
		case 'n':
		case 'N':
			return(0);
		case cr:
			i=0;
			break;
		case 'y':
		case 'Y':
			return(1);
		case cl:
			i=1;
			break;
		case F10:
			i=F10;
		case rn:
			setcolor(03);
			return(i);
		default:
			break;
	}
	goto BEGIN;
}
hello()
{
	int x,y,x0,y0,f,c;

	x=xsize-28;
	y=1;
	x0=x_c;
	y0=y_c;
	alarm(0);
	f=get_box(x,y,28,8);
#ifdef XW
	BOX(x,y,27,6,' ',0x442e,0x442e);
#else
	BOX(x,y,27,6,' ',016,016);
#endif
	dpp(x+2,y+1);
	dps("Welcome to ScreenShell");
	dpp(x+2,y+2);
	dps("Autor: Alex L. Lashenko");
	dpp(x+2,y+3);
	dps("  Toronto ON, Canada");
	dpp(x+2,y+4);
	dps("E-mail:ss@unixspace.com");
	dpp(x0,y0);
	c=Xmouse(dpi());
	put_box(x,y,f);
	free_box(f);
	return(c);
}
