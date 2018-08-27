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

	Last modification:Fri Mar 23 14:25:10 2001
			Module:indrun.c
*/
/* ����� ���������� � ��������� ��������� */

#include "tty_codes.h"
#include "ssh.h"
#include "run.h"

ind_run() {
    register int         i, n_run;
    register struct run *p;
    char                *pn;
    int                  i_run, c, fout;
    static int           last_run = 0;

    i_run = 0;  n_run = 0;                /* �᫮ ����饭��� = ? */
    for( i=maxrun; --i>=0; )  if( at_run[i].r_pid )  ++n_run;

    if( n_run ) {
	if( ch_flag )
	{              /* �஢��塞 �����襭�� ����ᮢ */
#ifndef BSD
            if( (i=fork()) == 0 )  exit(0);
            wait_pid(i);
#else
            wait_pid(0);
#endif
        } else {
            ++ch_flag;
	}

	dpp(0,y_run);  dpo( '(' );

	for( i=0; i<maxrun; ++i ) {             /* ����� �믮�������� */
	    p = &at_run[i];
	    if( p->r_pid > 0 ) {
		dpp( x_run + i_run*w_run, y_run );
		dpo(' ');  dps( p->r_name );
		dpn(w_run-1-strlen(p->r_name),' ');
		++i_run;
	    }
	}

	for( i=0; i<maxrun; ++i ) {             /* ����� �����訢���� */
	    p = &at_run[i];
	    if( p->r_pid < 0 ) {
		register int cdd;
		cdd = p->r_code;
		c = !cdd ? '+' : ((cdd>0) ?'?' :'-'); /*��� �����襭��*/
		dpp( x_run + i_run*w_run, y_run );
		if( cdd )  dps((char *) Conv(cdd<0 ? -cdd : cdd ,3,10,' ') );
		else       dps("   " );
		dpo(c);  dps( p->r_name );
		dpn(w_run-4-strlen(p->r_name),' ');

		dpo(bl);                     /* 㤠�塞 䠩� ��⮪��� */
		pn = prot_name(p->r_name);   /*           �᫨ �� ���� */
		fout = open(pn,0);
		if( fout > 0 and lseek(fout,(off_t)0l,2)==0 ) {
		    unlink(pn);   last_mod=0;
		}
		close(fout);

		p->r_pid = 0;
		++i_run;
	    }
	}

	dpp(x_run+n_run*w_run,y_run);  dpo( ')' );
    }

    if( last_run != n_run ) {                 /* ��ࠥ� ���� �뤠� */
	last_run = n_run;
	if( !n_run )  {                               /* ��ࠥ� (...) */
	    dpp(0,y_run);                     dpo(' ');
	    dpp(x_run+last_run*w_run,y_run);  dpo(' ');
	}
	while( last_run > n_run ) {                   /* ��ࠥ� ����� */
	    dpp( x_run + n_run*w_run, y_run );  dpn(w_run,' ');
	    ++n_run;
	}
    }
}
