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

	Last modification:Fri Mar 23 14:15:45 2001
			Module:dos.c
*/
#include <signal.h>
#include <conio.h> 
#include <stdio.h>
#include "tty_descr.h"

char PC;
char *BC;
char *UP;
int ospeed;

int l_y,l_x;
int rus_lat;
int rus_lat_out;

int x_c=0,y_c=0;                 /* координаты курсора виртуальные */
int x_p=-100,y_p=-100;           /* координаты курсора физические  */

int S_color=0;              /* цвет виртуальный  */
static int _p_color=0;       /* цвет физический   */

static sdps();
static char _buf[1025];           /* общий буфер */

struct s_tdescr t;

#undef c
#undef s
#undef e

static char   *buf  = _buf;    /* буфер для вывода      */
static int  lbuf  = 1024;    /* размер буфера вывода  */
static int  xbuf;            /* индекс записи в буфер */


/* ИНИЦИАЛИЗАЦИЯ */

char cbuf[300];

/* 32 входных кода */
char *cvtin[32];
char *cvtin_[]=
{
/*       ^'@    ^'a   ^'b      F12    F3     fl  s_finish  bl  */
/*        dc     ta     lf     F1     F2     rn     F4     F5  */
/*        F6     F7     rd     ru     cd     cu     cr     cl  */
/*        ic   s_help   il     dl     F8     F9     F10    F11 */

	   0,     0,     0,  "kB",  "k3",     0,     0,     0,
	   0,     0,     0,  "k1",  "k2",     0,  "k4",  "k5",
	"k6",  "k7",  "EN",  "HM",  "kd",  "ku",  "kr",  "kl",
	"IN",  "kC",  "PD",  "PU",  "k8",  "k9",  "k0",  "kA"
};

/* 16 выходных */
char *cvtout[16];
char *cvtout_[]=
{
/*         -     el     rd     ru     cd     cu     cr     cl */
/*        ic     dc     il     dl     es     -      se     so */

	 "cm",  "ce",  "sr",  "sf",  "kd",  "ku",  "kr",   "kl",
	 "ic",  "dc",  "al",  "dl",  "cd",  "Cf",   "se",   "so"
};

char *Mf,*Mb;
int Nf,Nb;
char *tgetstr(), *tgoto(), *getenv();
static am;
int dpbeg()
{
	rus_lat=rus_lat_out=0; 
	xbuf=0; 
 
	window(1,1,80,25); 
	t.param.xdim=80; 
	t.param.ydim=25; 
	l_x=79; l_y=24; 
	t.ct=2;
	s_color=-1; 
	rus_lat=rus_lat_out=0; 
	ini_screen();
	{
		register x,y;
		for(y=0;y<t.param.ydim;y++)
			for(x=0;x<t.param.xdim;x++)
				put_f(x,y,0);
		cp_screen();
	}

	return(1);
bend:
	exit(0);

}
dpinit()
{
	dpp(0,0);
	s_dpp(0,0);
	setcolor(03);
	s_setcolor(03);
	dpo(es);
}


/* ПЕРЕКЛЮЧЕНИЕ В ОБЫЧНЫЙ РЕЖИМ */

int dpend()
{
	s_dpp( 0, t.param.ydim-2 );
	s_setcolor(03);
	s_dpo(es);
	sdps(t.end);
	scrbufout();
	free_screen();
	Ttyreset();
}


/* СООБЩАЕТ ПАРАМЕТРы ТЕРМИНАЛА */

struct s_dpd dpd() {
	return( t.param );
}

/* ВЫВОД ОДНОГО СИМВОЛА */

int dpo( c )
int c;
{
	switch( c &= 0377)
	{

		case lf: dpo(rn);
			return;
		case ta: x_c=x_c+8-x_c%8;
			goto END;
		case cr: if(x_c<l_x ) x_c++;
			goto END;
		case cl: if(x_c) x_c--;
			goto END;
		case cd: if(y_c<l_y) y_c++;
			goto END;
		case cu: if(y_c) y_c--;
			goto END;
		default:
			break;
	}
	if(c==0x9b) c=0xeb;
	if(c==ic || c==dc || c==il || c==dl || c==es || c==el)
	{
		screen_out();

		dpo_scr(c);

		cp_screen();
		s_dpp(x_c,y_c);
		s_dpo(c);
		return;
	}
	if(!t.ct && c>0xaf && c<0xe0)
	{
		if(c==0xb3 || c==0xba) c='|';
		else
		if(c==0xc4 || c==0xcd) c='-';
		else
		if(c>=0xb0 && c<=0xb2) c=' ';
		else    c='+';
	}
	dpo_scr(c);
	if(c>=' ')  x_c++;
END:
	if(c==rn)
	{ 
		x_c=0; 
		y_c++;
	}
	if(y_c>l_y)
	{
		scroll_up(0);
		y_c=l_y;
	}
}
s_dpo(i)
int i;
{
	char *c;
	if(i>16 && i<32)
	{
		if(i==cr && x_p<l_x ) x_p++;
		if(i==cd && y_p<l_y ) y_p++;
		if(i==cl && x_p ) x_p--;
		if(i==cu && y_p ) y_p--;
		switch(i)
		{ 
			case cu: 
			case cd: 
			case cl: 
			case cr: 
			case ta: 
			case rn: 
			case lf: 
				s_dpp(x_c,y_c);
				return; 
			case il: 
				insline(); 
				return; 
			case dl: 
				delline(); 
				return; 
			case es: 
				clrscr(); 
				return; 
			case el: 
				clreol(); 
				return; 
			default: 
				putch(c); 
				return; 
		} 
		return;
	}
	sdpo(i);
	if(i>=' ')
	{
		x_c=++x_p;
		if(am && x_c>l_x)
		{
			y_p=++y_c;
			x_p=x_c=0;
		}
	}
}

