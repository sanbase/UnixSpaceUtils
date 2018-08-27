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

	Last modification:Fri Mar 23 14:23:20 2001
			Module:conv.c
*/
/* ПРЕОБРАЗОВАТЕЛЬ ЦЕЛЫХ: ЧИСЛО --> ЕГО ТЕКСТОВОЕ ПРЕДСТАВЛЕНИЕ */
/* на выходе: указатель на строку длиной 'length', дополненную \000,
              содержащую преобразованное по данной системе счисления число,
              начинающееся спереди символами 'begins' */

char *Conv (value, length, radix, begins)
unsigned value;                     /* преобразуемое значение */
int length;                         /* длина выходного поля */
int radix;                     /* основание системы счисления */
char begins;                        /* чем дополнить число спереди */
{
	char *ptr;
	static char buf[16+1];
	register i;

	buf[16]=0;
	for(i=0; i<16; ++i)  buf[i]=begins;
	ptr = &buf[16];
	do {
		*--ptr = (i=value%radix) < 10 ? i+'0' : i-10+'A';
		value /= radix;
	} 
	while(value);
	return( begins ? &buf[16-length] : ptr );
}
