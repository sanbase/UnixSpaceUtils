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

	Last modification:Fri Mar 23 14:25:27 2001
			Module:malloc.c
*/
void *malloc();
#include <stdio.h>
#include <sys/types.h>
#ifndef LINUX
#include <stdlib.h>
#endif
#include <string.h>
#include <unistd.h>

#ifdef ALLOC
char *sbrk();
static char *max,*min;
typedef int ALIGN;
union headmem
{
	struct
	{
		union headmem *ptr;
		unsigned size;
	} s;
	ALIGN x;
};
typedef union headmem HEADMEM;
static HEADMEM base;
static HEADMEM buzy; /* магигеский адрес для любителей 2 раза делать free */
static HEADMEM *allocp=NULL;
static HEADMEM *morecore();
void *malloc(),*realloc();

void *malloc(nbytes)
unsigned nbytes;
{
	HEADMEM *morecore();
	register HEADMEM *p,*g;
	register int nunits;
	nunits=1+(nbytes+sizeof (HEADMEM)-1)/sizeof (HEADMEM);
	if(allocp==NULL)
	{
		min=sbrk(0);
		base.s.ptr=allocp=g=&base;
		base.s.size=0;
	}
	g=&base;
	for(p=g->s.ptr;;g=p,p=p->s.ptr)
	{
		if(p->s.size>=nunits)
		{
			if(p->s.size==nunits)
				g->s.ptr=p->s.ptr;
			else
			{
				g->s.ptr+=nunits;
				g->s.ptr->s.size=p->s.size-nunits;
				g->s.ptr->s.ptr=p->s.ptr;
				p->s.size=nunits;
			}
			p->s.ptr=&buzy;
			return((char *)(p+1));
		}
		if(p->s.ptr==&base) /* нет подходящей дырки */
		{
			if((p=morecore(nunits))==NULL)
				return(NULL);
		}
	}
}
void free(void *);
#define NALLOC 1024
static HEADMEM *morecore(nu)
unsigned nu;
{
	char *sbrk();

	register char *cp;
	register HEADMEM *up;
	register unsigned int rnu;
	rnu=(NALLOC*(1+(nu-1)/NALLOC))/sizeof (HEADMEM);
	cp=sbrk(rnu*sizeof (HEADMEM));
	if(cp==(char *)-1)
		return(NULL);
	max=cp+rnu*sizeof (HEADMEM);
	up=(HEADMEM *)cp;
	up->s.size=rnu;
	up->s.ptr=&buzy;
	free((void *)(up+1));
	return(allocp);
}
void free(ap)
void *ap;
{
	register HEADMEM *p,*g;
	p=(HEADMEM *)ap-1;

	if((char *)p<min || (char *)p>max)
	{
		char *str="\n\rfree:it's bug in your programm!",i;
		if((char *)ap==NULL)
			return;
		write(2,str,34);
		fflush(stderr);
		read(0,&i,1);
		return;
	}
	if(p->s.ptr!=&buzy)     /* это не занятый блок, освобождать нечего */
		return;
	for(g=&base;;g=g->s.ptr)
	{
		if(g<p && (g->s.ptr>p || g->s.ptr==&base))
		{
			if(g+g->s.size==p)
			{
				g->s.size+=p->s.size;
				if(g+g->s.size==g->s.ptr)
				{
					g->s.size+=g->s.ptr->s.size;
					g->s.ptr=g->s.ptr->s.ptr;
					p->s.ptr=&base;
				}
				goto END;
			}
			if(g->s.ptr==p+p->s.size)
			{
				p->s.size+=g->s.ptr->s.size;
				p->s.ptr=g->s.ptr->s.ptr;
				g->s.ptr=p;
				goto END;
			}
			break;
		}
		if(g->s.ptr==&base)
			break;
	}

	p->s.ptr=g->s.ptr;
	g->s.ptr=p;
END:
	allocp=g;
}
void *realloc(f, nbytes)
void *f;
size_t nbytes;
{
	register HEADMEM *p,*s;
	HEADMEM *g;
	int nunits;
	unsigned size;

	if((char *)f<min || (char *)f>max)
		return(malloc(nbytes));
	s=(HEADMEM *)f-1;
	size = s->s.size;
	nunits=1+(nbytes+sizeof (HEADMEM)-1)/sizeof (HEADMEM);
	g=&base;

	for(p=g->s.ptr;p->s.ptr!=&base;g=p,p=p->s.ptr)
	{
		if(p==s+size && p->s.size>=nunits-size)
		{
			if(p->s.size==nunits-size)
				g->s.ptr=p->s.ptr;
			else
			{
				g->s.ptr+=nunits-size;
				g->s.ptr->s.size=p->s.size-(nunits-size);
				g->s.ptr->s.ptr=p->s.ptr;
			}
			s->s.size=nunits;
			return((char *)f);
		}
	}

	if((g = (HEADMEM *)malloc(nbytes))==NULL)
		return((char *)f);
	s = f;
	p = g;
	nunits--;
	if(nunits<size)
		size = nunits;
	while(size--!=0)
		*p++ = *s++;
	free(f);
	return((char *)g);
}

void *calloc(size_t num,register size_t size)
{
	register void *p;
	size*=num;
	if((p=malloc(size))!=NULL)
		bzero(p,size);
	return(p);
}
#endif
/*
    Вернуть системе неиспользуемую память если она в конце сегмента данных.
*/
sqmem()
{
#ifdef ALLOC
	register HEADMEM *p,*g;
	g=&base;
	for(p=g->s.ptr;;g=p,p=p->s.ptr)
	{
		if((char *)(p+p->s.size)==max && p->s.size >= NALLOC/sizeof (HEADMEM))
		{
			g->s.ptr=p->s.ptr;
			if(p->s.size)
				sbrk(-((p->s.size)*sizeof (HEADMEM)));
			return;
		}
		if(p==&base)
			break;
	}
#endif
}
