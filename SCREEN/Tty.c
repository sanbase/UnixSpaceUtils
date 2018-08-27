/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2000 SanBase inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (1990-2000)
	and M. Flerov (1986-1988).
	E-mail: lashenko@unixspace.com

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

	Last modification:Fri Mar 23 14:15:13 2001
			Module:Tty.c
*/
/*
 *
 *      Ttyset ()
 *
 *              - set terminal CBREAK mode.
 *
 *      Ttyreset ()
 *
 *              - restore terminal mode.
 *
 *      TtyFlush ()
 *
 *              - flush input queue.
 */
#include <unistd.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#ifdef SPARC
 #include "termios.h"
#else
 #include <termios.h>
#endif
#include <signal.h>
#include "tty_codes.h"



static struct termios oldtio, newtio;
#define CHANNEL STDERR_FILENO                      /* output file descriptor */

static int ttyflags;

static void onintr(int i)
{
	dpend();
	exit(0);
}
static int _set=0;
#ifndef TCGETA
#define TCGETA TIOCGETA
#define TCSETA TIOCSETA
#define TCFLSH TIOCFLUSH
#endif

void Ttyset ()
{
	static   void      (*fsig)(int);
	if(_set) return;
	if (ioctl (CHANNEL, TCGETA, (char *) &oldtio) < 0)
		return;

	newtio = oldtio;
	newtio.c_iflag &= ~(INLCR | ICRNL | IGNCR | ISTRIP);
	newtio.c_oflag &= ~OPOST;      /* no output postprocessing */
	newtio.c_lflag &= ~(ECHO | ICANON);

	newtio.c_cc [VMIN]   =  1;     /* break input after each character */
	newtio.c_cc [VTIME]  =  0;     /* timeout is 0 msecs */
	newtio.c_cc [VINTR]  =  3;     /* interupt after CTRL/C */
	newtio.c_cc [VQUIT]  = -1;
	tcsetattr(CHANNEL,TCSANOW,&newtio);

	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == SIG_DFL )
		fsig = onintr;
	signal( SIGINT, fsig );
	_set=1;
}

void Ttyreset ()
{
	if(!_set) return;
	ioctl (CHANNEL, TCSETA, (char *) &oldtio);
	signal(SIGINT,SIG_DFL);
	_set=0;
}

void TtyFlush ()
{
	int p;

	if(_set) return;
	ioctl (CHANNEL, TCFLSH, 0);
}
