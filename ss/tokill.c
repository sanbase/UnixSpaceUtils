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

	Last modification:Fri Mar 23 14:26:55 2001
			Module:tokill.c
*/
/*  “„€‹…ˆ… €€‹‹…‹œë• –…‘‘‚ */

#include <signal.h>
#include "tty_codes.h"
#include "ssh.h"
#include "run.h"

int to_kill() {
    register int i, n_run, sig;
    int          current, pr;

    for( n_run=i=0; i<maxrun; ++i )  {
        if( at_run[i].r_pid > 0 ) ++n_run;
        at_run[i].r_mark = 0 ;
    }
    if( !n_run )  return( -1 );          /* ­¥â ¯ à ««¥«ì­ëå ¯à®æ¥áá®¢ */
    current = 0;

inp:
    switch( dpt(n_run, &current, 1, w_run, x_run, y_run) ) {
        default:         dpo(bl);  goto inp;

        case '?':
        case '"':
        case '2':
        case '1':        rspec('d',NULL,NULL,NULL);  ind_cat();  break;

	case F1:     help("ssh",'&');

        case '(':
        case ')':
        case s_refuse:   break;

        case '-':
            dpo('-');
            pr = current;
            for( i=0; i<maxrun; ++i ) {
                if( at_run[i].r_pid > 0 )  if( !(pr--) )  break;
            }
            at_run[i].r_mark = '-';
            current = (current+n_run-1) % n_run;
            goto inp;

        case rn:         sig = SIGTERM;  goto kl;
	case F8:    sig = SIGKILL;  goto kl;
        kl: for( i=maxrun; --i>=0; )  {
                if( at_run[i].r_mark=='-' && (pr=at_run[i].r_pid) > 0 ) {
                    kill( pr, sig );
                }
            }
            break;
    }
/*    ind_run();        */
    return(0);
}
