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

		Last modification:Fri Nov  8 09:31:20 2002
			Module:misc.c
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <sys/utsname.h>
#include "tty_codes.h"

extern struct s_tdescr t;
extern int font_H;
extern int font_L;
extern int LL;
extern int HH;
extern int _no_cursor;


static int reper_x,reper_y,reper_h;
static int xx,yy,ll,hh;
static int last_num;

int black_white_mode=0;
static char name[64];
static void ext(int sig)
{
	if(*name)
		unlink(name);
	exit(0);
}



void Chart(int *value,int num,int x,int y,int l,int h,int color,int num_chart)
{
	int i;

	if(t.ct!=15 || num<3)
		return;
	if(font_H)
		HH=font_H;
	if(font_L)
		LL=font_L;
	xx=x*LL;
	yy=y*HH;
	hh=h*HH;
	ll=l*LL;

	if(num_chart==last_num+1)
	{
		DrawLine(color,xx,yy+hh-value[0],xx+ll/(num-1),yy+hh-value[1]);
		for(i=1;i<num;i++)
			DrawVect(xx+i*ll/(num-1),yy+hh-value[i]);
		last_num++;
	}
}
static int _rep=0;
/**** служебная функция ConteXt'а. Больше ее никуда нельзя вставить */
void Reper_Down()
{
	char str[32];
	int color;

	if(t.ct!=15 || !_rep)
		return;

	sprintf(str,"%c[F",27);
	sdps(str);
	scrbufout();
	Del_Last_Line();
	Del_Last_Line();
	_rep=0;
}
void Reper_Up(int x, int y)
{
	if(t.ct!=15)
		return;
	if(_rep)
		Reper_Down();
	reper_x=x;
	reper_y=y;
	DrawLine(7,reper_x,yy+HH/2,reper_x,yy+hh);
	DrawLine(7,xx,reper_y,xx+ll,reper_y);
	_rep=1;
}
void Flush_Screen()
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[b",27);
	sdps(str);
}
int get_Reper_x()
{
	return(reper_x/LL);
}
void DrawRectangle(int color, int x, int y, int l, double h)
{
	char str[64];

	if(t.ct!=15)
		return;
	if(font_H)
		HH=font_H;
	if(font_L)
		LL=font_L;
	h*=HH;
	y*=HH;
	if(h>0)
	{
		y-=(int)h;
		if(y<0)
			y=0;
	}
	else h=-h;
	sprintf(str,"%c[%d;%d;%d;%d;%dt",27,x*LL,y,l*LL,(int)h,color);
	sdps(str);
}
void Del_Rectangles()
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[0;0;0;0;0t",27);
	sdps(str);
}
void DrawLine(int color,int x0,int y0,int x1, int y1)
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[%d;%d;%d;%d;%dx",27,x0,y0,x1,y1,color);
	sdps(str);
}
void DrawVect(int x,int y)
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[%d;%dy",27,x,y);
	sdps(str);
}
void DrawCircle(int x,int y,int l, int h)
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[%d;%d;%d;%dz",27,x,y,l,h);
	sdps(str);
}
void DrawArc(int arc,int color)
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[%d;%dw",27,arc,color);
	sdps(str);
}
void Del_Last_Line()
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[v",27);
	sdps(str);
}
void Del_Chart()
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[0;0;0;0;0x",27);
	sdps(str);
	last_num=0;
	_rep=0;
}
void BlackWhite(int x,int y,int l,int h)
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[%d;%d;%d;%dX",27,x,y,l,h);
	sdps(str);
	black_white_mode=1;
}
void MultiColor(int x,int y,int l,int h)
{
	char str[64];

	if(t.ct!=15)
		return;
	sprintf(str,"%c[%d;%d;%d;%dY",27,x,y,l,h);
	sdps(str);
	black_white_mode=0;
}
void Show_Image(int x, int y, char *name, char *description)
{
	char str[128];

	if(t.ct!=15)
		return;
	if(x<1)
		x=0;
	if(y<1)
		y=0;
	dpp(x,y);
	scrbufout();
	if(description!=NULL && *description)
		sprintf(str,"%c#%s=%s!",27,name,description);
	else
		sprintf(str,"%c#%s!",27,name);
	sdps(str);
}
#ifdef GCC
void Show_Image(int x, int y, char *name)
{
	Show_Image(x,y,name,NULL);
}
#endif
void Del_Image(int x,int y)
{
	char str[128];

	if(t.ct!=15)
		return;
	if(x<0)
		x=0;
	if(y<0)
		y=0;
	sprintf(str,"%c[%d;%d!",27,x,y);
	sdps(str);
}
void Del_All_Images()
{
	char str[16];

	if(t.ct!=15)
		return;
	sprintf(str,"%c#!",27);
	sdps(str);
}
#include <sys/stat.h>
void Frame(char *file)
{
	char str[64];
	int fd,fin;
	long i,size;
	unsigned char *buf;
	char *ch;
	int html=0;
	struct stat st;
	if(t.ct!=15)
	{
		return;
	}

	if((fin=open((char *)file,O_RDONLY))<0)
	{
		return;
	}
	ch=strrchr(file,'.');
	fstat(fin,&st);
	size=st.st_size;
	if(size>1024)
		size=1024;
	if((buf=(unsigned char *)malloc(size))==NULL)
		return;
	read(fin,(char *)buf,size);

	if(ch==NULL || strcmp(ch,".html") || strcmp(ch,".htm"))
	{
		for(i=0;i<size;i++)
		{
			if(buf[i]<' ' && buf[i]!='\n' && buf[i]!='\r' && buf[i]!='\t')
			{
				html=1;
				break;
			}
		}
	}

	sprintf(name,"/tmp/%ld%s",(int)st.st_size,ch==NULL?"":ch);
	if(mkfifo(name,0666))
		return;

	signal(SIGPIPE,ext);
	sprintf(str,"%c#cgi-bin/tmp_read?%s&",27,name+4);
	sdps(str);
	scrbufout();
	fd=open(name,O_WRONLY);
	if(fd<0)
		return;
	ch="<HTML><HEAD><PRE>\n";
	if(!html)
		write(fd,ch,strlen(ch));
	for(;;)
	{
		char *a=NULL;
		int j=0,jsize=0;
		for(i=0;i<size;i++)
		{
			int c=buf[i];
			if(c>=179 && c<=223)
			{
				if(c==179 || c==186)
					buf[i]='|';
				else if(c==196 || c==205)
					buf[i]='-';
				else    buf[i]='+';
				c=' ';
			}

			if(!html)
			{
				if(c>=128 && c<=175)
					c+=1040-128;
				else if(c>=224 && c<=239)
					c+=1088-224;
				if(c>=128 || c=='&' || c=='<' || c=='>')
				{
					int len;
					sprintf(str,"&#%d;",c);
					len=strlen(str);
					a=(char *)realloc(a,jsize+=len);
					bcopy(str,a+j,len);
					j+=len;
					continue;
				}
			}

			a=(char *)realloc(a,++jsize);
			a[j++]=buf[i];
		}
		write(fd,a,jsize);
		free(a);
		if((size=read(fin,buf,size))<=0)
			break;
	}
END:
	close(fin);
	ch="\n</PRE></HEAD></HTML>";
	if(!html)
		write(fd,ch,strlen(ch));
	close(fd);
}
void cursor_visible()
{
	_no_cursor=0;
	if(t.ct==15)
	{
		char str[4];
		sprintf(str,"%c1",27);
		sdps(str);
	}
}
void cursor_invisible()
{
	_no_cursor=1;
	if(t.ct==15)
	{
		char str[4];
		sprintf(str,"%c0",27);
		sdps(str);
	}
}
void New_Color(int i,int r,int g,int b)
{
	if(t.ct==15)
	{
		char str[64];
		sprintf(str,"%c[%d;%d;%d;%dI",27,i,r&0xff,g&0xff,b&0xff);
		sdps(str);
	}
}
void Set_Box_BG(int x,int y,int l,int h,int color)
{
	if(t.ct==15)
	{
		int i,j;
		char str[64];
		for(j=0;j<l;j++)
			for(i=0;i<h;i++)
				put_bg_old(x+j,y+i,color);
		sprintf(str,"%c[%d;%d;%d;%d;%dc",27,x,y,l,h,color);
		sdps(str);
	}
}
void Init_Color()
{
	if(t.ct==15)
	{
		char str[64];
		sprintf(str,"%c[i",27);
		sdps(str);
	}
}
void JMenu(char *menu)
{
	if(t.ct==15)
	{

		char *str=(char *)malloc(strlen(menu)+10);
		scrbufout();
		sprintf(str,"%c#%s|",27,menu);
		sdps(str);
		free(str);
	}
}
void Del_Menu()
{
	if(t.ct==15)
	{
		char str[128];

		scrbufout();
		sprintf(str,"%c#|",27);
		sdps(str);
	}
}
void Set_Title(char *name)
{
	if(t.ct==15)
	{
		char str[128];

		sprintf(str,"%c#%s@",27,name);
		sdps(str);
	}
}
