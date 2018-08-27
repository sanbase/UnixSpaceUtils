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

	Last modification:Fri Mar 23 14:24:10 2001
			Module:get_tree.c
*/
#include "ssh.h"
#include "tty_codes.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

char *buf_line,*malloc();
static int fdt;
unsigned short line,nline;
extern short l_y;
unsigned long *buf;

get_tree()
{
	register int i;
	struct stat head;
	int c,x,j;
	void *old_sig;
	if((i=open("/tmp/.tree_s",0))==-1) return(-1);
	fstat(i,&head);
	buf=(unsigned long *)malloc(head.st_size);
	read(i,buf,head.st_size);
	close(i);
/*        line=0; */
	nline=head.st_size/(sizeof (long));
	old_sig=signal(SIGINT,SIG_IGN);
BEG:
	dpp(0,0); 
	setcolor(02);
	dpo(es);
	if(fdt) close(fdt);
	if((fdt=open("/tmp/.tree",O_RDONLY))==-1) goto ERR;
	for(i=0,j=line;i<l_y;line++,i++)
		showline(0);
	dpp(0,0);
	line=j;
	showline(1);
	dpp(0,0);
	x=0;
REP:
	while((c=dpi())!=rn)
	{
		showline(0);
		switch(c)
		{
		case rd:
			line=nline-l_y;
			goto BEG;
		case ru:
			line=0;
			goto BEG;
		case il:
			if(line+l_y<nline)
			{
				line+=l_y;
				goto BEG;
			}
			dpp(0,x);
			showline(1);
			dpp(0,x);
			continue;
		case dl:
			if(line-l_y>0) line-=l_y;
			else line=0;
			goto BEG;
		case cd:
			if(line>=nline-1)
			{
				dpp(0,x);
				showline(1);
				dpp(0,x);
				continue;
			}
			x++;
			line++;
			if(x>=l_y)
			{
				dpp(0,0);
				dpo(dl);
				dpp(0,x=l_y-1);
				showline(0);
			}
			dpp(0,x);
			showline(1);
			dpp(0,x);
			continue;
		case cu:
			if(!line)  continue;
			line--;
			x--;
			if(x<0)
			{
				dpp(0,x=0);
				dpo(il);
				showline(1);
			}
			dpp(0,x);
			showline(1);
			dpp(0,x);
			continue;
		case F10:
		case de:
		case 0:
			goto ERR;
		default:
			dpp(0,x);
			showline(1);
			dpp(0,x);
			continue;
		}
	}
	showline();
		if((stat(buf_line,&head)==-1) ||
		((head.st_uid!=getuid())&&((head.st_mode&05)!=05))||
		((head.st_mode&0500)!=0500))
		{
			dpp(0,x);
			goto REP;
		}
	close(fdt);
	free(buf);
	signal(SIGINT,old_sig);
	return(0);
ERR:
	close(fdt);
	free(buf);
#ifdef ALLOC
	sqmem();
#endif
	signal(SIGINT,old_sig);
	return(-1);
}

showline(v)
int v;
{
	register int i,j,clr;
	if(line>=nline)
	{
		line=nline;
		return;
	}
	lseek(fdt,(long)buf[line],0);
	j=read(fdt,buf_line,80);
	if(v)  setcolor(0340);
	for(i=0,clr=010;i<80;i++)
	{
		if(i>=j)
		{
			buf_line[i]=0;
			continue;
		}
		if(!v && buf_line[i]=='/')
			setcolor(clr=++clr>017?011:clr);
		dpo(buf_line[i]);
		if(buf_line[i]=='\n')
			break;
	}
	buf_line[i]=0;
}
