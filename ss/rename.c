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

	Last modification:Fri Mar 23 14:25:58 2001
			Module:rename.c
*/
/* ПЕРЕИМЕНОВАНИЕ ФАЙЛА 'name' И ЗАВИСИМЫХ ФАЙЛОВ */

#include "ssh.h"
#include <sys/stat.h>
#include "tty_codes.h"
static srename();
extern char ftype;
extern int   wcolumn;
extern int x_c,y_c;
#undef D_NAME
#define D_NAME 128
char *Rename( name )
char     *name;
{
	register int c, delta, i;
	char        *p;
	int          pos, lroot, er_flag;
	static char  new[256], dep[256];

	dpo('=');  
	strcpy(new,name);

	pos = 0;
	setcolor(014+0200+0x400);
inp:
	do {                                      /* ввод нового имени */
		c = dpedit(0,new,255,wcolumn==16?14:wcolumn==26?17:24,x_c,y_c,0 );
		if( c == Esc || c==F10) onbreak();
		if( c == s_refuse )  goto ref;      /* отказ от переименования */
		if( c == dc || c==F1 || c==s_help)
		{
			help("ssh",'=');  
			goto ref;
		}
	} while ( c!=rn && c!=s_erase );
	er_flag = (c==s_erase);

	if( !strcmp(new,name) ) {             /* набрали такое же имя */
ref:    
		return(name);
	}

	if( *new==0                           /* проверка, есть ли уже имя */
	or (!er_flag and exist(new)) )  {
		dpo(bl);  
		goto inp;
	}

	if( only_x ) {
		delta = strlen(new) - strlen(name);
		for( c=lind; --c>=0; ) {        /* переименование зависимых */
			p = cat[indcat[c]].d_name;
			if( (lroot = is_boss( name, p ))      /* имя зависимое     */
			    &&  strcmp("..",p)  &&  strcmp(".",p) /*   и не спец-имя   */
			    &&  lroot + delta <= 255           /*   и суффикс тот же*/
			&&  !strcmp( name+lroot, new+lroot+delta )
			    &&  lroot+delta+strlen(p+lroot) <= 255 )
			{
				strcpy( dep, new );
				strcpy( dep+lroot+delta, p+lroot );
				srename( p, dep, er_flag );
			}
		}
	}

	srename( name, new, er_flag );
	return( new );
}


static srename(    old, new, er_flag )
register char *old,*new;
int                      er_flag;
{
	errno = 0;
	if( er_flag and exist(new) )  srm(new);
	if( errno )  return;
	if( rspec( 'a', old, new, NULL ) == 0 ) {
		errno = 0;
		if( link(old,new) != -1 )  unlink( old );
		ind_err(old,1);
	}
}
