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

	Last modification:Fri Mar 23 14:24:57 2001
			Module:indcat.c
*/
#include <ar.h>
#include <fcntl.h>
#include <setjmp.h>
#include <sys/types.h>
#include <signal.h>
#include "ssh.h"
#include <pwd.h>
#include <sys/stat.h>
/*
#include <sys/a.out.h>
*/
#include <stdio.h>
#include "tty_codes.h"
extern int fon;

extern int rus_lat;
extern int   mhead, thead, uhead, phead,act_flg;
extern char *nhead,new_inf;
extern int   is_shell;
extern char *dir_name;
extern char *alt_dir;
extern char *usr_name,localname[lname],Btty_name[32];
extern char full_flg;
int no_act;
extern void onalrm();
extern int x_c,y_c,tic;
extern long summ;
long summ_dir;
int   wcolumn;
char    Mailfile[32];

/* for SCO only */
#define ARCMAGIC        0xff65
#define A_MAGIC1        0407
#define A_MAGIC2        0410
#define A_MAGIC3        0411
#define A_MAGIC4        0405

extern int x11;

static char direct[1024];
static unsigned char            sq0[] = {


	0, 6, 16, 27, 40, 55, 72, 91, 112, 135, 160, 187

//        0, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196

	};

ind_cat()
{
	register int          i, c;
	register struct dir2 *pd;
	char                 *p, *pe;

	if(!new_inf)
	{
		int x,y;
		x=x_c;
		y=y_c;
		if(no_act++==30)
			dpi_nit();
		new_time();
		check_mail();
		if(no_act>30)
		{
			no_act=301;
			signal( SIGALRM, onalrm );
			alarm(5*tic);
			dpi();
			act_flg=1;
			goto A;
		}
		if(act_flg)
			ind_act();
		dpp(x,y);
		signal( SIGALRM, onalrm );
		alarm( 10*tic );
		return;
	}
A:
	signal(SIGALRM,SIG_IGN);
	no_act=0;
	summ_dir=0l;
	dpp(0,0);
	setcolor(017);
	columns=get_columns(nfiles);
	width=wcolumn-1;
	lines = (nfiles+(columns-1))/columns;
	if(!lines) lines=1;
	if(lines<ysize/2)
	{
		setcolor(07);
		dpp(0,4+lines+2);
#ifndef XW
		dpo(es);
#endif

	}
	screen_clear();
	/* рабочий справочник */
	if(*dir_name)
	{
		dps("1-");
		dps(dir_name);
	}
	if(*alt_dir)
	{
		dpp(0,1);
		dps("2-");
		dps(alt_dir);
	}
	if(full_flg)
	{
		setcolor(014);
		dpp(xsize-2,0);
		dpo('F');
	}
	setcolor(016);
	dpp(0,y_head+1);
	dps(messages[2]);
	new_time();
	summ_mark();    /* суммарный размер помеченых файлов */
	if( !mhead ) {                                 /* селекция */
		dpp( 2, y_head );
		dps( only_x ? "+ " : "  " );
		if( c=strlen(Select) )  {
			dps(Select);
			dpo('?');
			dpn(18-c,'^');
		}
		if( selsuf )  {
			dpo('.');
			dpo(selsuf);
		}
	}
	if( !uhead )
	{           /* имя пользователя */
		i = geteuid();
		setcolor(014);
		dpp( (xsize-strlen(usr_name)-8) / 2, y_head ); /* p */
		dps( i ? "===" : "<<<" );
		dpo( is_shell ? ' ' : '(' );
		dps( usr_name );
		dpo( is_shell ? ' ' : ')' );
		dps( i ? "===" : ">>>" );
	}
	if( !mhead and nalt!=0 )  {
		dpp(0,y_head);
		dpo('*');
	}

	i=columns*width+1;
	if(i<xsize-1 && wcolumn == 16) i+=1;
	x0=(xsize - i)/2+1;
	box2(x0-1,4,i,lines+2,' ',07,(fon>>4),02,(fon>>4));
	if(wcolumn==26 && columns>1)
	{
		register x;
		int col=columns-1;

		x=x0+width-1;
		while(col)
			vert_s(x+(col---1)*width,4,lines+1);
	}
	p = hcat;                                      /* имя каталога */

	if(strlen(p)>46)
	{
		p=hcat+strlen(p)-46;
		strcpy(direct,p);
		if(*direct=='/') *direct=',';
		for(i=0;direct[i] && direct[i]!='/';i++)
			direct[i]=',';
		p=direct;
	}

	if( nhead ) {
		pe = index(nhead,'\n');
		ini_shbuf();
		*pe = 0;
		cm_line(nhead);
		*pe = '\n';
		p = get_shbuf();
	}
	setcolor(017);
	dpp( (xsize-strlen(p)) / 2, y_name ); /* p */
	dps( p );
	dpp(xsize-strlen(localname)-strlen(Btty_name),y_head-1);
	setcolor(017);
	dps(localname);
	setcolor(03);
	dps(Btty_name);

	for( i=0; i<nfiles; ++i )
		ind_file(i);
	{                                   /* объем каталога */
		long sum;
		sum=(summ_dir/1024)+1;
		if(summ_dir==0) sum=0;
		p=(char *)Conv(sum,6,10,' ');
		setcolor(016);
		dpp( xsize-strlen(p)-10, y_head+1 );
		dps(messages[3]);
		setcolor(03);
		dps(p);
		dps(" KB");
	}
	ind_lang(); /* Покажем язык ввода символов */
	help_line((nmarked||nalt)?messages[77]:messages[4]);
	signal( SIGALRM, onalrm );
	alarm(10*tic);
//        Set_Box_BG(x0-1,4,i,lines+2,fon>>4);

}
#define LINESIZE 256
/* функция центровки сообщения */
char *centr(str)
char *str;
{
	int size,num;
	register char *ch;
	register int i;
	char str1[LINESIZE+1];

	if(!(size=strlen(str)))
		return(str);
	num=xsize-size-1;      /* столько пробелов добавить */
	while(num>0)
	{
		size=strlen(str)+1;
		for(i=0,ch=str1;i<size;ch++,i++)
		{
			if(str[i]==' ' && num)
			{
				*ch=' '; ch++;
				num--;
				for(;str[i]==' ' && i<size;i++,ch++)
					*ch=str[i];
			}
			*ch=str[i];
		}
		strcpy(str,str1);
	}
	return(str);
}

