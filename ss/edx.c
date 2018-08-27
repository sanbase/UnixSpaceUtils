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

	Last modification:Fri Mar 23 14:23:49 2001
			Module:edx.c
*/
char *hdr="edx v. 1.0  30.10.90  San (Alex Lashenko) lashenko@unixspace.com";

#include <tty_codes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>

char *status(),*Conv(),*get_uid_name();

long seek_abs,size_file,atol();
unsigned char seek;
int fd,x0,x1,x2,y0,bl_size,mode;
#define X0  4
#define X1 34
#define X2 64
unsigned char buf[16][16];
struct s_dpd tty;
unsigned char str_sear[33];
struct Edit { long pos; unsigned char ch; };
int last_red;
long beg=0,end1=0;
char mark=0;
#define MOD_SIZE 1024
struct Edit red[MOD_SIZE];
char *getcwd();
extern char insert;

extern struct mouse ev;
int X_size,Y_size;

main(argc,argv)
int argc;
char **argv;
{
	int rd_only=0,ch,i;
	char str[256],name[80];
	char first_time=1;

	if(argc<2)
	{
		printf("Binary Editor EDX\n");
		printf("edx [-] file_name\n");
/*
		printf("Это программа просмотра и модификации файлов\n");
		printf("Запуск: edx [-] имя_файла\n");
*/
		exit(0);
	}
	*name=0;
	if(*argv[1]!='/')
	{
		getcwd(name,70);
		strcat(name,"/");
	}
	if(!strcmp(argv[1],"-")) { 
		rd_only=1; 
		strcat(name,argv[2]); 
	}
	else strcat(name,argv[1]);
	if((fd=open(name,O_RDWR))<0)
	{
		rd_only=1;
		if((fd=open(name,O_RDONLY))<0) goto EXIT;
	}
#ifdef X11
	X_size=80,Y_size=25;
#endif
	dpbeg("Edx"); dpp(0,0); dpo(es); scrbufout();
	dpinit();
	insert=0;
	tty=dpd();
	tty.xdim=80;
	tty.ydim=25;
	/*        tty.ydim=10; */
	for(i=0;i<MOD_SIZE;i++) red[i].pos=-1l;
	last_red=0;
FIRST:
BEGIN:
	setcolor(0216);
	box(0,0,tty.xdim-1,tty.ydim-4,' ');

	dpp(0,3); 
	dpo('╟');
	dpn(tty.xdim-3,'─'); 
	dpo('╢');

	dpp(tty.xdim-23,3);
	dpo('┬');
	vert(tty.xdim-23,4,tty.ydim-9);
	dpp(tty.xdim-23,tty.ydim-5);
	dpo('╧');

	dpp(13,3);
	dpo('┬');
	vert(13,4,tty.ydim-9);
	dpp(13,tty.ydim-5);
	dpo('╧');

	setcolor(017);
	dpp((tty.xdim-strlen(name)-2)/2,0);
	dps("< "); 
	dps(name);
	dps(" >");
	strcpy(str,status(name));
	dpp((tty.xdim-strlen(str))/2,2); 
	dps(str);
	setcolor(017);
	dpp(X0,tty.ydim-4);
	dps("Short =");
	dpp(X1,tty.ydim-4);
	dps("Short =");
	dpp(X2,tty.ydim-4);
	dps("Short =");
	dpp(X0,tty.ydim-3);
	dps("Long  =");
	dpp(X1,tty.ydim-3);
	dps("Float =");
	dpp(X2,tty.ydim-3);
	dps("Char  =");
	hlp();
//        Set_Box_BG(0,0,tty.xdim-1,tty.ydim-4,010);
	full_draw(x0=2,x1=16,x2=tty.xdim-20,y0=4);
	for(;;)
	{
		pos(seek,1);
		if(first_time)
		{
			ch=hello();
			first_time=0;
		}
		else
		        ch=Xmouse(dpi());
		switch(ch)
		{
			long sear_seek;
			int dlina;
		case 0:
			pos(seek,0);
			if(ev.x<=13)
				break;
			if(ev.x>tty.xdim-23)
			{
				if(mode==0)
				        mode=1;
				if(ev.x>=tty.xdim-20 && ev.x<=tty.xdim-4 && ev.y>=4 && ev.y<=19)
					seek=(ev.x-tty.xdim+20)+(ev.y-4)*16;
			}
			else
			if(ev.x<tty.xdim-23)
			{
				if(mode==1)
				        mode=0;
				if(ev.x>=16 && ev.y>=4 && ev.y<=19)
					seek=((ev.x-16)*2)/5+(ev.y-4)*16;
			}
			break;
		case F3:
			write_mod();
			goto BEGIN;
		case 1:
			{
				short nb,c;

				nb=get_box(0,0,tty.xdim,tty.ydim);
				dpp(0,0);
				setcolor(03);
				dpo(es);
				scrbufout();
				put_box(0,0,nb);
				free_box(nb);
			}
			goto BEGIN;
		case F5:
			setcolor(014);
			dpp(2,2);
			strcpy(str,Conv(seek_abs+seek,10,10,'0'));
			if(dpedit(0,str,10,10,2,2,10)==F8) break;
			seek_abs=atol(str);
			if(mark)
				break;
			else
				goto BEGIN;
		case F9:                  /* начать или закончить пометку */
			if(!mark)
			{
				end1=(beg=seek_abs+seek)+1;
				mark=1;
			}
			else
				mark=0;
			goto BEGIN;
		case F8:                  /* отменить пометку */
			end1=beg;
			mark=0;
			goto BEGIN;
		case F12:                 /* сбросить дамп в файл */
			write_dump();
			goto BEGIN;
		case F7:
			{
				long seek_std;
				int i,nb,x;
				pos(seek,0);
SEARCH:
				setcolor(0160);
				if(mode)
				{
					nb=get_box(28,y0+1,23,5);
					x=28;
#ifdef X11
					BOX(28,y0+1,20,4,' ',0x442e,0x442e);
#else
					BOX(28,y0+1,20,4,' ',017,0160);
#endif
					dpp(35,y0+2);
					dps("Pattern");
					dpp(30,y0+3);
					setcolor(016);
					dps("                ");
				}
				else
				{
					nb=get_box(12,y0+1,39,5);
					x=12;
#ifdef X11
					BOX(12,y0+1,36,4,' ',0x442e,0x442e);
#else
					BOX(12,y0+1,36,4,' ',017,0160);
#endif
					dpp(35,y0+2);
					dps("Pattern");
					dpp(14,y0+3);
					setcolor(016);
					dps("                                ");
				}
				cconv(str_sear,mode?0:16);
EDIT:
				help_line("Enter end_pattern Page/Up find_to_begin Page/Do find_to_end F8 erase F10 exit");
				i=dpedit(0,str_sear,mode?16:32,mode?16:32,mode?30:14,y0+3,mode?0:22);
	                        hlp();
				if(i==F8) { 
					*str_sear=0; 
					goto NO; 
				}
				if(i==F10) { 
					if(!mode) uconv(str_sear,16); 
					goto NO; 
				}
				if(i==il) goto DOWN;
				if(i==dl) goto UP;
				put_box(x,y0+1,nb);
				free_box(nb);
				nb=get_box(19,y0+1,33,5);
				x=19;
				setcolor(0160);
#ifdef X11
				BOX(19,y0+1,30,4,' ',0x442e,0x442e);
#else
				BOX(19,y0+1,30,4,' ',017,0160);
#endif
				dpp(21,y0+2); 
				dps("PageUp-назад  Home-сначала");
				dpp(21,y0+3); 
				dps("PageDn-вперед End -с конца");
				help_line(" Down back Up forward Page/Up form_end Page/Do from_begin F10 exit");
GET:
				i=Xmouse(dpi());
				hlp();
				switch(i)
				{
				case ru:
					sear_seek=0;
					i=1;
					break;
				case rd:
					sear_seek=size_file-1;
					i=0;
					break;
				case il:
DOWN:
					sear_seek=seek_abs+seek+1;
					i=1;
					break;
				case dl:
UP:
					sear_seek=seek_abs+seek-1;
					i=0;
					break;
				case F10:
					goto NO;
				default:
					dpo(bl);
					goto GET;
				}
				dlina=strlen(str_sear);
				seek_std=seek_abs;
				if(!mode) uconv(str_sear,16);
				if(search(sear_seek,i,mode?dlina:dlina/2)<0)
				{
					dpp(30,y0+3);
					dps("   Not found.   ");
					Xmouse(dpi());
				}
NO:
				setcolor(0);
				put_box(x,y0+1,nb);
				free_box(nb);
				if(seek_std!=seek_abs)
					goto BEGIN;
				break;
			}
		case ta:
			mode=!mode;
			break;
		case Cr:
CR:
			pos(seek,0);
			if(seek==(tty.ydim-9)*16-1)
			{
				seek=0; 
				goto IL;
			}
			/*if(seek_abs+seek <= size_file-1)*/
			seek++;
			break;
		case cl:
			pos(seek,0);
			if(!seek)
			{
				if(!seek_abs) { 
					dpo(bl); 
					break; 
				}
				seek=(tty.ydim-9)*16-1;
				goto DL;
			}
			seek--;
			break;
		case cd:
			pos(seek,0);
			if(seek>=(tty.ydim-10)*16)
			{
				if(scroll(1)) goto BEGIN;
				break;
			}
			seek+=16;
			break;
		case cu:
			pos(seek,0);
			if(seek<16)
			{
				if(!seek_abs) { 
					dpo(bl); 
					break; 
				}
				if(scroll(0)) goto BEGIN;
				break;
			}
			seek-=16;
			break;
		case ru:
			pos(seek,0);
			seek=0;
			if(mark)
				break;
			else
				goto BEGIN;
		case rd:
			pos(seek,0);
			seek=(tty.ydim-9)*16-1;
			if(mark)
				break;
			else
				goto BEGIN;
		case il:
IL:
			seek_abs+=(tty.ydim-9)*16;
			if(mark)
				break;
			else
				goto BEGIN;
		case dl:
DL:
			if(!seek_abs) break;
			seek_abs-=(tty.ydim-9)*16;
			if(seek_abs<0l) seek_abs=0l;
			if(mark)
				break;
			else
				goto BEGIN;
		case F10:
			write_mod();
			goto EXIT;
		default:
			if(rd_only || ch<' ') { 
				dpo(bl); 
				break; 
			}
			if(!mode)
				if(minied(&ch)<0) { 
					dpo(bl); 
					break; 
				}
			modify(seek_abs+seek,ch);
			goto CR;
		}
		if(mark)
		{
			/* несколько нерационально, но иначе пришлось
						бы отслеживать изменение помеченной области
						при любом передвижении курсора */
			if(seek_abs+seek<beg)
				beg=seek_abs+seek;
			else
				if(seek_abs+seek>end1)
					end1=seek_abs+seek;
			goto BEGIN;
		}
	}
EXIT:
	setcolor(03);
	dpend();
	exit(0);
}
char *status(name)
char *name;
{
	static char str[80];
	struct stat buf;
	if(!stat(name,&buf))
	{
		size_file=buf.st_size;
		strcpy(str,"User:");
		strcat(str,get_uid_name(buf.st_uid));
		strcat(str," Mode:");
		strcat(str,Conv((long)buf.st_mode,3,8,' '));
		strcat(str," Type:");
		switch(buf.st_mode&0170000)
		{
		case S_IFDIR:   /* Справочник   */
			strcat(str,"directory");
			break;
		case S_IFCHR:
			strcat(str,"special byte");
			break;
		case S_IFBLK:
			strcat(str,"special block");
			size_file=0xfffffff;
			break;
		case S_IFIFO:
			strcat(str,"chanal");
			break;
			/*case S_IFNAM:
			strcat(str,"список");
			break;*/
		default:
			if(buf.st_mode&0100) strcat(str,"execut");
			else
				strcat(str,"data");
			strcat(str," Size=");
			strcat(str,Conv((long)size_file,6,10,' '));
		}
		return(str);
	}
}
full_draw()
{
	register int i;

	lseek(fd,seek_abs,0);
	if((bl_size=read(fd,buf,(tty.ydim-9)*16))<0)
		bl_size=0;
	for(i=bl_size;i<(tty.ydim-9)*16;i++)
	        buf[i/16][i%16]=0;
	for(i=0;i<tty.ydim-9;i++)
		draw_line(i,16);
	return(0);
}

