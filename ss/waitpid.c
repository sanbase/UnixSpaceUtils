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

	Last modification:Fri Mar 23 14:27:07 2001
			Module:waitpid.c
*/
/* ЖДЕМ ПРОЦЕСС 'pid' И ВОЗВРАЩАЕМ КОД ОШИБКИ (системные < 0 )
   Если pid == 0, то просто проверяет состояние процессов */
#include "ssh.h"
#include <signal.h>
#include "run.h"
#ifdef BSD
#    include <wait.h>
#endif

int wait_pid( pid )
int       pid;
{
	register int i, pr;
	void *old_trap;
	int          stt;

	old_trap = signal(SIGINT,SIG_IGN);
	for(;;) {
		pr = wait(&stt);
		if( pr <= 0  ||  pid == pr )  break;
		for( i=maxrun; --i>=0; )
		{
			if( at_run[i].r_pid  == pr )
			{
				at_run[i].r_pid  = -1;
				pr               = stt & 0177;
				at_run[i].r_code = !pr ? ((stt>>8)&0377) : -pr ;
				break;
			}
		}
	}
	signal(SIGINT,old_trap);
	ch_flag = 0;
	pr      = stt & 0177;
	return( !pr ? ((stt>>8)&0377) : -pr );
}
