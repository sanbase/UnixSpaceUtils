#include <string.h>
/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev
	Copyright (c) 1996 SOFTEX PLUS, inc. Toronto ON, Canada.
	All rights reserved.
	Writen by Alexander Lashenko.
	Report problems to san@interlog.com, unitex@public.ua.net
*/


#include "tty_codes.h"
extern int black_white_mode;

/* draw box, if ch!=0 than fill it by character ch */
void box(int x,int y,int l,int h,char ch)
{
	box2(x,y,l,h,ch,0,0,0,0);
}
void box1(int x,int y,int l,int h,char ch,int bord_fg,int bord_bg,int text_fg,int text_bg)
{
	register int i,j;

	h--;
	dpp(x,y);
	if(bord_fg || bord_bg) Set_Color(bord_bg,bord_fg);
	dpo('Ú');
	dpn(l-2,'Ä');
	dpo('¿');
	for(j=y+1;j<y+h;j++)
	{
		if(ch) dpp(x,j);
		for(i=x;i<x+l;i++)
			if(i==x || i==x+l-1)
			{
				dpp(i,j);
				if(bord_fg || bord_bg) Set_Color(bord_bg,bord_fg);
				dpo('³');
			}
			else if(ch)
			{
				if(text_fg || text_bg) Set_Color(text_bg,text_fg);
				dpo(ch);
			}
	}
	dpp(x,y+h);
	if(bord_fg || bord_bg) Set_Color(bord_bg,bord_fg);
	dpo('À');
	dpn(l-2,'Ä');
	dpo('Ù');
}

void box2(int x,int y,int l,int h,char ch,int bord_fg,int bord_bg,int text_fg,int text_bg)
{
	register int i,j;

	h--;
	dpp(x,y);
	if(bord_fg || bord_bg) Set_Color(bord_bg,bord_fg);
	dpo('É');
	dpn(l-2,'Í');
	dpo('»');
	for(j=y+1;j<y+h;j++)
	{
		if(ch) dpp(x,j);
		for(i=x;i<x+l;i++)
			if(i==x || i==x+l-1)
			{
				dpp(i,j);
				if(bord_fg || bord_bg) Set_Color(bord_bg,bord_fg);
				dpo('º');
			}
			else if(ch)
			{
				if(text_fg || text_bg) Set_Color(text_bg,text_fg);
				dpo(ch);
			}
	}
	dpp(x,y+h);
	if(bord_fg || bord_bg) Set_Color(bord_bg,bord_fg);
	dpo('È');
	dpn(l-2,'Í');
	dpo('¼');
}

struct clr color_to_clr(int);

void BOX(int x,int y,int l,int h,char ch,int bord,int text)
{
	struct clr clr1,clr2;

	if(bord)
		setcolor(bord);
	bzero(&clr1,sizeof clr1);
	bzero(&clr2,sizeof clr2);
	if(bord)
		clr1=color_to_clr(bord);
	if(text)
		clr2=color_to_clr(text);
	box2(x,y,l,h,ch,clr1.fg+(clr1.atr1<<8),clr1.bg+(clr1.atr2<<8),clr2.fg+(clr2.atr1<<8),clr2.bg+(clr2.atr2<<8));
	shadow(x,y,l,h);
	if(text)
		setcolor(text);
}

extern struct s_tdescr t;
void shadow(int x,int y,int l,int h)
{
	register int i,j;

	if(!t.ct) return;
	i=x+l;
	j=y+1;
	if(h<2)
	{
		revers(i,y);
	        revers(i+1,y);
	}
	for(;j<y+h+1;j++)
	{
		revers(i,j);
		revers(i+1,j);
	}
	x+=2;
	y+=h;
	for(i=0;i<l-2;i++)
		revers(x+i,y);
}
int shad(int bg)
{
	if(bg<8)
		return(8);
	if(bg==8)
		return(0);
	if(bg==14)
		return(20);
	if(bg<16)
		return(bg-8);
	if(bg==17)
		return(26);
	if(bg<20)
		return(bg-1);
	if(bg==20)
		return(8);
	return(bg-1);

}
void revers(int x,int y)
{
	unsigned int ch;
	unsigned fg,bg;
	int clr;

	dpp(x,y);
	ch =get_ch(x,y);
	if(ch==0xff)
		ch=' ';
	bg=get_bg(x,y);
	fg=get_fg(x,y);
	clr=bg&0x1f;
	{
		if(ch==176)
		{
			Set_Color(26,0);
			dpo(' ');
			return;
		}
		if(ch==178 || ch==177)
			ch--;
		if(clr!=7 && (clr<=8 || clr==16 || clr==25 || clr==26 || clr==27 || black_white_mode))
			bg=(bg&0x300);
		else if(clr==7 || clr>16)
			bg=25+(bg&0x300);
		else if(clr==15)
			bg=22+(bg&0x300);
		else bg=8+(bg&0x300);
		Set_Color(bg,shad(fg));
		dpo(ch);
	}
}

extern struct s_tdescr t;
void vert(int x,int y,int h)
{
	register int i;
	int ch;
	if(t.ct) ch='³';
	else     ch='|';
	dpp(x,y);
	for(i=0;i<h;i++)
	{
		dpp(x,y+i);
		dpo(ch);
	}
}
void goriz(int x,int y,int l)
{
	dpp(x,y);
	if(t.ct)
		dpn(l,'Ä');
	else
		dpn(l,'-');
}
void goriz_s(int x,int y,int l)
{
	dpp(x,y);
	if(t.ct)
		dpo('Ç');
	else    dpo('+');
	goriz(x+1,y,l-1);
	dpp(x+l,y);
	if(t.ct)
		dpo('¶');
	else    dpo('+');
}
void vert_s(int x,int y,int h)
{
	dpp(x,y);
	if(t.ct)
		dpo('Ñ');
	else    dpo('+');
	vert(x,y+1,h-1);
	dpp(x,y+h);
	if(t.ct)
		dpo('Ï');
	else    dpo('+');
}
void krest(int x,int y)
{
	dpp(x,y);
	if(t.ct)
		dpo('Å');
	else    dpo('+');
}
