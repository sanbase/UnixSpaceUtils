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

	Last modification:Fri Mar 23 14:25:33 2001
			Module:menu.c
*/
/* ПРыЖКИ ПО ПОЛЯМ (меню) */

#include <stdio.h>
/* #include <a.out.h> */
#include <ar.h>
#include <fcntl.h>
#include <signal.h>
#include "ssh.h"
#include <pwd.h>
#include <sys/stat.h>
#include "tty_codes.h"
     char *get_uid_name();
extern char new_inf;
extern int act_flg;
extern int x_c,y_c;
extern struct mouse ev;
int no_act;
extern onalrm();
extern char first_time;


int menu( elements, current, lines, width, x0, y0 )
int  elements,*current, lines, width, x0, y0;
{
	register int c;
	int old_pos,mx=0,my=0;

	if( *current<0   ||  *current>=elements  ||  lines>elements ) {
		dpend();  
		exit( 100 );
	}
	old_pos=*current;
BEGIN:
	ind_act();
	for(;;)
	{
		setcolor(0202);
		dpp( x0 + ((*current/lines)*width), y0 + (*current%lines) );
		if(first_time)
		{
			c=hello();
			first_time=0;
			goto SWITCH;
		}
		c=Xmouse(dpi());
SWITCH:
		switch(c)
		{
		case 0:
		{
			int num;
			if(!ev.b)
				continue;
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
				goto retdpt;
			}
			if(ev.x<x0 || ev.x > x0+columns*width)
				continue;
			if(ev.y<y0 || ev.y > y0+lines)
				continue;
			for(num=0;num<elements;num++)
			{
				if(ev.x>=x0 + ((num/lines)*width) &&
				   ev.x<x0+((num/lines+1)*width) &&
				   ev.y==y0+(num%lines))
				{
					ind_file(old_pos);
					*current=num;
					old_pos=*current;
					if(mx==ev.x && my==ev.y)
						goto retdpt;
					mx=ev.x; my=ev.y;
					act_flg=1;
					goto BEGIN;
				}
			}
			continue;
		}
		case cu:
			if( lines<=1 )  goto retdpt;
			if(--*current < 0)  *current += elements;
			break;
		case cd:
			if( lines<=1 )  goto retdpt;
			if(++*current >= elements) *current -= elements;
			break;
		case rd:
			*current=elements-1;
			break;
		case ru:
			*current=0;
			break;
		case cr:
			if( lines==elements )  goto retdpt;
			if((*current += lines)>= elements) {
				*current = (*current+1) % lines;
			}
			break;
		case cl:
			if( lines==elements )  goto retdpt;
			if((*current -= lines) < 0) {
				*current += ((elements+lines-1)/lines)*lines-1;
				if(*current>=elements)  *current -= lines;
			}
			break;
		default:
			goto retdpt;
		}
		if(!act_flg)
		{
			setcolor(07);
			ind_file(old_pos);
		}
		act_flg=1;
		if(no_act==301)
		{
			no_act=0;
			ind_cat();
		}
		no_act=0;
		old_pos=*current;
		alarm( 2 );
	}
retdpt:
	alarm( 2 );
	return( c );
}
char *getname(),*getgroup();
int   wcolumn;
ind_act()
{
	int i;
	register struct dir2 *pd;
	char line[20];

	if(!act_flg || !nfiles)
		return;
	act_flg=0;

	pd=item(current);
	if(!pd->mode)
	{
		struct stat buf;

		if(lstat(pd->d_name,&buf))
			return;
		pd->size=buf.st_size;
		pd->mode=buf.st_mode;
		if((pd->mode&0170000)==S_IFDIR && if_base("",pd->d_name))
			pd->mode=0170000;
		pd->date=buf.st_mtime;
		pd->uid =buf.st_uid;
		pd->gid =buf.st_gid;
		if((buf.st_mode&0170000)==S_IFDIR)
			pd->size=lendir(pd->d_name);
	}
	setpos(current);
	ind_mark(pd->d_mark);
	indnam(pd->d_name,0xe0);
	i=get_tip(pd,1);
	setcolor(07);
	s_clear(ysize-2);
	if(i==0236)
		dps(messages[43]);
	else
	switch(i&0177)
	{
	case 0x1a:
		dps(messages[75]);
		break;
	case 027:
		dps(messages[74]);
		break;
	case 036:
		dps(messages[39]);
		break;
	case 015:
		dps(messages[40]);
		break;
	case 014:
		dps(messages[41]);
		break;
	case 011:
		dps(messages[42]);
		break;
	case 07:
		dps(messages[73]);
		break;
	case 016:
		dps(messages[44]);
		break;
	case 03:
		dps(messages[68]);
		break;
	case 02:
		dps(messages[45]);
		break;
	case 013:
		dps(messages[46]);
		break;
	case 017:
		dps(messages[73]);
		break;
	case 012:
		dps(messages[48]);
		break;
	}
	setcolor(i);
	for(i=0;i<14 && pd->d_name[i];i++)
		dpo(pd->d_name[i]);
	dpp(25,ysize-2);
	setcolor(07);
	dps(messages[49]);
	dps((char *)Conv((long)pd->size,8,10,' '));
	dps(messages[50]);

	dps(getname(pd->uid));

	dpp(55,ysize-2);
	dps(messages[52]);
	dps(getgroup(pd->gid));
	dps(messages[51]);
	dps(Conv((long)pd->mode,3,8,' '));
	check_mail();
	return;
}

/* ПРЫЖКИ ПО ПОЛЯМ (меню)
 *
 * $Log:        dpt.c,v $
 * Revision 1.1  88/09/26  16:17:21  guba
 * Initial revision
 *
 */

int dpt( elements, current, lines, width, x0, y0 )
    int  elements,*current, lines, width, x0, y0;
{
    register int c;
/*    register int current; */

/*    current = *cur; */
    if( *current<0   ||  *current>=elements  ||  lines>elements ) {
	dpend();  exit( 100 );
    }

    for(;;) {
	dpp( x0 + ((*current/lines)*width), y0 + (*current%lines) );
	switch( c=dpi() ) {
	    case cu:
		if( lines<=1 )  goto retdpt;
		if(--*current < 0)  *current += elements;
		break;
	    case cd:
		if( lines<=1 )  goto retdpt;
		if(++*current >= elements) *current -= elements;
		break;
	    case cr:
		if( lines==elements )  goto retdpt;
		if((*current += lines)>= elements) {
		    *current = (*current+1) % lines;
		}
		break;
	    case cl:
		if( lines==elements )  goto retdpt;
		if((*current -= lines) < 0) {
		    *current += ((elements+lines-1)/lines)*lines-1;
		    if(*current>=elements)  *current -= lines;
		}
		break;
	    default:
		goto retdpt;
	}
    }
retdpt:
/*    *cur = current; */
    return( c );
}

#define and &&
#define or  ||
#define not !
extern char insert;
extern int ssh_flg;

int dpr(     buf, lgt, pos, mode )
char    *buf;                           /* редактируемый буфер */
int           lgt;                    /* его длина */
int               *pos;               /* смещение */
unsigned                mode;           /* флаги */
{
	register int c;
	int x,y;
	char line[256];
	x=x_c;y=y_c;
	for(c=lgt;--c>=0;)
		if(buf[c]!=' ') break; /* ищем смысл. конец */
	memcpy(line,buf,c+1);
	line[c+1]=0;
	if(c==lgt-1)
		insert=0;
	else
		insert=1;
	ssh_flg=1;
	c=dpedit(0,line,255,lgt,x,y,0);
	strcpy(buf,line);
	return( c );
}