int dpin()
{
	register int c, i;
	switch(bioskey(2))
	{
		case 18989:
			c=F1;
			goto END;
		case 15360:
			c=F2;
			goto END;
		case 15616:
			c=F3;
			goto END;
		case 15872:
			c=F4;
			goto END;
		case 16128:
			c=F5;
			goto END;
		case 16384:
			c=F6;
			goto END;
		case 16640:
			c=F7;
			goto END;
		case 16896:
			c=F8;
			goto END;
		case 17152:
			c=F9;
			goto END;
		case 17408:
			c=F10;
			goto END;
		case 18989:
			c=F11;
			goto END;
		case 20011:
			c=F12;
			goto END;
		case 20992:
			c=ic;
			goto END;
		case 21248:
			c=dc;
			goto END;
		case 18176:
			c=ru;
			goto END;
		case 20224:
			c=rd;
			goto END;
		case 18088:
			c=il;
			goto END;
		case 20736:
			c=dl;
			goto END;
		case 18432:
			c=cu;
			goto END;
		case 20480:
			c=cd;
			goto END;
		case 19712:
			c=cr;
			goto END;
		case 19200:
			c=cl;
			goto END;
		default:
			return(getch());
	}
END:
	bioskey(0);
OK: 
	return( c ); 
} 
difstr(a,b)
unsigned char *a,*b;    /* a - образец, b - строка */
{
	register int i;
	for(i=0;a[i] && b[i];i++)
		if(a[i]!=b[i]) break;
	if(!a[i] && !b[i]) return(0);  /* строки равны */
	if(!b[i]) return(1);           /* образац еще не кончился */
	return(-1);
}


int dpi()
{
	scrbufout();
	{
		register int c;
		c=dpin();
		if(rus_lat) c=rus(c);
		return(c);
	}
}


/* ПРОВЕРКА, БЫЛ ЛИ НАЖАТ СИМВОЛ */

int dpa()
{
	scrbufout();
	return( empty(0) ? no_symb : dpi() );
}


/* УСТАНОВ КУРСОРА В ПОЗИЦИЮ (x,y) */
dpp(x,y)
int  x, y;
{
	if(x>=t.param.xdim) x=l_x;
	if(y>=t.param.ydim) y=l_y;
	x_c=x;
	y_c=y;
}
flush_dpi()
{
	x_p=-100;
	y_p=-100;
	_p_color=0;
}
s_dpp( x, y )
int  x, y;
{
	register char *c;
	if(x>=t.param.xdim) x=l_x;
	if(y>=t.param.ydim) y=l_y;
	if(y==y_p)
	{
		int l;
		l=x-x_p;
		if(l>0 && l<6)
		{
			int i;
			for(i=0;i<l;i++)
			{
/*                                if(get_clr(x_p,y)==S_color || get_ch(x_p,y)==' ' && get_clr(x_p,y)<020 && S_color<020) */
				if(get_clr(x_p,y)==S_color)
					s_dpo(get_ch(x_p,y));
				else break;
			}
		}
	}
	if(x==x_p-1)
		s_dpo(cl);
	else
		if(x==x_p+1)
		{
			if(get_clr(x_p,y)==S_color)
				s_dpo(get_ch(x_p,y));
			else
				s_dpo(cr);
		}

	if(y==y_p+1)
		s_dpo(cd); 
	else
		if(y==y_p-1)
			s_dpo(cu);

	if(x==x_p && y==y_p)
		goto END;

	gotoxy( x+1, y+1);
END:
	x_c=x_p=x;
	y_c=y_p=y;
	return;
}


/* ВЫВОД СТРОКИ СИМВОЛОВ ДО \0 */

static sdps( str )
char    *str;
{
	while( *str ) sdpo( *(str++) );
}

char *getenv();

/* Установить цвет */
setcolor(i)
int i;
{
	register int let,fon;
	if(i<0) return(S_color);
	fon=(i>>4)&017;
	let=i&017;

	i=let+fon*16;
	S_color=i;
	return(S_color);
}
s_setcolor(i)
int i;
{
	if(i==-1 || !t.ct) return(S_color);
	if(_p_color==i) return(S_color=_p_color);

	S_color=_p_color=i;
	if(t.ct==1) let=i;
	textattr(i);
}
/* ВыВОД ОДНОГО СИМВОЛА */

sdpo( c )
unsigned char c;
{
	buf[xbuf] = c;
	if( ++xbuf >= lbuf )  _scrbufout();
}


/* СБРОС БУФЕРА */
scrbufout()
{
	screen_out();
	_scrbufout();
}
_scrbufout()
{
	write(1,buf,xbuf);
	fflush(stdout);
	xbuf = 0;
}


ind_lang()
{
	int x,y;
	x=x_c; 
	y=y_c;
	s_dpp(t.param.xdim-1,0);
	rus_lat?sdpo('R'):sdpo('L');
	s_dpp(x,y);
}
