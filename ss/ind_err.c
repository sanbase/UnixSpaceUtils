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

	Last modification:Fri Mar 23 14:24:51 2001
			Module:ind_err.c
*/
#include <errno.h>
#include "tty_codes.h"
#include "ssh.h"

int ind_err( name, flag )
char    *name;
int            flag;
{
	register char *p;

	if( errno != 0 ) {
		dpp(0,ysize-2);  
		dpo(el);
		dpp(0,ysize-1);  
		dpo(el);
		dpp(0,ysize-1);
		if( name )  {
			dps(name);
			dps(" - ");
		}
		dpo(bl);
		dps(messages[67]);
		switch( errno ) {
		case EPERM:   
			p=messages[11]; 
			break;
		case ENOENT:  
			p=messages[12]; 
			break;
		case ESRCH:   
			p=messages[13]; 
			break;
		case EIO:     
			p=messages[14]; 
			break;
		case ENXIO:   
			p=messages[15]; 
			break;
		case E2BIG:   
			p=messages[16]; 
			break;
		case EBADF:   
			p=messages[17]; 
			break;
		case EAGAIN:  
			p=messages[18]; 
			break;
		case ENOMEM:  
			p=messages[19]; 
			break;
		case EACCES:  
			p=messages[20]; 
			break;
		case ENOTBLK: 
			p=messages[21]; 
			break;
		case EBUSY:   
			p=messages[22]; 
			break;
		case EEXIST:  
			p=messages[23]; 
			break;
		case EXDEV:   
			p=messages[24]; 
			break;
		case ENODEV:  
			p=messages[25]; 
			break;
		case ENOTDIR: 
			p=messages[26]; 
			break;
		case EISDIR:  
			p=messages[27]; 
			break;
		case EINVAL:  
			p=messages[28]; 
			break;
		case ENFILE:  
			p=messages[29]; 
			break;
		case EMFILE:  
			p=messages[30]; 
			break;
		case ENOTTY:  
			p=messages[31]; 
			break;
		case ETXTBSY: 
			p=messages[32]; 
			break;
		case EFBIG:   
			p=messages[33]; 
			break;
		case ENOSPC:  
			p=messages[34]; 
			break;
		case EROFS:   
			p=messages[35]; 
			break;
		case EMLINK:  
			p=messages[36]; 
			break;
		case EFAULT:  
			p=messages[37]; 
			break;
		case 1000+1:  
			p=messages[38];
			break;
		default:      
			p=(char *)Conv(errno,5,10,0);
			break;
		}
		dps(p);
		if( flag ) {
			dpi();  
			dpp(0,ysize-1);  
			dpo(el);
		}
	}
	return( errno );
}
ind_lerr(n,f)  {
	errno=1000+n; 
	ind_err((char*)0,f);
}
