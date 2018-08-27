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

	Last modification:Fri Mar 23 14:24:30 2001
			Module:getsuit.c
*/
/** ВЫБОР ФАЙЛОВ, УДОВЛЕТВОРЯЮЩИХ УСЛОВИЯМ:

   get_suit()      сравнивается со строкой 'select'
		   если 'only_x', то не является зависимым файлом
*/

#include "ssh.h"

int sort_arg;

get_suit(arg)
int arg;
{
	register char *n, *s;
	register int   code;
	int            i, skip, ind;
	struct dir2   *pd;
	if(arg)
		for( i=lind; --i>=0; )
		{        /* убираем '-' пометку */
			pd = (struct dir2*) (&cat[indcat[i]]);
			if( pd->d_mark=='-' || pd->d_mark=='d' )
				pd->d_mark=0;
		}

	nfiles = 0;
	if( ico_from )  indico[nfiles++] = maxcat+1;   /* <--( */
	skip = ico_from;

	for( i=0; i<lind; ++i ) 
	{
		if( nfiles >= maxico ) 
		{
			indico[nfiles-1] = maxcat;     /* )--> */
			break;
		}
		pd = (struct dir2*) (&cat[indcat[i]]);

		if( *Select ) {  /* селекция по имени */
			n = pd->d_name;
			s = Select;
			while( *s )  
			{
				if( *s == '^' )  
				{
					if( *n == 0 )  goto not_suit;
				}
				else 
				{
					if( *n != *s ) goto not_suit;
				}
				++n;
				++s;
			}
		}

		if( selsuf )
		{                           /* селекция по суффиксу */
			register int l;
			n = pd->d_name;
			for( l=strlen(n); --l>=0; )  if( n[l]=='.' )  break;
			if( l < 0  ||  n[l+1] != selsuf )  goto not_suit;
		}

		if( only_x && pd->d_name[0]=='.')
			goto not_suit;  /* без .файлов */

		if( only_x && nfiles )
		{         /* не учитываем зависимые файлы */
			for(code=0,ind=0;ind<nfiles;ind++)
			{
				if(code=depend(cat[indico[ind]].d_name,pd->d_name))
				{
					register int j;
					if(code>0) goto not_suit; /* новый зависит от старого */
					/** старый зависит от нового*/
					for(j=ind;j<nfiles-1;j++)
						indico[j]=indico[j+1];
					nfiles--; 
					ind--;
				}
			}
			if(code<=0)  goto toico;   /* независимый файл */
		} 
		else 
		{
toico:
			if( skip-- <= 0 )  indico[nfiles++] = indcat[i];
		}
not_suit:
		;
	}
}
