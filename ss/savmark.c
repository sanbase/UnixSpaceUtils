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

	Last modification:Fri Mar 23 14:26:13 2001
			Module:savmark.c
*/
/* СМЕНА ТЕКУЩЕГО КАТАЛОГА ==> ЗАПОМИНАЕТ ПОМЕТКУ */

#include "ssh.h"

int savmark() {
    register int          i,seek=0;
    register struct dir2 *pd;

/*  rspec('q',NULL,NULL,NULL);  немного меняем логику (guba,24.06.87) */
    if( nmarked != 0 ) {
	altcat[0] = 0;
        if( hcat[0] == '/' ) {
            strcpy( altcat, hcat );
            if( arname != NULL )  strcpy( altarname, arname );
            else                  altarname[0] = 0;
            altpath = arpath ? altcat + (arpath-hcat) : NULL ;

	    for( i=0, nalt=0; i<nmarked and nalt<maxalt; ++i )
	    {
		pd = &cat[marked[i]];
		if(seek+strlen(pd->d_name)>1000)
			break;
		strcpy(alt_names+seek,pd->d_name);
		altnam[nalt].d_name=alt_names+seek;
		altnam[nalt++].d_mark=pd->d_mark;
		seek+=strlen(pd->d_name)+1;
#ifdef dummy
                if( only_x ) {           /* запоминаем зависимые файлы */
                    register int          j;
                    register struct dir2 *pdx;

                    for( j=lind; --j>=0 && nalt<maxalt; ) {
			pdx = &cat[indcat[j]];
			if( is_boss( pd->d_name, pdx->d_name ) )
			{
				if(seek+strlen(pdx->d_name)>1000)
					break;
				strcpy(alt_names+seek,pdx->d_name);
				altnam[nalt].d_name=alt_names+seek;
				altnam[nalt++].d_mark=pdx->d_mark;
				seek+=strlen(pdx->d_name)+1;
                        }
                    }
                }
#endif
            }
        }
    }
}