draw_line(i,l)
int i,l;
{
	register j;
	unsigned char str[32];

	setcolor(0216);
	dpp(0,y0+i);
	dpo('║');
	dpp(tty.xdim-2,y0+i);
	dpo('║');
	vert(tty.xdim-23,y0+i,1);
	vert(13,y0+i,1);

	dpp(x0,y0+i);
#ifdef X11
	setcolor(0x400+0213);
#else
	setcolor(0213);
#endif
	sprintf(str,"%010ld",seek_abs+i*16);
	dps(str);
	dpp(x1,y0+i);
	for(j=0;j<l;j++)
		draw_int(i,j,0);
	dpp(x2,y0+i);
	for(j=0;j<l;j++)
		draw_char(i,j,0);
}

#define if_mark(i,j) ((seek_abs+i*16+j>=beg && seek_abs+i*16+j<end1)?1:0)

draw_int(i,j,arg)
int i,j,arg;
{
	unsigned char line[16],ch;
	int poz,color;
	if((poz=check_mod(seek_abs+i*16+j))<0)
	{
		ch=buf[i][j];
		color=if_mark(i,j)?0x70:0202;
		if(i*16+j>=bl_size) {
			setcolor(if_mark(i,j)?0x72:0202);
			dps("__");
			goto END;
		}
	}
	else
	{
		ch=red[poz].ch;
		color=if_mark(i,j)?0x7f:0212;
	}
	if(arg)
	{
		if(mode) color=if_mark(i,j)?0x0f:0160;
		else     color=if_mark(i,j)?0x0f:0340;
		color|=0x100;
	}
	sprintf(line,"%02x",ch);
#ifdef X11
		color+=0x400;
#endif
	setcolor(color);
	dps(line);
END:
	if(j%2) dpo(Cr);
}
draw_char(i,j,arg)
int i,j,arg;
{
	unsigned char ch;
	int pos,color;
	ch=buf[i][j];
	if((pos=check_mod(seek_abs+i*16+j))<0)
		color=if_mark(i,j)?0x70:0203;
	else
	{
		ch=red[pos].ch; 
		color=if_mark(i,j)?0x7f:0213;
	}
	if(arg)
	{
		if(!mode) color=if_mark(i,j)?0x0f:0160;
		else     color=if_mark(i,j)?0x0f:0340;
	}
#ifdef X11
		color+=0x400;
#endif
	setcolor(color);
	if(i*16+j>=bl_size && pos<0) ch=' ';
	else
		if(ch<' '|| ch==0177 || ch==0377)  ch='.';
	dpo(ch);
}

