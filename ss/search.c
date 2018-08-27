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

	Last modification:Fri Mar 23 14:26:19 2001
			Module:search.c
*/
/* РАБОТА СО СПИСКОМ ИНДЕКСОВ ФАЙЛОВ

   точки входа:

      search_ind( name )      Поиск индекса файла 'name' в упорядоченном
			      списке файлов 'indcat'.
			      Возвращает индекс если найден и
			      ~индекс позиции куда надо вставить если нет.
			      ~индекс а не -индекс, поскольку с минусом
			      неразличим 0.
			      Заметим, что ~индекс < 0.

      insert_ind( ind, to )   Вставка нового индекса в 'indcat' в
			      позицию 'to';
*/

#include "ssh.h"
#include <sys/stat.h>
search(name)
char *name;
{
	register i;
	for(i=0;i<nfiles;i++)
		if(!(strcmp(name,cat[indico[i]].d_name))) return(i);
	return(-1);
}


int search_ind( pd,arg)
struct dir2 *pd;
int arg;
{
	register int code, from, to, middle;

	from = 0;  
	to = lind;                            /* бинарный поиск */
	while( from != to )
	{
		middle = (from + to) / 2;
		code   = diff( pd, &cat[indcat[middle]],arg);
		if( code == 0 )  return( middle );           /* найден */
		if( code >  0 )  from = middle+1;
		else             to   = middle;
	}
	return( ~from );                                 /* не найден */
}


insert_ind( ind, to )
int     ind, to;
{
	int *bmove, *emove;

	bmove = &indcat[to];
	emove = &indcat[lind];
	while( bmove < emove )
	{
		--emove;
		*(emove+1) = *emove;
	}
	*bmove = ind;
	++lind;
}
diff(pd1,pd2,arg)
struct dir2 *pd1,*pd2;
int arg;
{
	long i;
	register int r;
	if(!arg) return(strcmp(pd1->d_name,pd2->d_name));
	if(arg==1 || arg==2)
	{
		i=pd1->size-pd2->size;
		r=i<0l?-1:i>0l?1:0;
		if(r) return(arg==1?r:-r);
		return(strcmp(pd1->d_name,pd2->d_name));
	}
	else if(arg==3 || arg==4)
	{
		i=pd1->date-pd2->date;
		r=i<0l?-1:i>0l?1:0;
		if(r) return(arg==3?r:-r);
		return(strcmp(pd1->d_name,pd2->d_name));
	}
	else
		i=pd1->uid-pd2->uid;
	if(i) return(i);
	return(strcmp(pd1->d_name,pd2->d_name));
}
