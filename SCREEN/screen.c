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

		Last modification:Fri Nov  8 09:24:29 2002
			Module:screen.c
*/
#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include "tty_codes.h"
#include <stdlib.h>

extern struct s_tdescr t;

static union fpic *Screen;               /* видеопамять */
static union fpic *scrold;               /* видеопамять */
int *X_C,*Y_C,*C_C;

extern int  l_y,l_x;
extern int  x_c,y_c;                   /* координаты курсора */
extern unsigned int S_color;

#undef c
#undef s
#undef e
extern int x_p,y_p;           /* координаты курсора физические  */
extern int x11;

void ini_screen()
{
	int l=(t.param.xdim+1);
	int h=(t.param.ydim+1);
	if((Screen=(union fpic *)calloc(l*h,sizeof (union fpic)))==NULL)
	{
		Ttyreset();
		exit(1);
	}
	memset((char *)Screen,0,l*h*sizeof (union fpic));
	if((scrold=(union fpic *)calloc(l*h,sizeof (union fpic)))==NULL)
	{
		Ttyreset();
		exit(1);
	}
	X_C=&x_c;
	Y_C=&y_c;
	C_C=(int *)&S_color;
	zero_scrold();
}
int get_XC()
{
	return(*X_C);
}
int get_YC()
{
	return(*Y_C);
}
int get_CC()
{
	return(*C_C);
}

void erase_area(int x,int y,int l,int h)
{
	int i,j;
	union fpic f;

	f.p.ch=0xff;
	f.p.clr=color_to_clr(S_color);
	for(j=y;j<y+h;j++)
		for(i=x;i<x+l;i++)
			put_f_old(i,j,f);
}
void zero_scrold()
{
	memset((char *)scrold,0,t.param.xdim*t.param.ydim*sizeof (union fpic));
}
void zero_box(int x,int y,int l,int h)
{
	int i,j;
	union fpic f;

	f.f=0;
	f.p.ch=0xff;
	for(j=y;j<y+h;j++)
		for(i=x;i<x+l;i++)
			put_f_old(i,j,f);
}
void get_scrold(char *buf)
{
	int len=t.param.xdim*t.param.ydim*sizeof (union fpic);
	bcopy((char *)scrold,buf,len);
	buf[len]=x_p;
	buf[len+1]=y_p;
}
void clear_scr_old()
{
	register int x,y;
	union fpic cc;

	cc.p.ch=0xff;
	cc.p.clr.fg=cc.p.clr.bg=0xff;
	for(y=0;y<t.param.ydim;y++)
		for(x=0;x<t.param.xdim;x++)
			put_f_old(x,y,cc);
}

void put_scrold(char *buf)
{
	int len=t.param.xdim*t.param.ydim*sizeof (union fpic);
	bcopy(buf,(char *)scrold,len);
	x_p=buf[len];
	y_p=buf[len+1];
}
void free_screen()
{
	free(Screen);
	free(scrold);
}
/* взять символ из видеопамяти */
unsigned short get_ch(int x,int y)
{
	if(x>l_x || y>l_y ) return(0);
	return(Screen[y*t.param.xdim+x].p.ch);
}