static char *Keys[]=
{
       "Esc",  "",    "",           "F12",     "F3",  "",      "",     "",
       "Del",  "Tab", "Ctrl/Enter", "F1",      "F2",  "Enter", "F4",   "F5",
       "F6",   "F7",  "End",        "Home",    "Down","Up",    "Right","Left",
       "Ins",  "",    "Page/Up",    "Page/Do", "F8",  "F9",    "F10",  "F11"
};
help_line(line)
char *line;
{
	register int i,j,k;
	char word[256],str[256];

	strcpy(str,line);
	centr(str);
	dpp(0,ysize-1);
/*        setcolor(072); */
	setcolor(0x43a);
#ifdef XW
	setcolor(0x53a);
#endif
	dps(str);
	for(i=j=0;str[i];i++)
	{
		while(str[i] && str[i]==' ') i++;
		while(str[i] && str[i]!=' ')
			word[j++]=str[i++];
		word[j]=0;
		j=0;
		for(k=0;k<32;k++)
			if(!strcmp(word,Keys[k]))
			{
				dpp(i-strlen(word),ysize-1);
				setcolor(07);
				dps(word);
				break;
			}
		if(str[i]==0)
			break;
	}
}

get_columns(num)
int num;
{
	int Columns,max=(xsize-2)/(wcolumn-1);
	for(Columns=1;Columns<max;Columns++)
		if( num <= sq0[Columns] )  break;
	lines = (num+(Columns-1))/Columns;
	return(Columns);
}


