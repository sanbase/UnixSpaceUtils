/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2002 ConteXt Technology inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (2000-2002)
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

		Last modification:Thu Nov  7 09:27:41 2002
			Module:edit.c
*/
/* редактировать строку line длиной size. */
#include <string.h>
#include <stdlib.h>

char insert=1;
char mask[256];         /*  маска ввода              */
static int mask_flg;
#include "tty_codes.h"
#include <stdio.h>
int full_str;
int cur_poz;
int ssh_flg=0;
extern int x_c,y_c;
extern struct mouse ev;
static int secure;
int dpedit(unsigned char i,char *line,int size,int show,int x,int y,int arg)
{
	char *line_std;
	char *obr="0123456789abcdefABCDEF";
	int poz;
	int maxpoz,shift_poz;

	if(arg<0)
		secure=1;
	else secure=0;
	mask_flg=mask[0]; /* если = 0 - маски нет */
	for(poz=strlen(line);poz<size;poz++)
		line[poz]=0;
	shift_poz=0;
	dpp(x,y); 
	inds(line,show,'_',0);
	for(poz=0;poz<size && line[poz];poz++);
	maxpoz=poz;
	if(*mask)
	        for(poz=0;mask[poz] && mask[poz]!=' ';poz++);
	line[size-1]=0;
	if(poz>show)
		poz=show;
	line_std=(char *)malloc(strlen(line)+1);
	strcpy(line_std,line);
	dpp(x+poz,y);
	if(i) goto SWITCH;
	for(;;)
	{
		i=Xmouse(dpi());
		if(i>=0xfc)
			continue;
SWITCH:
		switch(i)
		{
		case 0:
			if(ev.y!=y || ev.x<x || ev.x>x+show)
				return(0);
			poz=ev.x-x;
			indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
			dpp(x+poz-shift_poz,y);
			break;
		case 2:
			continue;
		case F10:
			strcpy(line,line_std);
			cur_poz=poz;
			free(line_std);
			return(i);
		case cl:        /* - позиция */
CL:
			poz=seek_poz(0,poz,size);
			while(poz-shift_poz<0)
			{
				shift_poz-=show==1?1:show/2;
				indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
			}
			dpp(x+poz-shift_poz,y);
			break;
		case rd:
			poz=strlen(line);
			if(poz>=size) poz=size-1;
			if(maxpoz>show)
			{
				while(poz-shift_poz>=show) shift_poz+=show==1?1:show/2;
				clear_line(x,y,show);
				indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
				break;
			}
			dpp(x+poz-shift_poz,y);
			continue;
		case ru:
			poz=0;
			shift_poz=0;
			indstr(0,line,0,x,y,show);
			dpp(x,y);
			continue;
		case ic:        /* вставка/замена */
			if(mask_flg) break;
			insert=!insert;
			ind_ins(x+poz-shift_poz,y);
			break;
		case Cr:        /* + позиция */
CR:
			poz=seek_poz(1,poz,size);
			while(poz-shift_poz>=show)
			{
				shift_poz+=show==1?1:show/2;
				clear_line(x,y,show);
				indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
			}
			if(poz>=size) poz=size-1;
			dpp(x+poz-shift_poz,y);
			break;
		case dc:        /* удалить символ */
			if(mask_flg)
				goto CL;
			if(poz) poz--;
			else break;
		case de:
			if(poz<maxpoz)
			{
				while(poz-shift_poz<0)
				{
					shift_poz-=show==1?1:show/2;
					indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
				}
				if(!insert)
					line[poz]=' ';
				{
					register int j;
					for(j=poz;j<maxpoz-1;j++)
						line[j]=line[j+1];
					line[maxpoz=j]=0;
				}
DEL:
				clear_line(x,y,show);
				indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
			}
			else
			{
				if(poz==maxpoz)
				{
					line[poz]=0;
					if(poz)
						maxpoz=--poz;
					goto DEL;
				}
			}
			if(poz-shift_poz<0)
			{
				poz++;
				goto CL;
			}
			continue;
		case ta:        /* табуляция */
/*
			poz+=(8-(poz%8));
			if(poz>=size) poz=size-1;
			goto CR;
*/
		case rn:
		{
			int j;

			for(j=strlen(line)-1;j>=0 && line[j]==' ';j--)
				line[j]=0;
			line[size]=0;
			cur_poz=poz;
			free(line_std);
			return(i);
		}
		default:
			if(i<' ')
			{
				cur_poz=poz;
				free(line_std);
				return(i);
			}
			if(poz==size || ((maxpoz==size) && insert))
				continue;
			if(arg>0)
			{
				int j,arg1;
				arg1=arg;
				if(arg=='c' || arg=='C')
				{
					if(i=='*' || i=='/' || i=='%')
						goto OK;
					arg1=10;
				}
				if(i=='-' || i=='+' || i=='.') goto OK;
				for(j=0;j<arg1 && obr[j] && i!=obr[j] ;j++);
				if(j==arg1 || !obr[j])
				{ 
					dpo(bl);  
					break;
				}
			}
OK:
			if(poz<maxpoz && insert )
			{
				register int j;
				if(maxpoz>=size) break;
				for(j=++maxpoz;j>poz&&j;j--)
					line[j]=line[j-1];
				line[poz]=' ';
				indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
			}
			else
			{
				for(;maxpoz<=poz;maxpoz++)
					line[maxpoz]=' ';
			}
			line[poz]=i;
			line[maxpoz]=0;
			poz=seek_poz(1,poz,size);
			if(poz-shift_poz>show)
			{
				shift_poz+=show==1?1:show/2;
				clear_line(x,y,show);
				indstr(0,line+shift_poz,poz-shift_poz,x,y,show);
			}
			else
			{
				if(secure)
					dpo('*');
				else
					dpo(i);
				dpp(x+poz-shift_poz,y);
			}
			if(full_str && poz==size)
			{
				cur_poz=poz;
				free(line_std);
				return(rn);
			}
		}
	}
	free(line_std);
	return(i);
}
extern struct s_tdescr t;
void ind_ins(int x,int y)
{
	unsigned int color;

	dpp(t.param.xdim-2,t.param.ydim-1);
	color=setcolor(-1);
	Set_Color(0,014);
	if(!insert) dpo('-');
	else        dpo('+');
	setcolor(color);
	dpp(x,y);
}