union zn {
	unsigned char chr[4]; 
	short sh[2]; 
	long lo; 
	float ft; 
};

pos(seek,arg)
short seek;
int arg;
{
	int i,j,x,y;
	i=seek>>4;
	j=seek&017;
	y=tty.ydim-4;
	if(arg)
	{
		int poz;
		unsigned char str[80],str1[4];
		union zn *val;

		str1[0]=(poz=check_mod(((seek_abs+seek)/4)*4+0))<0?buf[i][(j/4)*4+0]:red[poz].ch;
		str1[1]=(poz=check_mod(((seek_abs+seek)/4)*4+1))<0?buf[i][(j/4)*4+1]:red[poz].ch;
		str1[2]=(poz=check_mod(((seek_abs+seek)/4)*4+2))<0?buf[i][(j/4)*4+2]:red[poz].ch;
		str1[3]=(poz=check_mod(((seek_abs+seek)/4)*4+3))<0?buf[i][(j/4)*4+3]:red[poz].ch;

		setcolor(012);
		dpp(2,2);
		dps(Conv(seek_abs+seek,10,10,'0'));
		/*                val=&buf[i][(j/4)*4]; */
		val=(union zn *)str1;
		setcolor(07);
		sprintf(str,"%06d",val->sh[(j/2)%2]);
		dpp(X0+8,y);
		dps(str);
		sprintf(str,"%04x",(unsigned short)val->sh[(j/2)%2]);
		dpp(X1+8,y);
		dps(str);
		sprintf(str,"%06o",(unsigned short)val->sh[(j/2)%2]);
		dpp(X2+8,y);
		dps(str);
		sprintf(str,"%011ld",val->lo);
		dpp(X0+8,y+1);
		dps(str);
		if(((str1[3]&0177)==0177) && str1[2]==0377 || str1[3]==0377 && str1[2]>0x7f)
			sprintf(str,"- Нет кода  ");
		else
			if(str1[3]==0x7f && str1[2]>0x7f)
				sprintf(str,"- Нет кода  ");
			else
				sprintf(str,"%012g",val->ft);
		dpp(X1+8,y+1);
		dps(str);
		sprintf(str,"%3d",val->chr[j%4]);
		dpp(X2+8,y+1);
		dps(str);
	}
	x=x1+(j>>1)*5+(2*(j&1));
	dpp(x,y0+i);
	draw_int(i,j,arg);
	dpp(x2+j,y0+i);
	draw_char(i,j,arg);
	flush_dpi();
	if(!mode) dpp(x,y0+i);
	else dpp(x2+j,y0+i);
}
scroll(arg)
int arg;
{
	register i;
	if(arg)
	{
		/*if(seek_abs+seek+16>=size_file) return(0);*/
		seek_abs+=16;
	}
	else  if(seek_abs>=16)  seek_abs-=16;
	else
	{
		seek_abs=0; 
		return(1); 
	}
	lseek(fd,seek_abs,0);
	if((bl_size=read(fd,buf,256))<0)
		bl_size=0;
	for(i=bl_size;i<256;i++) buf[i/16][i%16]=0;
	if(arg)
	{
		dpp(0,y0); 
		dpo(dl);
		dpp(0,y0+tty.ydim-10);
		dpo(il);
		draw_line(tty.ydim-10,16);
	}
	else
	{
		dpp(0,y0+tty.ydim-10);
		dpo(dl);
		dpp(0,y0); 
		dpo(il);
		draw_line(0,16);
	}
	return(0);
}
search(sear_seek,arg,len)
long sear_seek;
int arg;
int len;
{
	char buf[256];
	int i,j=0;
	int size;
	if(!arg)
		if((sear_seek-=(tty.ydim-9)*16)<0) sear_seek=0l;
	setcolor(014);
BEGIN:
	lseek(fd,sear_seek,0);
	while((size=read(fd,buf,(tty.ydim-9)*16))>0)
	{
		if((j%1024)==0)
		{
		        dpp(2,2);
		        dps(Conv(sear_seek,10,10,'0'));
		        scrbufout();
			j=0;
		}
		j++;
		if(seek_abs<0) break;
		if((i=search_line(str_sear,buf,(tty.ydim-9)*16,arg,len))<0)
			sear_seek+=arg?size:-size;
		else
		{
			if(arg && size-i<len)
			{
				sear_seek+=i-1;
				goto BEGIN;
			}
			if(!arg)
			{
				if(i<len-1)
				{
					sear_seek-=len-i;
					goto BEGIN;
				}
				i-=len-1;
			}
			seek_abs=((sear_seek+i)/((tty.ydim-9)*16)*(tty.ydim-9)*16);
			seek=(sear_seek+i)%((tty.ydim-9)*16);
			return(1);
		}
		if(!arg)
		{
			if(sear_seek==-size)
				break;
			if(sear_seek<0) sear_seek=0;
			lseek(fd,sear_seek,0);
		}
	}
	return(-1);
}
/* поиск вхождения образца в блок */
search_line(a,b,l,arg,len)
unsigned char *a,*b;       /* a - образец, b - блок */
int l;                     /* размер блока */
int arg;                   /* в какую сторону искать */
int len;                   /* размер образца */
{
	register short i,j;
	int first;
	if(arg)
		for(i=0;i<=l;i++)
		{
			if(a[0]==b[i])
			{
				if(l<i+len) return(i);
				if(compare(a,b+i,len)) continue;
				return(i);
			}
		}
	else
		for(i=l;i>=0;i--)
		{
			if(b[i]==a[len-1])
			{
				if(i-len<0) return(i);
				if(compare(a,b+i-len+1,len)) continue;
				return(i);
			}
		}
	return(-1);
}
compare(a,b,len)
unsigned char *a,*b;
int len;
{
	register int i;
	for(i=0;i<len;i++)
		if(a[i]!=b[i]) return(1);
	return(0);
}