indnam(name,ch)
char *name;
int ch;
{
	int i;
	int width,fn;

	fn=(ch&0xf0)>>4;
	if(fn!=(fon>>4))
		fn|=0x100;
	Set_Color(fn,ch&0xf);
	width=wcolumn==16?14:wcolumn==26?17:24;
	for(i=0;i<width;i++)
	{
		if(!name[i])
			break;
		dpo(name[i]);
	}
	if(name[i] && name[i+1])
	{
		dpo(cl);
		Set_Color(fon>>4,014);
		dpo('>');
		Set_Color((ch&0xf0)>>4,ch&0xf);
	}
}
setpos(i)
register int i;
{
	dpp( x0 + (i/lines)*width  , y_table + i%lines );
}


summ_mark()
{
	setcolor(03);
	dpp(9,y_head+1);
	dps((char *)Conv(summ,4,10,' '));
	dps(" KB");
}
summa(pd,i,c)
struct dir2 *pd;
int i;
char c;
{
	register int j;
	long size;
	size=pd->size;
	if(!c || c=='<' || c=='>' || c=='-') return;
		j=(size>>10)+1;
		if(i>0)
			summ+=j;
		else
			if((summ-=j)<0) summ=0;
}

//long time();
int new_time()
{
	long date;
	char *ch;

	if( !thead )
	{                 /* дата */
		setcolor(012);
		dpp( xsize-17, y_head );
		date=time(0);
	        ch=(char *)ctime(&date);
	        ch[16]=0;
	        dpp( xsize-strlen(ch), y_head );
	        dps(ch);
	}
}
ind_file(i)
int i;
{                      /* файлы */
	register struct dir2 *pd;
	int ch;

	if(!nfiles)
		return;
	setpos( i );
	pd = item( i );
	ind_mark(pd->d_mark);
	indnam( pd->d_name ,ch=(unsigned)get_tip(pd,full_flg));
	if(((ch-fon)>>4)&01)
		setcolor(036);
	show_inf(pd,i);
	if(ch&~017)
		Set_Color(fon>>4,02);
}
char *getname(),*getgroup();
char *cuserid();
show_inf(pd,i)
register struct dir2 *pd;
int i;
{
	int ch,j,y,x1;
	char mode[10];
	if(!(ch=pd->mode) || wcolumn==16) return;
	y=y_table + i%lines;
	if(wcolumn==26)
	{
		dpsl(pd->size,x0+(i/lines)*width+15,y,8);
		return;
	}
	x1=xsize-47;
	dpsl(pd->size,x1-9,y,8);
	dpp(x1,y);
	dps(getname(pd->uid));
	x1+=9;
	dpp(x1,y);
	x1+=9;
	dps(getgroup(pd->gid));
	dpp(x1, y );
	switch(pd->mode&0170000)
	{
		case S_IFDIR:
			dpo('d');
			break;
		case S_IFCHR:
			dpo('c');
			break;
		case S_IFBLK:
			dpo('b');
			break;
		case S_IFIFO:
			dpo('p');
			break;
#ifdef S_IFLNK
		case S_IFLNK:
			dpo('l');
			break;
#endif
#ifdef S_IFSOCK
		case S_IFSOCK:
			dpo('s');
			break;
#endif
#ifdef S_IFNAM
		case S_IFNAM:
			dpo('n');
			break;
#endif
		default:
			dpo(' ');
	}
	strcpy(mode,"rwxrwxrwx");
	for(j=8;j>=0;j--,ch>>=1)
		if(!(ch&01)) mode[j]='-';

	dps(mode);
	x1+=11;
	dpp(x1 , y);
	ind_date(pd->date);

}

ind_mark(c)
unsigned char c;
{
	setcolor(02+fon);
	if(!c || c<' ') {  dpo(' '); return; }
	setcolor(017+fon);
	if(c)
	{
		if( c=='1' )  c = '*';
	}
	dpo(c);
}