void clear_line(int x,int y,int l)
{
	dpp(x,y);
	dpn(l,'_');
	dpp(x,y);
}
int seek_poz(int znak,int poz,int size)
{
	int poz_std;
	poz_std=poz;
	if(znak)
	{
		if(poz<size) poz++;
		if(poz<size && mask_flg)
		{
			while(mask[poz]!=' ' && poz<size) poz++;
			if(poz==size) poz=seek_poz(0,poz_std,size);
		}
	}
	else
	{
		if(poz) poz--;
		if(mask_flg)
		{
			if(!poz && mask[poz]==' ') return(poz);
			while(mask[poz]!=' ' && poz) poz--;
			if(!poz) poz=seek_poz(1,poz_std,size);
		}
	}
	return(poz);
}
extern int l_y,l_x;
/* показать строку line размером size в позиции (x,y)
  если ind - выделить цветом. После - курсор установить на (x+poz,y) */
void indstr(int ind,char *line,int poz,int x,int y,int size)
{
	if(x+size>l_x) size=l_x-x;
	if(ind) setcolor(ind);
	dpp(x,y);
	inds(line,size,'_',0);
	dpp(x+poz,y);
}
void inds(char *ss,char l,int ch,int color)
{
	register int i,flag;
	unsigned int color_std;
	unsigned char *str=(unsigned char *)ss;
	struct clr clr;
	int x_std=x_c,y_std=y_c;
	struct clr color_to_clr(int);
	color_std=setcolor(-1);
	clr=color_to_clr(color_std);
	if(color)
		setcolor(color);
	for(i=flag=0;i<l;i++)
	{
		if(!str[i])
		{
			Set_Color(clr.bg,clr.fg);
			flag=1;
		}
		if(flag)
		{
			dpo(ch);
			continue;
		}
		if(secure)
			dpo('*');
		else if(str[i]=='\n' || str[i]=='\t') dpo(' ');
		else  if((unsigned char)str[i]<' ') dpo('?');
		else  dpo(str[i]);
	}
	if(get_a2(x_std,y_std))
	{
		put_ch(x_std,y_std,get_ch(x_std,y_std)|0x8000);
		put_ch(x_std+l-1,y_std,get_ch(x_std+l-1,y_std)|0x4000);
	}
	Set_Color(clr.bg,clr.fg);
}