char *obr="0123456789abcdefABCDEF";

uconv(str,dig)
unsigned char *str;
int dig;
{
	unsigned char line[256];
	int i,j;
	for(i=0;i<(tty.ydim-9)*16;i++) line[i]=0;
	for(i=0;i<(tty.ydim-9)*16 && str[i];i++)
	{
		for(j=0;obr[j] && str[i]!=obr[j];j++);
		if(!obr[j]) j=0;
		if(j>15) j-=6;
		line[i/2]+=i%2?j:j<<4;
	}
	for(j=0;j<i/2;j++)
		str[j]=line[j];
	str[j]=0;
}
cconv(str,dig)
unsigned char *str;
int dig;
{
	unsigned char line[256];
	register int i,dlina;
	dlina=strlen(str);
	if(!dig)
	{
		for(i=0;i<dlina;i++)
			if(str[i]<' ') str[i]='.';
		return;
	}
	for(i=0;i<dlina;i++)
	{
		line[2*i]=obr[str[i]/dig];
		line[2*i+1]=obr[str[i]%dig];
	}
	line[2*i]=0;
	strcpy(str,line);
}
hlp()
{
	char *str="F3 Write F5 Seek F7 Find F8 Delete mark F9 Mark F10 Exit F12 Write dump.";
	help_line(str);
}
#define LINESIZE 128
/* функция центровки сообщения */
char *centr(str)
char *str;
{
	int size,num;
	register char *ch;
	register int i;
	char str1[LINESIZE+1];

	if(!(size=strlen(str)))
		return(str);
	num=tty.xdim-size-1;      /* столько пробелов добавить */
	while(num>0)
	{
		size=strlen(str)+1;
		for(i=0,ch=str1;i<size;ch++,i++)
		{
			if(str[i]==' ' && num)
			{
				*ch=' '; 
				ch++;
				num--;
				for(;str[i]==' ' && i<size;i++,ch++)
					*ch=str[i];
			}
			*ch=str[i];
		}
		strcpy(str,str1);
	}
	return(str);
}

