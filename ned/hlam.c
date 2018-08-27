/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: hlam.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#define X11
#include "tty_codes.h"
#include "ned.h"


void emess(char *str)
{
	int nb;
	int l,l1;
	char *mess,*ch;

	stor_poz();
	mess=Strdup(str);
	nb=get_box(0,l_y-5,l_x+1,errno?5:4);
	setcolor(COLMESS);
	if(!errno)
	        l=strlen(mess);
	else {
		ch=strerror(errno); l=max(strlen(mess),strlen(ch)+strlen(pmes(118)));
	}
	if(l>l_x-6) mess[l=l_x-6]=0;
	wind(((l_x-l)>>1)-2,l_y-5,l+4,errno?4:3);
	l=strlen(mess); dpp(((l_x-l)>>1),l_y-4); dps(mess);
	if(errno) {
	        l=strlen(ch)+strlen(pmes(118)); dpp(((l_x-l)>>1),l_y-3);
		dps(pmes(118)); dps(ch);
	}
	dpp(l_x-1,l_y);
	Free(mess);
	ReadCommand(NULL,-1);
	put_box(0,l_y-5,nb); free_box(nb);
	if(errno) errno=0;
	rest_poz();
}

void title(int y,char *titl)
{
	int l,x;

	l=(int)strlen(titl);
	if(l<=0) return;
	wind(x=((l_x-l-4)>>1),y,l+4,3);
	dpp(x+2,y+1); dps(titl);
}

void wind(int x,int y,int l,int h)
{
	int c,i;

	c=setcolor(-1);

#ifndef OLDSCREEN
#ifdef X11
	box1(x,y,l,h,' ',0,0,0,0);
#else
	box1(x,y,l,h,' ',0,0);
#endif
#else
	box(x,y,l,h-1,' ');
#endif
	for(i=y+1;i<y+h+1;i++) revers(x+l,i);
	for(i=x+1;i<x+l;i++) revers(i,y+h);

	setcolor(c);
}

static int std_mes=0;

int line_status(int mes)
{
	int i;

	if(mes==std_mes) return(mes);
	if(!mes) {
		 std_mes=0; return(0);
	}
	down_mes(pmes(mes),COLFSTL,COLCSTL);
	i=std_mes; std_mes=mes;
	return(i);
}

#define BRH 10

int y_n_m(int  y,char *mess1,char *mess2,int *arg,...)
{
	int nb,len1,len2,i,l,s,x;
	char *hotmess;
	va_list ap;

	stor_poz();
	va_start(ap,arg);
	len1=Strlen(mess1);
	len2=Strlen(mess2);
	nb=get_box(0,y,l_x+1,8);
	setcolor(COLMBGR);
	l=max(len1,len2);
	for(s=0,i=0;;i++) {
		hotmess=va_arg(ap,char *);
		if(hotmess==NULL) break;
		s+=Strlen(hotmess)+(i?BRH:0)+2;
	}
	va_end(ap);
	if(i<2) { /* ??? */
		*arg=0; i=C_STOP; goto END;
	}
	i=max(l,s);
	wind(x=((l_x-i-7)>>1),y,i+10,7);
	dpp(x+2,y+1);  dps(" ▄▄▄");
	dpp(x+((i-len1)>>1)+9,y+1);
	if(len1) dps(mess1);
	dpp(x+2,y+2);  dps("█   █");
	dpp(x+((i-len2)>>1)+9,y+2);
	if(len2) dps(mess2);
	dpp(x+2,y+3);  dps("   █");
	dpp(x+2,y+4);  dps("  █");
	dpp(x+2,y+5 ); dps("  ▄");
	s=(abs(i-s)>>1); len2=-1;
	while(1) {
		if(*arg!=len2) {
			dpp(x+s+9,y+5); setcolor(COLMBGR);
			va_start(ap,arg);
			for(len1=0;;len1++) {
				hotmess=va_arg(ap,char *);
				if(hotmess==NULL) break;
				if(len1) dpn(BRH,' ');
				if(len1==*arg) {
					setcolor(COLMSTR); i=x_c;
				}
				dpo(' '); dps(hotmess); dpo(' ');
				if(len1==*arg) setcolor(COLMBGR);
			}
			va_end(ap);
			len2=*arg;
			dpp(i,y+5);
		}
		switch(i=ReadCommand(NULL,-1)) {
		case C_STOP:
		case C_RETURN:
			goto END;
		case C_STRLEFT:
			if(!*arg) continue;
			(*arg)--;
			break;
		case C_STRRIGHT:
			{
				int j;

				va_start(ap,arg);
				for(j=0;j<=*arg;j++) {
					hotmess=va_arg(ap,char *);
					if(hotmess==NULL) break;
				}
				if(hotmess !=NULL)
					if(va_arg(ap,char *)==NULL) {
						va_end(ap); continue;
					}
				va_end(ap);
			}
			(*arg)++;
			break;
		default:
			dpo(bl); continue;
		}
	}
END:
	put_box(0,y,nb); free_box(nb); rest_poz();
	return(i);
}