/* поместить символ в видеопамять */
int put_ch(int x,int y,unsigned short ch)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].p.ch=ch;
	return(0);
}
unsigned int get_fg(int x,int y)
{
	if(x>l_x || y>l_y ) return(0);
	return(Screen[y*t.param.xdim+x].p.clr.fg);
}
unsigned int get_bg(int x,int y)
{
	if(x>l_x || y>l_y ) return(0);
	return(Screen[y*t.param.xdim+x].p.clr.bg);
}
unsigned int get_a1(int x,int y)
{
	if(x>l_x || y>l_y ) return(0);
	return(Screen[y*t.param.xdim+x].p.clr.atr1);
}
unsigned int get_a2(int x,int y)
{
	if(x>l_x || y>l_y ) return(0);
	return(Screen[y*t.param.xdim+x].p.clr.atr2);
}
unsigned int get_clr(int x,int y)
{
	if(x>l_x || y>l_y ) return(0);
	return(clr_to_col(Screen[y*t.param.xdim+x].p.clr));
}
struct clr get_clrn(int x,int y)
{
	if(x>l_x || y>l_y )
	{
	        struct clr clr;
		clr.fg=0;clr.bg=0;clr.atr1=0;clr.atr2=0;
	        return(clr);
	}
	return(Screen[y*t.param.xdim+x].p.clr);
}
int put_fg(int x,int y,int color)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].p.clr.fg=color;
	return(0);
}
int put_fg_old(int x,int y,int color)
{
	if(x>l_x || y>l_y ) return(-1);
	scrold[y*t.param.xdim+x].p.clr.fg=color;
	return(0);
}
int put_bg(int x,int y,int color)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].p.clr.bg=color;
	return(0);
}
int put_bg_old(int x,int y,int color)
{
	if(x>l_x || y>l_y ) return(-1);
	scrold[y*t.param.xdim+x].p.clr.bg=color;
	return(0);
}
int put_a1(int x,int y,int atr1)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].p.clr.atr1=atr1&03;
	return(0);
}
int put_a1_old(int x,int y,int atr1)
{
	if(x>l_x || y>l_y ) return(-1);
	scrold[y*t.param.xdim+x].p.clr.atr1=atr1&03;
	return(0);
}
int put_a2(int x,int y,int atr2)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].p.clr.atr2=atr2&03;
	return(0);
}
struct clr color_to_clr(int);
int put_clr(int x,int y,int color)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].p.clr=color_to_clr(color);
	return(0);
}
union fpic get_f(int x,int y)
{
	if(x>l_x || y>l_y )
	{
		union fpic f;
		f.f=0;
	        return(f);
	}
	return(Screen[y*t.param.xdim+x]);
}
int put_f(int x,int y,union fpic f)
{
	if(x>l_x || y>l_y ) return(-1);
	Screen[y*t.param.xdim+x].f=f.f;
	return(0);
}
int put_f_old(int x,int y,union fpic f)
{
	if(x>l_x || y>l_y ) return(-1);
	scrold[y*t.param.xdim+x].f=f.f;
	return(0);
}
void cp_screen()
{
	bcopy(Screen,scrold,t.param.xdim*t.param.ydim*sizeof (union fpic));
}
void cp_screenold()
{
	bcopy(scrold,Screen,t.param.xdim*t.param.ydim*sizeof (union fpic));
}

#ifdef X11R6
void Xput_box(int x,int y,int l,struct clr clr);
void X_g_line(int x,int y,struct clr clr,int arg,int l);
void Xdps(unsigned char *str,struct clr clr,int x,int y,int n);
void Xdpo_ch(unsigned char ch,struct clr clr,int x,int y);
void res_Cur();