static char *Keys[]=
{
	"Esc",  "",    "",           "F12",     "F3", "",      "",    "",
	"Del",  "Tab", "Ctrl/Enter", "F1",      "F2", "Enter", "F4",  "F5",
	"F6",   "F7",  "End",        "Home",    "",   "",      "",    "",
	"Ins",  "",    "Page/Up",    "Page/Do", "F8", "F9",    "F10", "F11"
};

help_line(line)
char *line;
{
	register int i,j,k;
	char word[80],str[128];
	struct s_dpd tty;

	strcpy(str,line);
	centr(str);
	tty=dpd();
	dpp(0,tty.ydim-1);
#ifdef X11
	setcolor(072+0x100+0x400);
#else
	setcolor(060);
#endif
	dps(str);
	for(i=j=0;str[i];i++)
	{
		while(str[i] && str[i]==' ') i++;
		while(str[i] && str[i]!=' ')
			word[j++]=str[i++];
		word[j]=0;
		j=0;
		for(k=0;k<32;k++)
			if(!strcmp(word,Keys[k]))
			{
				dpp(i-strlen(word),tty.ydim-1);
				setcolor(07);
				dps(word);
				break;
			}
	}
}
cmp(a,b)
struct Edit *a,*b;
{
	return(a->pos-b->pos);
}