/* "Растягивает" строку статуса на полную строку внизу экрана */
static char *norm_status_line(char *s,int smax)
{
	static char str[NAMESIZE+1];
	int i,j,flag=0;

	strncpy(str,s,smax);
	for(j=strlen(str);j<smax;) {
		for(i=0;str[i] && j<smax;i++) {
			if(i && str[i-1]==';') {
				memmove(str+i+1,str+i,(j++)-i); str[i++]=' ';
				flag=1;
			}
		}
		for(i=0;str[i] && j<smax;i++)
			if((!i && flag) || str[i]=='-') {
				memmove(str+i+1,str+i,(j++)-i); str[i++]=' ';
				flag=1;
			}
		if(!flag) break;
	}
	return(str);
}

void down_mes(char *line,int col1,int col2)
{
	int i,c;
	char *ch;

	c=setcolor(-1);
	stor_poz();
	setcolor(col1); dpp(X,Y+H);
	for(i=0;i<X+L;i++) dpo(' ');
	ch=norm_status_line(line,L);
	setcolor(col1); dpp(X,Y+H);
	for(i=0;ch[i] && i<L;i++) {
		if(ch[i]==';') {
			setcolor(col1); dpo(' '); continue;
		}
		if(ch[i]=='-') {
			dpo(' '); setcolor(col2); continue;
		}
		if((ch[i]>0 && ch[i]<' '))
			dpo(' ');
		else
			dpo(ch[i]);
	}
	rest_poz();
	setcolor(c);
}


void apaint(int x,int y,int show,int size,int p,
	    int t,int (*func)(),int col1,int col2)
{
	register int k;

	for(k=0;k<show && k+t<size;k++) (*func)(x,y+k,k+t,k==p?col2:col1);
}

int amenu(int x,int y,int len,int show,int size,
       int (*func)(),int col1,int col2,int *ii,int *jj,int sym)
{
	int c;

	setcolor(col1);
	wind(x,y,len+2,show+2);
	x++; y++;
	apaint(x,y,show,size,*ii,*jj-*ii,func,col1,col2);
	if(sym) {
		c=(int)sym; goto SWI;
	}
	while(1)
	{
#ifndef NOBORDIND
	        show_bord_vert(x+len,y,show-2,size,*jj,col1);
#endif
		dpp(x,y+*ii);
		c=ReadCommand(NULL,-1);
#ifdef X11
		if(c==C_MOUSE) {
			if(ev.b!=3) {
#ifndef NOBORDIND

				int p;

				if((p=get_bord_vert(ev.x,ev.y))>=0) {
					if(p<*jj-*ii || p>=*jj-*ii+show) {
					        if(p<*ii)
						        *jj=*ii=p;
					        *jj=p;
					}
					else {
						*ii+=p-*jj; *jj=p;
					}
					if(show+*jj-*ii>size) *ii=show-1;
					break;
				}
#endif
			if(ev.x>=x && ev.x<=x+len && ev.y>=y-1 && ev.y<=y+show+1)
				if(ev.y==y+*ii)
					c=C_RETURN;
				else
					if(ev.y==y-1)
						c=C_PGUP;
					else
						if(ev.y==y+show)
							c=C_PGDN;
						else
						{
							*jj-=*ii-ev.y+y;
							*ii=ev.y-y;
							break;
						}
			}
			else
				c=C_STOP;
		}
#endif

SWI:
		switch(c)
		{
		case C_DOWN:
			if(*jj==size-1 || *ii==size-1) {
				dpo(bl); continue;
			}
			if(*ii==show-1)
				(*jj)++;
			else
			{
			        (*jj)++;
			        if(ii!=jj) (*ii)++;
			}
			break;
		case C_UP:
			if(!(*jj)) {
				dpo(bl); continue;
			}
			if(!(*ii))
				(*jj)--;
			else {
			        (*jj)--;
			        if(ii!=jj) (*ii)--;
			}
			break;
		case C_STRHOME:
HO:
			if(!(*jj)) continue;
			*ii=0; *jj=0;
			break;
		case C_STREND:
EN:
			if(*jj==size-1 || *ii==size-1)
				continue;
			*ii=(size<show)?size-1:show-1;
			if(ii!=jj) *jj=size-1;
			break;
		case C_PGDN:
			if(*jj+(show<<1)-*ii<size) {
				(*jj)+=show; break;
			}
			else
				goto EN;
		case C_PGUP:
			if(*jj-show-*ii>0) {
				(*jj)-=show; break;
			}
			else
				goto HO;
		default:
		        apaint(x,y,show,size,*ii,*jj-*ii,func,col1,invers(col2));
			return(c);
		}
		apaint(x,y,show,size,*ii,*jj-*ii,func,col1,col2);
	}
}

