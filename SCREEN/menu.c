/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2000 SanBase inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (1990-2000)
	and M. Flerov (1986-1988).
	E-mail: lashenko@unixspace.com

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

	Last modification:Fri Mar 23 14:16:42 2001
			Module:menu.c
*/
#include "tty_codes.h"
#include <stdio.h>
#include <string.h>
struct ret_xy vert_menu(int l,int h,int size,char *( (*dpms)(int,int)));
extern int l_x,l_y;
static ind_pos(struct ret_xy ret,int act,int size,char *( (*dpms)(int,int)));
static ind_v_pos(struct ret_xy ret,int act,int len,int size,int shift,char *( (*dpms)(int,int)));
int M_FON=060;       /* цвет внутри рамки */
int M_ACT=0340;      /* цвет активного поля */
int M_BORD=07;       /* цвет рамки */
struct ret_xy ret_std;
struct ret_xy ret;
extern struct mouse ev;
extern int x11;

struct ret_xy dpm(int l,int h,int c1,int c2,int c3,char *( (*dpms)(int,int)))
{
	int i,j,size,box,full=0;
	if(c1) M_FON=c1;
	if(c2) M_ACT=c2;
	if(c3) M_BORD=c3;
	box=get_box(0,0,l_x,4);
	dpp(0,0);
	BOX(1,0,l_x-3,3,' ',M_BORD,M_FON);
	setcolor(M_FON);
	size=(l_x-5)/l;
	for(i=j=0;i<l;i++,j+=size)
	{
		char *ch;
		int len;

		ch=(*dpms)(i,0);
		len=strlen(ch);
		if(len>size)
			len=size;
		dpp(j+2+(size-len)/2,1);

		dps((*dpms)(i,0));
	}
/*        ret.x=ret.y=0; */
	ret=ret_std;
	ret.ch=' ';
	if(ret.x>l-1)
		ret.x=l-1;
	if(ret.y)
		goto FULL;
	for(;;)
	{
BEGIN:
		ind_pos(ret,1,size,dpms);
		ret.ch=Xmouse(dpi());
MOUSE:
		if(!ret.ch)
		{
			int x,i,len;
			for(i=0;i<l;i++)
			{
				char *ch;
				ch=(*dpms)(i,0);
				x=i*size+2+(size-(len=strlen(ch)))/2;
				if(ev.x>=x && ev.x<=x+size && ev.y==1)
				{
					if(ret.x!=i)
					{
						ind_pos(ret,0,size,dpms);
						ret.x=i;
						goto BEGIN;
					}
					switch(ev.b)
					{
						case 1:
							ret.ch=rn;
							break;
						case 3:
							ret.ch=lf;
							break;
						default:
							ret.ch=F10;
					}
				}
			}
		}
BEG:
		ind_pos(ret,0,size,dpms);
		ret.y=0;
		switch(ret.ch)
		{
		case cl:
			if(ret.x)
				ret.x--;
			else    ret.x=l-1;
			if(full) goto FULL;
			break;
		case cr:
			if(ret.x<l-1)
				ret.x++;
			else    ret.x=0;
			if(full) goto FULL;
			break;
		case rn:
		case lf:
		case F1:
		case 1:
FULL:
			if(h<2 || ret.ch==F1 || ret.ch==1)
				goto END;
			if(!*(*dpms)(ret.x,1))
			{
				if(ret.ch==rn)
					goto END;
				break;
			}
			ind_pos(ret,1,size,dpms);
			full=1;
			ret=vert_menu(l,h,size,dpms);
			ind_pos(ret,0,size,dpms);

			if(x11 && ret.ch==0)
				goto MOUSE;
			if(ret.ch==rn || ret.ch==F1)
				goto END;
			if(ret.ch==F10 || ret.ch==0)
			{
				full=0;
				break;
			}
			if(ret.ch==cr)
			{
				if(ret.x<l-1)
					ret.x++;
				else    ret.x=0;
			}
			else
				if(ret.ch==cl)
				{
					if(ret.x)
						ret.x--;
					else    ret.x=l-1;
				}
			ret.ch=lf;
			goto BEG;
		case F10:
		case 0:
			ret.x=ret.y=0;
			goto END;
		}
	}
END:
	put_box(0,0,box);
	free_box(box);
	return(ret);
}
struct ret_xy vert_menu(int l,int h,int size,char *( (*dpms)(int,int)))
{
	int len=0,shift;
	register int i,j;