modify(pos,ch)
long pos;
unsigned char ch;
{
	register i;
	if((i=check_mod(pos))<0)
		i=last_red++;
	if(i==MOD_SIZE)
	{
		dpp(0,23); 
		setcolor(014);
		dpo(es); 
		dpo(bl);
		dps("Исчерпан массив модифицированных данных! Запишите изменения.");
		Xmouse(dpi());
		return(-1);
	}
	red[i].pos=pos; 
	red[i].ch=ch;
	qsort(red,i,sizeof(struct Edit),cmp);
	return(0);
}
check_mod(pos)
long pos;
{
	register int i;
	for(i=0;red[i].pos>=0l && i<MOD_SIZE;i++)
		if(red[i].pos==pos) return(i);
	return(-1);
}
minied(ch)
unsigned char *ch;
{
	int  i,j,pos=0;
	unsigned char zn[2];
	j=*ch;
	goto CONT;
	while(1)
	{
		j=dpi();
CONT:
		switch(j)
		{
		default:
			for(i=0;obr[i];i++) if(j==obr[i]) break;
			if(!obr[i])
			{
				dpo(bl); 
				return(-1);
			}
			if(i>15) i-=6;
			zn[pos++]=i;
			dpo(j);
			if(pos==2) goto END;
		}
	}
END:
	*ch=(zn[0]<<4)+zn[1];
	return(0);
}
yes()
{
	int i;
	i=dpi();
	return(i=='y' || i=='Y' || i=='д' || i=='Д');
}

