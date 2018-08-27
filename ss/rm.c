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

	Last modification:Fri Mar 23 14:26:04 2001
			Module:rm.c
*/
/* “„€‹…ˆ… ”€‰‹€ (Š€’€‹ƒ€) ˆ ‚‘…• ‡€‚ˆ‘ˆŒ›• ”€‰‹‚ (¬®¦¥â ¡ëâì) */

#include "ssh.h"
#include <sys/stat.h>

rm()
{
	register int  i, j;
	register char *p;
	struct   dir2 *pd;

	if(!nfiles)
		return;
	if(!yes(1,messages[78]))
		return;
	for(i=nfiles; --i>=0; )
	{
		pd = item(i);
		if( pd->d_mark == '-' )
		{
			if( only_x )
			{              /* ã¤ «ï¥¬ § ¢¨á¨¬ë¥ ä ©«ë */
				for( j=lind; --j>=0; )
				{
					p = cat[indcat[j]].d_name;
					if( is_boss( pd->d_name, p ) )
						srm( p );
				}
			}
			srm( pd->d_name );          /* ... ¨ á ¬ ä ©« */
		}
	}
}


srm(               name )
register char *name;
{
	register int i;
	if( rspec( 'b', name, NULL, NULL ) ) {
		errno = 0;
	}
	else {
		errno = 0;
		unlink( name );
		for( i=lind; --i>=0; ) {
			if( !strcmp( cat[indcat[i]].d_name, name ) )
			{
				*cat[indcat[i]].d_name=0;
				break;
			}
			ind_err(name,1);
		}
	}
}