static void Draw_Area(union fpic *begin,union fpic *a,union fpic *n,int y)
{
	int x_t,x,i;
	struct clr clr;
	unsigned char str[256];

	Xput_box(x_t=x=begin-n,y,i=a-begin,begin->p.clr);

	if(i>0 && begin->p.clr.atr2)
	{
		X_g_line(x,y,begin->p.clr,1,i);
		if(begin->p.ch&0x200)
			X_g_line(x,y,begin->p.clr,2,0);
		if((a-1)->p.ch&0x100)
			X_g_line(x+i-1,y,begin->p.clr,0,0);
	}
	else if(begin->p.ch&0x200)
		X_g_line(x,y,begin->p.clr,2,0);

	clr=begin->p.clr;
	for(i=0;i<256 && begin<a;begin++,x_t++)
	{
		unsigned char ch=begin->p.ch&0xff;

		if((ch>175 && ch<224) && !(ch==176 || ch==177 || ch==178 || ch==196 || ch==205 || ch==179 || ch==186))
		{
			if(i)
			{
				Xdps(str,clr,x,y,i);
				x+=strlen((char *)str);
				i=0;
			}
			Xdpo_ch(ch,begin->p.clr,x,y);
			x++;
			clr=(begin+1)->p.clr;
			goto END;
		}
		if(ch==' ' || (clr.fg)==(begin->p.clr.fg))
		{
			str[i++]=ch;
			str[i]=0;
		}
		else
		{
			if(i)
			{
				Xdps(str,clr,x,y,i);
				x=x_t;
				clr=begin->p.clr;
				i=0;
				begin--;
				x_t--;
				continue;
			}
			x++;
		}
END:
		scrold[y*t.param.xdim+x_t].f=begin->f;
	}
	if(i)
		Xdps(str,clr,x,y,i);
}
void Xscreen_out()
{
	int y,p;
	int x_s,y_s;

	x_s=x_c;
	y_s=y_c;

	res_Cur();

	for(p=0;;)
	{
		register union fpic *a,*b;
		union fpic *begin,*l,*n;

		y=t.param.ydim/2+p;
		if(p>=0) p++;
		p=-p;
		if(p>t.param.ydim/2+1)
			break;
		if(y<0 || y>=t.param.ydim)
			continue;

		b=&scrold[y*t.param.xdim];
		n=&Screen[y*t.param.xdim];
		l=&Screen[(y+1)*t.param.xdim];
		for(a=begin=n;a<l;a++,b++)
		{

			if(a->f != b->f)
			{
				if(a->p.ch==b->p.ch)
				{
					if(begin!=NULL && a->p.ch==' ' && !a->p.clr.bg && !begin->p.clr.bg && (begin->p.clr.atr2==a->p.clr.atr2))
						continue;
				}
				if(begin!=NULL &&
				((begin->p.clr.bg)!=(a->p.clr.bg)    ||
				 (begin->p.clr.atr2)!=(a->p.clr.atr2)||
				 (begin->p.clr.atr1)!=(a->p.clr.atr1)))
				{
					Draw_Area(begin,a,n,y);
					begin=a;
				}
				else if(begin==NULL)
					begin=a;
			}
			else if(begin!=NULL)
			{
				Draw_Area(begin,a,n,y);
				begin=NULL;
			}
		}
		if(begin!=NULL)
		{
			Draw_Area(begin,a,n,y);
			begin=NULL;
		}
	}
}
#endif