write_dump()
{
	int i,j;
	char buffer[BUFSIZ];

	if(beg==end1)
	{
		dpo(bl);
		return(0);
	}
	j=get_box(12,y0+4,39,5);
#ifdef X11
	BOX(12,y0+1,36,4,' ',0x442e,0x442e);
#else
	BOX(12,y0+1,36,4,' ',017,0160);
#endif
	dpp(22,y0+2);
	dps("Filename for dump");
	setcolor(016);
ED:
	*buffer=0;
	i=dpedit(0,buffer,64,32,14,y0+3,0);
	switch(i)
	{
	case F10:
		goto END;
	case rn:
		{
			int fd1;
			long seek1;
			int nb1;

			if(access(buffer,06))
			{
				if((fd1=creat(buffer,0660))<0)
				{
ERR:
					dpo(bl);
					goto ED;
				}
			}
			else
				if((fd1=open(buffer,O_WRONLY))<0)
					goto ERR;
			nb1=get_box(15,y0+2,39,4);
#ifdef X11
			BOX(15,y0+2,37,3,' ',0x442e,0x442e);
#else
			BOX(15,y0+2,37,3,' ',017,0160);
#endif
			dpp(17,y0+3);
			dps("Position in this file");
			setcolor(016);
ED1:
			*buffer=0;
			i=dpedit(0,buffer,11,11,39,y0+3,'c');
			switch(i)
			{
			case F10:
				close(fd1);
				put_box(15,y0+2,nb1);
				free_box(nb1);
				goto ED;
			case rn:
				put_box(15,y0+2,nb1);
				free_box(nb1);
				lseek(fd1,atol(buffer),0);
				break;
			default:
				goto ED1;
			}
			for(i=0;i<last_red && beg>red[i].pos;i++);
			lseek(fd,beg,0);
			for(seek1=beg;seek1<end1;seek1+=BUFSIZ)
			{
				int size;

				bzero(buffer,BUFSIZ);
				read(fd,buffer,BUFSIZ);
				size=(seek1+BUFSIZ<end1?BUFSIZ:end1-seek1);
				for(;i<last_red && red[i].pos<seek1+size;i++)
					buffer[red[i].pos-seek1]=red[i].ch;
				write(fd1,buffer,size);
			}
			close(fd1);
		}
		goto END;
	default:
		dpo(bl);
		goto ED;
	}
END:
	put_box(12,y0+1,j);
	free_box(j);
	return(0);
}