#ifndef NOBORDIND

#ifdef X11

struct indbord {
	int  x;
	int  y;
	int len;
	int poz;
	int all;
};

static struct indbord g,v;
#endif

void show_bord_vert(int x,int y,int len,int all,int i,int col) {
	int j;

	if(!all) all++;
	setcolor(invers(col));
#ifdef X11
	v.x=x; v.y=y; v.len=len; v.all=all;
#endif
	dpp(x,y); dpo('■');
	setcolor(col);
	for(j=0;j<len;j++)
	{
		dpp(x,y+j+1);
		if(j==(int)((double)len/(double)all*(double)i) || (j==len-1 && i>=all))
#ifdef X11
		{
			v.poz=j;
#endif
		        dpo('■');
#ifdef X11
		}
#endif
		else
		        dpo('░');
	}
	setcolor(invers(col)); dpp(x,y+len+1); dpo('■');
}

void show_bord_gor(int x,int y,int len,int all,int i,int col)
{
	int j;

	if(!all) all++;
	setcolor(invers(col));
#ifdef X11
	g.x=x; g.y=y; g.len=len; g.all=all;
#endif
	dpp(x,y); dpo('<');
	setcolor(col);
	for(j=0;j<len;j++)
	{
		dpp(x+j+1,y);
		if(j==(int)((double)len/(double)all*(double)i) || (j==len-1 && i>=all))
#ifdef X11
		{
			g.poz=j;
#endif

		        dpo('■');
#ifdef X11
		}
#endif
		else
		        dpo('░');
	}
	setcolor(invers(col)); dpp(x+len+1,y); dpo('>');
}

#ifdef X11

int get_bord_vert(int x,int y)
{
	if(v.x!=x ||  y<v.y || y>v.y+v.len+1 || v.y+v.poz+1==y)
	        return(-1);
	if(y==v.y+v.len)
	        return(v.all-1);
	else
	        if(y==v.y+1)
		        return(0);
		else
	                if(y==v.y)
			        return(-2);
			else
	                        if(y==v.y+v.len+1)
					return(-3);
	return((int)((double)v.all/(double)v.len*(double)(y-v.y)));
}

int get_bord_gor(int x,int y)
{
	if(g.y!=y || x<g.x || x>g.x+g.len+1 || g.x+g.poz+1==x)
	        return(-1);
	if(x==g.x+g.len)
	        return(g.all-1);
	else
	        if(x==g.x+1)
		        return(0);
		else
			if(x==g.x)
				return(-2);
			else
				if(x==g.x+g.len+1)
					return(-3);
	return((int)((double)g.all/(double)g.len*(double)(x-g.x)));
}

#endif

#endif

#ifdef NO_BZERO

int bzero(char *a,int len)
{
	memset(a,0,len);
}

#endif

#define M_X 3

static int x[M_X],y[M_X],c[M_X],poz=-1;

void stor_poz()
{
	if(poz<M_X-1) {
		x[++poz]=(int)x_c; y[poz]=(int)y_c; c[poz]=setcolor(-1);
	}
}

void rest_poz()
{
	if(poz>=0) {
		dpp(x[poz],y[poz]); setcolor(c[poz]); poz--;
	}
}
