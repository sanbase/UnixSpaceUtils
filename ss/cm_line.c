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

	Last modification:Fri Mar 23 14:23:13 2001
			Module:cm_line.c
*/
#include "ssh.h"

char *cvmacro();
extern char *fname, *fsuff, *ffull, *name_f, *name_g;

cm_line(         p )
register char *p;
{
	register int     i;
	register char    addsym;
	register char   *s;

	addsym = 0;
	for( ; *p != '\n'; ++p ) {
		if( *p == '$'  and  p[1] != '\n' ) {
			switch( *++p ) {      /* ДОБАВЛЯЕМ ИМЕНА ФАЙЛОВ: */
			case '*':                 /* имена помеченных файлов */
			case ':':                 /* м.б. без пути */
				for(i=0; i<nmarked; ++i) add_name(&cat[marked[i]&0377],0);
				for(i=0; i<nalt;    ++i) add_name(&altnam[i],1+(*p==':'));
				break;

			case '#':                 /* номера помеченных файлов */
				for( i=0; i<nmarked; ++i ) {
					put_sh(' ');
					cpy_shbuf( Conv(marked[i]&0377,5,10,0) );
				}
				break;

				/* guba, 22.06.87  --->  */
			case '%':                 /* номер текущего файла */
				put_sh(' ');
				cpy_shbuf( Conv(indico[current],5,10,0) );
				break;
				/* guba, 22.06.87  <---  */

			case '-':                 /* имена помеченных - файлов */
				for( i=0; i<nfiles; ++i )  {
					register struct dir2 *pd, *sd;
					register int          j;
					pd = item(i);
					if( pd->d_mark == '-' ) {
						add_name(pd,0);
						if( only_x ) {            /* метим зависимые файлы */
							for( j=lind; --j>=0; ) {
								sd = &(cat[indcat[j]]);
								if( is_boss(pd->d_name,sd->d_name) )  add_name(sd,0);
							}
						}
					}
				}
				break;

			case '$':                 /* $$ */
				goto pt;

			case 's': 
				addsym='.';
			default:
				s = cvmacro(*p);
				if( s!=NULL and *s ) {
					if( addsym!=0 )  put_sh(addsym);
					mcpy_shbuf(s);
				}
				addsym = 0;
				break;
			}
		} else {
pt:   
			if( *p=='/' and p[1]=='$' ) {
				addsym = '/';
			} else {
				if( addsym )  {
					put_sh(addsym);  
					addsym = 0;
				}
				else put_sh(*p);
			}
		}
	}
}

/* ВОЗВРАЩАЕТ УКАЗАТЕЛЬ НА ВЫБРАННЫЙ ФАЙЛ НОМЕР 'i' */
struct dir2 *item( i )
short            i;
{
	return( (struct dir2*)(&cat[indico[i]]) );
}