get_tip(pd,full)
struct dir2 *pd;
int full;
{
	int ch,i,lnk=0,len;

	if(fon==0200)
	        ch=02+fon;
	else    ch=012+fon;
	if(!pd->mode) return(ch);
	i=pd->mode;
BEGIN:
	switch(i&0170000)
	{
	case   0170000:
		ch=0x9a|0x100;
		break;
	case S_IFDIR:   /* Справочник   */
		ch=036|0x100;
		break;
	case S_IFCHR:
		ch=07+fon;
		break;
	case S_IFBLK:
		ch=04+fon;
		break;
	case S_IFIFO:
		ch=011+fon;
		break;
#ifdef S_IFNAM
	case S_IFNAM:
		ch=05+fon;
		break;
#endif
#ifdef S_IFLNK
	case S_IFLNK:
	{
		struct stat buf;
		if(stat(pd->d_name,&buf))
			ch=07+fon;
		else
		{
			i=buf.st_mode;
			lnk=1;
			goto BEGIN;
		}
		break;
	}
#endif S_IFLNK
#ifdef S_IFSOCK
	case S_IFSOCK:
		ch=06+fon;
		break;
#endif S_IFSOCK
	default:
		summ_dir+=pd->size;
		if(pd->mode&0100)
			ch=016+fon;
		if(full)
		{
			register int fd;
			unsigned int *wo;
			unsigned char b[20];

			if((fd=open(pd->d_name,O_RDONLY))==-1)
			{
				if(fon==0200)
				{
				        if(ch==(02+fon)) ch=03+fon;
				}
				else
				        if(ch==(012+fon)) ch=03+fon;
				break;
			}
			read(fd,b,20);
			wo=(unsigned int *)b;
			close(fd);
			if(*wo==ARCMAGIC || !memcmp(b,"!<arch>\n",8))
			{
				ch=013+fon;
				return(ch);
			}
			for(fd=0;fd<20;fd++)
				if(b[fd]<' '&&b[fd]!='\n'&&b[fd]!='\r'&&b[fd]!='\t')
				{
					if(ch!=(016+fon))
					{
						if(fon==0200)
						        ch=012+fon;
						else    ch=02+fon;
					}
					break;
				}
			if(fd==20 && ch==(016+fon))
				ch=015+fon;
		}
	}
	if((len=strlen(pd->d_name))>4)
	{
		char *ch1=(char *)strrchr(pd->d_name,'.');
		if(ch1!=NULL)
		{
			if(!strcmp(ch1,".htm") || !strcmp(ch1,".html") || !strcmp(ch1,".php") )
				ch=017+fon;
			if(!strcmp(ch1,".gif") || !strcmp(ch1,".jpg"))
				ch=014+fon;
		}
	}
	if(lnk)
	{
		if(ch==(036|0x100))
			ch=027|0x100;
		else
		{
		        ch&=017;
			ch|=0x100;
		}
	}
	return(ch);
}
s_clear(y)
int y;
{
	int i;
	dpp(0,y);
	for(i=0;i<xsize-1;i++)
		dpo(' ');
	dpp(0,y);
}
extern char *codes;
show(i)
int i;
{
	int j;
	char *c;
	c=codes;
	setcolor(07);
	for(j=0;j<32;j++)
	{
		if(i==j) break;
		while(*c) c++;
		c++;
	}
	for(j=0;j<10 && c[j];j++)
		dpo(c[j]);
	if(j<3)
		dpo(' ');
	setcolor(060);
}
dpsl(val,x,y,l)
long val;
int x,y,l;
{
	char line[21];
	register i;
	if(l>20) l=20;
	strcpy(line,(char *)Conv(val,l,10,' '));
	for(i=0;line[i]==' ';i++);
	dpp( x+i, y);
	dps(line+i);
}
check_mail()
{
	struct stat buf;
	if(stat(Mailfile,&buf) || !buf.st_size)
		return(0);
	dpp(xsize-3-strlen(messages[69])-strlen(messages[71]),0);
	dps(messages[69]);
	dps(messages[71]);
	return(1);
}