write_mod()
{
	int i,j;
	if(!last_red) return;
	pos(seek,0);
	j=get_box(28,y0+1,25,5);
#ifdef X11
	BOX(28,y0+1,23,4,' ',0x442e,0x442e);
#else
	BOX(28,y0+1,23,4,' ',07,0160);
#endif
	dpp(29,y0+2);
	dps(" Write. Are you sure?");
	dpp(36,y0+3);
	dps("[y/n]");
	if(yes())
	{
		for(i=0;i<MOD_SIZE;i++)
		{
			if(red[i].pos<0l) continue;
			lseek(fd,red[i].pos,0);
			write(fd,&red[i].ch,1);
			red[i].pos=-1l;
		}
		last_red=0;
	}
	put_box(28,y0+1,j);
	free_box(j);
}

struct name {
	int uid; 
	char name[16]; 
};
struct name names[16];
int l_t;
char *get_uid_name(uid)
int uid;
{
	register i,j;
	int k,l,fd;
	unsigned char buf[256],str[10];
	for(l=0;l<16 && names[l].uid;l++)
		if(names[l].uid==uid+1) return(names[l].name);
	if(l>=16)  l=l_t++;
	if(l>=16)  l=l_t=0;
	if((fd=open("/etc/passwd",O_RDONLY))<0) return("permission denaid");
	for(;;)
	{
		for(i=0;i<256&&read(fd,buf+i,1)&&buf[i]!='\n';i++);
		if(!i || i==256) return("unknown");
		for(j=0;j<i && buf[j]!=':';j++);
		buf[j]=0;
		for(j++;j<i && buf[j]!=':';j++);
		for(k=0,j++;j<i && buf[j]!=':';j++) str[k++]=buf[j];
		str[k]=0;
		if(uid!=atoi(str)) continue;
		names[l].uid=uid+1;
		for(i=0;i<15 && buf[i]; i++)
			names[l].name[i]=buf[i];
		names[l].name[i]=0;
		close(fd);
		return(names[l].name);
	}
}
/*
( Warning! This programm have a mistake for backfind.
	I have not time for find and correcting it, sorry.
		You may try to do it yourself. )
*/

char *Conv (value, length, radix, begins)
unsigned value;                     /* преобразуемое значение */
int length;                         /* длина выходного поля */
register radix;                     /* основание системы счисления */
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
extern short x_c,y_c,l_x;
hello()
{
	int x,y,x0,y0,f,c;

	x=l_x-28;
	y=1;
	x0=x_c;
	y0=y_c;
	f=get_box(x,y,28,8);
#ifdef X11
	BOX(x,y,27,6,' ',0x442e,0x442e);
#else
	BOX(x,y,27,6,' ',07,016);
#endif
	dpp(x+2,y+1);
	dps("   Welcome to EDX");
	dpp(x+2,y+2);
	dps("Autor: Alex L. Lashenko");
	dpp(x+2,y+3);
	dps("  Toronto ON, Canada");
	dpp(x+2,y+4);
	dps("E-mail:san@unixspace.com");
	dpp(x0,y0);
	c=Xmouse(dpi());
	put_box(x,y,f);
	free_box(f);
	return(c);
}