void screen_out()
{
	register union fpic *a;
	int y;
	int x_s,y_s,c_s;

#ifdef X11R6
	if(x11)
	{
		Xscreen_out();
		return;
	}
#endif
	x_s=x_c;
	y_s=y_c;
	c_s=S_color;

	if(t.ct==12)
		s_dps("\E[?25l");
	for(y=0;y<t.param.ydim;y++)
	{
		register union fpic *b,*l,*n;

		b=&scrold[y*t.param.xdim];
		n=&Screen[y*t.param.xdim];
		l=&Screen[(y+1)*t.param.xdim];

		for(a=n;a<l;a++,b++)
		{
			if(a->f != b->f)        /* что-то изменилось */
			{
				if(a->p.ch==b->p.ch && a->p.ch==' '&& b->p.clr.atr2==a->p.clr.atr2)
				{
					if(a->p.clr.bg==b->p.clr.bg)
						continue;
				}
				if(y==t.param.ydim-1 && a-n==t.param.xdim-1)
					break;
				s_dpp(a-n,y);
				s_setcolor(clr_to_col(a->p.clr));
				a_dpo(a->p.ch);
				b->f=a->f;
			}
		}
	}
	s_dpp(x_s,y_s);
	s_setcolor(c_s);
	if(t.ct==12)
		s_dps("\E[?25h");
	*X_C=x_c;
	*Y_C=y_c;
	*C_C=S_color;
}
void a_dpo(unsigned short c )
{
	c &= 0377;
	if(y_c>t.param.ydim-1)
	{
		scroll_up(0);
		y_c=t.param.ydim-1;
	}
	s_dpo(c);
}
void dpo_scr(unsigned short c)
{
	if(c>=040)
	{
		struct clr clr;
		put_ch(x_c,y_c,c);
		put_clr(x_c,y_c,S_color);
		return;
	}
	switch(c)
	{
	case 255:
	case 0:
		break;
	case ic: /* вставка символа */
		{
			register int i;
			for(i=l_x;i>x_c;i--)
				put_f(i,y_c,get_f(y_c,i-1));
			put_ch(y_c,x_c,' ');
		}
		break;
	case dc: /* удаление символа */
		{
			register int i;
			for(i=x_c;i<l_x;i++)
				put_f(i,y_c,get_f(y_c,i+1));
			put_ch(l_x,y_c,' ');
		}
		break;
	case il: /* вставка строки */
		{
			register int i;
			union fpic f;

			for(i=l_y;i>y_c;i--)
			{
				bcopy(&Screen[(i-1)*t.param.xdim],&Screen[i*t.param.xdim],(t.param.xdim)*sizeof(union fpic));
				if(!x11)
					bcopy(&scrold[(i-1)*t.param.xdim],&scrold[i*t.param.xdim],(t.param.xdim)*sizeof(union fpic));
			}
			if(x11)
				f.p.ch=' ';
			else
				f.p.ch=0;
			f.p.clr=color_to_clr(S_color);
			for(i=x_c;i<=l_x;i++)
			{
				f.p.clr=get_clrn(i,y_c);
				put_f(i,y_c,f);
			}
			f.p.ch=255;
			f.p.clr.bg=0;
			f.p.clr.fg=0;
			for(i=x_c;i<=l_x;i++)
				put_f_old(i,y_c,f);

		}
		break;
	case dl: /* удаление строки */
		scroll_up(y_c);
		break;
	case es: /* стирание до конца экрана */
		screen_cl_end(x_c,y_c);
		break;
	case el: /* стирание справа от курсора */
		{
			register int i;
			union fpic cc;

			cc.p.ch=' ';
			cc.p.clr=color_to_clr(S_color);
			for(i=x_c;i<=l_x;i++)
				put_f(i,y_c,cc);
		}
		break;
	default:
		s_dpo(c);
		break;
	}
}
void screen_clear()
{
	screen_cl_end(0,0);
}
void clear_scr(int col)
{
	register int x,y;
	union fpic cc;

	cc.p.ch=' ';
	cc.p.clr=color_to_clr(col);
	for(y=0;y<t.param.ydim;y++)
		for(x=0;x<t.param.xdim;x++)
		{
			put_f(x,y,cc);
			put_f_old(x,y,cc);
		}
}
void scroll_up(int y)
{
	register int i;
	union fpic cc;

	for(i=y;i<l_y;i++)
	{
		bcopy(&Screen[(i+1)*t.param.xdim],&Screen[i*t.param.xdim],(t.param.xdim)*sizeof (union fpic));
//                if(!x11)
//                        bcopy(&scrold[(i+1)*t.param.xdim],&scrold[i*t.param.xdim],(t.param.xdim)*sizeof (union fpic));
	}

	cc.p.ch=' ';
	cc.p.clr=color_to_clr(S_color);
	for(i=0;i<=l_x;i++)
	{
		cc.p.clr=get_clrn(i,l_y);
		put_f(i,l_y,cc);
	}
	cc.p.ch=255;
	cc.p.clr.bg=0;
	cc.p.clr.fg=0;
	for(i=0;i<=l_x;i++)
		put_f_old(i,l_y,cc);

}
void screen_cl_end(int x,int y)
{
	register int i,j=x;
	union fpic cc,cc_z;
	int erase=0;

	Del_Chart();
	bzero(&cc_z,sizeof cc_z);
	cc.p.ch=' ';
	cc.p.clr=color_to_clr(S_color);

	if(t.ct==15 && x==0 && y==0)
	{
		char str[16];
		sprintf(str,"%c[1m",27);
		s_setcolor(S_color);
		s_dps(str);
		erase=1;
	}

	for(i=y;i<=l_y;i++)
	{
		for(;j<=l_x;j++)
		{
			put_f(j,i,cc);
			if(erase)
				put_f_old(j,i,cc);
			else
				put_f_old(j,i,cc_z);
/*
			if(!x && y)
				put_f_old(j,i,cc);
*/
		}
		j=0;
	}
	dpp(x,y);
}
