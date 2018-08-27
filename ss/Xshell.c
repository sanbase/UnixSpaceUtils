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

	Last modification:Fri Mar 23 14:23:06 2001
			Module:Xshell.c
*/
#include <tty_codes.h>
#include "ssh.h"
#include <stdio.h>

static char *commands[16];
extern char insert;
#define LINESIZE 256
extern int x_c,y_c;
extern int M_FON;      /* цвет внутри рамки */
extern int M_ACT;      /* цвет активного поля */
extern int M_BORD;     /* цвет рамки */

shell()
{
	static  char str[LINESIZE+1];
	int c;

	*str = 0;
in: 
	c = command(str);
	if(c==F8)
	{
		*str=0;
		goto in;
	}
	if( c == Esc || c==F10 )
		return(0);
	if(      c == spec('&') )  c = '&';
	else if( c == spec('6') )  c = '&';
	else if( c == s_refuse  )
		return(0);
	else if( c == s_help || c==F1 || c==dc )
	{
		help("ssh",'!');  
		return(0);
	}
	else if( c != rn        )  goto in;

	if( *str == 0 )
		return(0);
	else
	{
		c=start( str, c, YES );
		dpp(0,ysize-1); dpn(xsize-1,'_'); scrbufout();
		return(c);
	}
}

static int current_cmd;
struct ret_xy ret;
struct ret_xy vert_menu();
char *dpms(x,y)
int x,y;
{

	if(y<=0 || y>21 || !commands[y-1])
		return("");
	return(commands[y-1]);
}

command(char *str)
{
	int x,y,l,a,i,rets=0;

	l=xsize/2-3;
	x=3;
	y=ysize-5;
	*str=0;
	help_line(messages[76]);
RET:
	if(strlen(str)>l)
		l=strlen(str);
	if(l>xsize-4)
		l=xsize-4;
	BOX(x,y,l,3,' ',0x402e,0x442e);
	setcolor(0x442e);
	dpp(x+2,y+1);
	dpn(l-4,'_');
	dpp(x+2,y+1);
	for(i=0;i<l-3 && str[i];i++)
		dpo(str[i]);
	dpp(x+2,y+1);
	switch(i=Xmouse(dpi()))
	{
		case cu:
		case cd:
			if(i==cu)
			{
				rets=1;
				if(--current_cmd<0)
				{
					current_cmd=15;
					while(current_cmd&&!commands[current_cmd])
						current_cmd--;
					if(!current_cmd)
						goto RET;
				}
			}
			else
			{
				rets=1;
				if(++current_cmd>15 || !commands[current_cmd])
				{
					current_cmd=0;
					if(!commands[current_cmd])
						goto RET;
				}
			}
P1:
			strcpy(str,commands[current_cmd]);
			goto RET;
		case F7:
		{
			int i;
			for(i=0;i<16;i++)
				if(commands[i])
					break;
			if(i==16)
				goto RET;
			M_ACT=0x57f;
			M_FON=0x442e;
			M_BORD=0x402e;
			ret.x=1;
			ret.y=1;
			ret=vert_menu(1,19,x>3?0:xsize,dpms);
			current_cmd=ret.y-1;
			goto P1;
		}
	        case 0:
			return(0);
	}
EDT:

	a=edit(i,str,LINESIZE-1,l-4,x+2,y+1,0);

	if(*str && a==rn)
	{
		if(!rets)
		{
			for(i=0;i<16 && commands[i];i++)
				if(!strcmp(commands[i],str))
					goto NEXT;
			if(i<16)
				current_cmd=i;
		}
		commands[current_cmd]=(char *)realloc(commands[current_cmd],strlen(str)+1);
		strcpy(commands[current_cmd],str);
		++current_cmd;
		current_cmd%=16;
	}
NEXT:
	help_line(messages[4]);
	return(a);
}