	for(i=1;i<h;i++)
	{
		if(!*(*dpms)(ret.x,i))
			break;
		if((j=strlen((*dpms)(ret.x,i)))>len)
			len=j;
	}
	if(!len)
		return(ret);
	h=i;
	if(h==1)
	{
		ret.ch=1;
		return(ret);
	}
	if(h>l_y-4) h=l_y-4;

	shift=(i=l_x-ret.x*size-3-len)<0?i:1;
	j=get_box(ret.x*size+shift,3,len+4,h+2);
	BOX(ret.x*size+shift,3,len+2,h+1,' ',M_BORD,M_FON);
	{
		struct ret_xy ret_tmp;

		ret_tmp=ret;
		for(ret_tmp.y=1;ret_tmp.y<h;ret_tmp.y++)
			ind_v_pos(ret_tmp,0,len,size,shift,dpms);
	}
/*        ret.y=1; */
	if(ret.y>=h-1 || ret.y<1)
		ret.y=1;
BEG:
	ind_v_pos(ret,1,len,size,shift,dpms);
		ret.ch=Xmouse(dpi());
		if(!ret.ch)
		{
			int x,y,i;
			for(i=0;i<h;i++)
			{
				x=ret.x*size+shift+1; y=3+i;
				if(ev.x>=x && ev.x<=x+len && ev.y==y)
				{
					if(ret.y!=i)
					{
						ind_v_pos(ret,0,len,size,shift,dpms);
						ret.y=i;
						goto BEG;
					}
					switch(ev.b)
					{
						case 1:
							ret.ch=rn;
							break;
						case 3:
							ret.ch=lf;
							break;
						default:
							ret.ch=F10;
					}
					goto SWITCH;
				}
			}
/*                        if(ev.y>2 && ev.y<3+h) */
			if(ev.y>2)
			{
				if(ev.x>ret.x*size+shift+len)
					ret.ch=cr;
				else
				if(ev.x<ret.x*size+shift)
					ret.ch=cl;
				else    ret.ch=F10;
			}
			if(ev.y==1)
			{
				put_box(ret.x*size+shift,3,j);
				free_box(j);
				ret.ch=0;
				return(ret);
			}
		}
SWITCH:
	ind_v_pos(ret,0,len,size,shift,dpms);
	switch(ret.ch)
	{
		case cd:
			if(ret.y<h-1)
				ret.y++;
			else    ret.y=1;
			goto BEG;
		case cu:
			if(ret.y>1)
				ret.y--;
			else    ret.y=h-1;
			goto BEG;
		default:
			if(ret.ch<' ')
				break;
			goto BEG;
	}
	put_box(ret.x*size+shift,3,j);
	free_box(j);
	return(ret);
}

static ind_pos(struct ret_xy ret,int act,int size,char *( (*dpms)(int,int)))
{
	char str[256];
	int len;
	register char *ch;

	ch=(*dpms)(ret.x,0);
	len=strlen(ch);
	if(len>size)
		len=size;
	memset(str,0x20,(size-len)/2);
	strncpy(str+(size-len)/2,ch,size);
	dpp(ret.x*size+2,1);
	inds(str,size,' ',act?M_ACT:M_FON);
	if(act)
	{
		dpp(ret.x*size+2,1);
		setcolor(M_FON);
		dpn((size-len)/2,' ');
	}
	dpp(ret.x*size+2+(size-len)/2,1);
}
static ind_v_pos(struct ret_xy ret,int act,int len,int size,int shift,char *( (*dpms)(int,int)))
{
	register char *ch;

	ch=(*dpms)(ret.x,ret.y);
	if(act)
		setcolor(M_ACT);
	else    setcolor(M_FON);
	dpp(ret.x*size+shift+1,3+ret.y);
	inds(ch,len,' ',0);
	dpp(ret.x*size+shift+1,3+ret.y);
	setcolor(M_FON);
}
