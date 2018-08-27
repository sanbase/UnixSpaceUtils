/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2002 ConteXt Technology inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (2000-2002)
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

		Last modification:Fri Nov  8 09:31:20 2002
			Module:dpi.c
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <sys/utsname.h>
#include <setjmp.h>
#include "tty_codes.h"

#ifdef X11R6
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include "Xfont.h"

static Display         *display;
static Window          window;
static int             screen;
static unsigned        long    foreground=1;
static unsigned        long    background=0;
static XEvent          event;
static XSizeHints      hints;
static Font            font;
extern int errno;

int X_size,Y_size;
#define NUMCOLORS       64
static XColor Xcolors[NUMCOLORS];
static GC Gcolors[NUMCOLORS];
static XGCValues       values;
static Atom wm_del_win;
unsigned long black_pixel, white_pixel ;
#define SHIFTX  0
#define SHIFTY 16
int ADD_X_SIZE;
int ADD_Y_SIZE;
#endif

int x11;
static int curs_x=-1,curs_y=-1;
int _no_cursor=0;
static char *x_code=
" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
static int shft=0;
static int cshft;
static int dp_act=0;
void zero_scrold();
int exit_flg=0;
static int koi8=0;
//#define Cr cr

char PC;
char *BC;
char *UP;
extern char mask[256];
static int scr_out;


int l_y,l_x;
int rus_lat;
int rus_lat_out;
int x_c=0,y_c=0;
int x_p=-100,y_p=-100;

unsigned int S_color=0;
static int _p_color=0;

static char tabl[96];
static char tabl2[96];

static char *_buf;
struct mouse ev;
static int fd_std,stdin_std=0,fd_macro=0;
struct s_tdescr t;
static void m_finish();
static char menu_str[128];
int font_H=20;
int font_L=11;
int LL=11;
int HH=20;


#undef c
#undef s
#undef e

static char   *buf;
static int  lbuf  = 1000;
static int  xbuf;


void _scrbufout();

char cbuf[300];

char *cvtin[32];
char *cvtin_[]=
{
/*       ^'@    ^'a   ^'b      F12    F3     fl  s_finish  bl  */
/*        dc     ta     lf     F1     F2     rn     F4     F5  */
/*        F6     F7     rd     ru     cd     cu     cr     cl  */
/*        ic   s_help   il     dl     F8     F9     F10    F11 */

	"DE",     0,     0,  "kB",  "k3",     0,     0,     0,
	   0,     0,     0,  "k1",  "k2",     0,  "k4",  "k5",
	"k6",  "k7",  "EN",  "HM",  "kd",  "ku",  "kr",  "kl",
	"IN",  "kC",  "PD",  "PU",  "k8",  "k9",  "k0",  "kA"
};

char *cvtout[32];
char *cvtout_[]=
{
/*         -     bb     bf     -      -      -      -      bl */
/*         -     ta     lf     -      -      rn     -      -  */
/*         -     el     rd     ru     cd     cu     cr     cl */
/*        ic     dc     il     dl     es     Cf     se     so */

	 "CX",  "bb",  "bf",  "Ca",  "Cb",  "Cn",   "",    "",
	 "",    "",    "",    "AF",  "AB",    "",    "",    "",
	 "cm",  "ce",  "sr",  "sf",  "kd",  "ku",  "kr",   "kl",
	 "ic",  "dc",  "al",  "dl",  "cd",  "Cf",  "se",   "so"
};

void end_pr(int i)
{
	dpend();
	exit(i);
}

#ifdef X11R6
static XColor Color(int r,int g,int b)
{
	XColor c;
	bzero(&c,sizeof c);
	c.red  =r<<8;
	c.blue =b<<8;
	c.green=g<<8;
	return(c);
}
#endif
static char *Mf,*Mb, *Is,*Ti,*Ks,*Fs,*Te,*Ke;
static int Nf,Nb;
static jmp_buf ON_alrm;
static int am;

static void onalrm(int sig)
{
	longjmp(ON_alrm,1);
}

int dpbeg(char *name)
{

	char *bufc,*c;
	int i;

	_buf=(char *)calloc(1024,1);
	bzero(_buf,1024);
	buf=_buf;
	memcpy(tabl," !—#$%&ç()*+,-.î0123456789í<=>žê€–„…”ƒ•ˆ‰Š‹ŒŽŸ‘’“†‚œë‡èíé^ê` ¡æ¤¥ä£å¨©ª«¬­®¯ïàáâã¦¢ìë§˜™~",96);
	memcpy(tabl2,tabl,96);
	if(getenv("DESTDIR")!=NULL)
	{
		sprintf(buf,"%s/lib/.%s.t",getenv("DESTDIR"),getenv("TERM"));
	}
	else
	{
		sprintf(buf,"/usr/local/lib/.%s.t",getenv("TERM"));
	}
	if((i=open(buf,0))>0)
	{
		read(i,tabl,96);
		close(i);
	}
	i=strlen(buf)-1;
	buf[i]='u';
	if((i=open(buf,0))>0)
	{
		read(i,tabl2,96);
		close(i);
	}
	else
		memcpy(tabl2,tabl,96);

	if((c=getenv("HOME"))!=NULL)
	{
		strcpy(buf,c);
		strcat(buf,"/.");
		if((c=getenv("TERM"))!=NULL)
		{
			strcat( buf, c);
			strcat(buf,".t");
			if((i=open(buf,0))>0)
			{
				read(i,tabl,96);
				close(i);
			}
			buf[strlen(buf)-1]='u';
			if((i=open(buf,0))>0)
			{
				read(i,tabl2,96);
				close(i);
			}
		}
	}
#ifdef X11R6
	display = XOpenDisplay("");
	if (display == 0)
#endif
	{
		x11=0;

		if(name!=NULL && !strcmp(name,"stderr"))
			scr_out=2;
		else    scr_out=1;
		x_c=y_c=S_color=0;
		fflush(stdout);
		if(dp_act)
			return(1);
		bufc=(char *)calloc(1024,1);
		if (!(c=getenv("TERM")) || !tgetent(bufc, c))
			goto bend;
		c=cbuf;

		Is=tgetstr("is",&c);
		Ti=tgetstr("ti",&c);
		Ks=tgetstr("ks",&c);
		Fs=tgetstr("fs",&c);
		Te=tgetstr("te",&c);
		Ke=tgetstr("ke",&c);

		Mb=tgetstr("Mb",&c);
		Mf=tgetstr("Mf",&c);
		for(i=0;i<32;i++)
		{
			if(!cvtin_[i]) { 
				cvtin[i]=NULL; 
				continue;
			}
			cvtin[i] = tgetstr(cvtin_[i], &c);
			if (!cvtin[i])
			{
				if(i==ru)
				{
					cvtin[i] = tgetstr("kh", &c);
					if (cvtin[i])
					{
						cvtin[i]=NULL;
						continue;
					}
				}
				cvtin[i]=NULL;
			}
		}
		for(i=0;i<32;i++)
		{
			if(!cvtout_[i]) { 
				cvtout[i]=NULL; 
				continue; 
			}
			cvtout[i] = tgetstr(cvtout_[i], &c);
			if (!cvtout[i])
			{
				cvtout[i]=NULL;
				if(i==16)
					goto bend;
			}
		}
		shft=(char *)strstr(cvtout[16],"%c")!=NULL;
		if(cvtout[12]!=NULL)
			cshft=(strstr(cvtout[12],"%c")!=NULL)?32:0;
		t.param.ydim = tgetnum("li");
		t.param.xdim = tgetnum("co");

		t.ct=tgetnum("CT");
		if(t.ct<=0)
			t.ct=tgetnum("ct");
		else    if(t.ct==9) t.ct=0;
		am=tgetflag("am");

		Nf=tgetnum("Nf");
		if(Mf==NULL) Nf=0;
		Nb=tgetnum("Nb");
		if(Mb==NULL) Nb=0;

		Ttyset();
		signal(SIGQUIT,end_pr);
		signal(SIGTERM,end_pr);
		if(t.ct==15)
		{
			int i;
			char str[64],*ch;

			bzero(str,sizeof str);
			sprintf(str,"%c[Z",27);
			write(scr_out,str,3);
			bzero(str,sizeof str);
			signal( SIGALRM, onalrm );
			alarm(5);
			if(setjmp(ON_alrm))
				goto B0;
A0:
			for(i=0;i<64;i++)
			{
				read_byte((unsigned char *)str+i);
				if(str[i]=='H')
					break;
			}
			if(str[i]==27)
			{
				bzero(str,sizeof str);
				goto A0;
			}
			alarm(0);
			signal( SIGALRM, SIG_IGN );

			if((font_H=atoi(str))>0)
				HH=font_H;
			else font_H=20;
			if((ch=strchr(str,';'))==NULL)
				goto A1;
			if((font_L=atoi(++ch))>0)
				LL=font_L;
			else font_L=11;
			if((ch=strchr(ch,';'))!=NULL)
			{
				t.param.xdim = atoi(++ch);
				if((ch=strchr(ch,';'))!=NULL)
					t.param.ydim = atoi(++ch);
				l_y=t.param.ydim-1;
				l_x=t.param.xdim-1;
			}
			umask(0);
#ifdef GCC
			if(!access(".log",F_OK) && ((ch=getenv("SSH_CLIENT"))!=NULL || (ch=getenv("TELNET_CLIENT"))!=NULL))
			{
				if((i=open("/var/log/dpi.log",O_RDWR|O_CREAT,0666))>0)
				{
					char  *ch1=strchr(ch,' ');
					char str1[64];
					time_t tim;

					lseek(i,0,SEEK_END);
					if(ch1!=NULL)
						write(i,ch,ch1-ch+1);
					else
					{
						write(i,ch,strlen(ch));
						write(i," ",1);
					}
					tim=time(0);
					strcpy(str1,ctime(&tim));
					str1[20]=0;
					write(i,str1,strlen(str1));
					sprintf(str,"%d=",atoi(str));
					switch(atoi(str))
					{
					case 36:
						strcat(str,"2048x1536");
						break;
					case 24:
					case 23:
						strcat(str,"1600x1200");
						break;
					case 22:
					case 21:
					case 20:
						strcat(str,"1280x1024");
						break;
					case 19:
					case 18:
					case 17:
						strcat(str,"1152x864");
						break;
					case 16:
					case 15:
						strcat(str,"1024x768");
						break;
					case 14:
					case 13:
						strcat(str,"800x600");
						break;
					case 12:
					case 11:
						strcat(str,"640x480");
						break;
					}
					write(i,str,strlen(str));
					write(i,"\n",1);
					close(i);
				}
			}
#endif

		}
B0:
		if(t.param.ydim<=0)
			t.param.ydim=25;
		if(t.param.xdim<=0)
			t.param.ydim=75;
		l_y=t.param.ydim-1;
		l_x=t.param.xdim-1;
A1:
		ini_screen();
		xbuf=0;
		rus_lat=rus_lat_out=0;
		if(t.ct==15)
		{
			char str[64];
			cursor_visible();
			sprintf(str,"%c[i",27);
			sdps(str);
			Del_Chart();
			Del_All_Images();
			MultiColor(0,0,l_x+1,l_y+1);
			scrbufout();
		}
#ifndef SHARED
		{
			register int x,y;
			union fpic f;

			f.p.ch=' ';
			f.p.clr=color_to_clr(03);
			for(y=0;y<t.param.ydim;y++)
				for(x=0;x<t.param.xdim;x++)
					put_f(x,y,f);
		}
#endif
		setcolor(02);
		if(Is)
			sdps(Is);
		if(Ti)
			sdps(Ti);
		if(Ks)
			sdps(Ks);
		free(bufc);
		bzero(mask,256);
		dp_act=1;
		if(t.ct==15)
		{
			struct utsname node;
			char str[64],*ch;

			uname(&node);
			bzero(str,sizeof str);
			strcpy(str,node.nodename);
			Set_Title(str);
		}
		return(1);
	}
#ifdef X11R6
	else
	{
		char *c,buf[128];
		XColor exact;
		Colormap        cmap;
		unsigned int depth ;
		Window root_win;
		int root_x = -1;            /* saved co-ordinates */
		int root_y = -1;
		unsigned int root_border;
		unsigned int root_depth;
		unsigned int width;
		unsigned int height;

		x11=1;
		XGetGeometry(display,XDefaultRootWindow(display),&root_win,&root_x,&root_y,&width,&height,&root_border,&root_depth);

		LL=10;
		HH=22;

		if(!X_size)
			t.param.xdim=width/LL-1;
		else
		{
			t.param.xdim=X_size;
			width=X_size*LL;
		}
		if(!Y_size)
			t.param.ydim=(height-root_depth)/HH-1;
		else
		{
			t.param.ydim=Y_size;
			height=Y_size*HH;
		}
		t.ct=2;

		hints.x=0;
		hints.y=0;
		hints.width=width+ADD_X_SIZE;
		hints.height=height+ADD_Y_SIZE;

		/** basic colors */

		Xcolors[0 ]=Color(  0,  0,  0);
		Xcolors[1 ]=Color(  0, 64,192);
		Xcolors[2 ]=Color(  0,184,  0);
		Xcolors[3 ]=Color(  0,176,176);
		Xcolors[4 ]=Color(160,  0, 96);
		Xcolors[5 ]=Color( 90, 70,150);
		Xcolors[6 ]=Color( 33,100,130);
		Xcolors[7 ]=Color(176,192,192);

		Xcolors[8 ]=Color( 56, 60,106);
		Xcolors[9 ]=Color(152,152,255);
		Xcolors[10]=Color(128,255,128);
		Xcolors[11]=Color(128,255,255);
		Xcolors[12]=Color(255,128,128);
		Xcolors[13]=Color(240,176, 90);
		Xcolors[14]=Color(255,220,120);
		Xcolors[15]=Color(255,255,255);

		Xcolors[16]=Color( 32, 64, 64);
		Xcolors[17]=Color( 48, 96, 96);
		Xcolors[18]=Color( 64,128,128);
		Xcolors[19]=Color( 64,160,160);
		Xcolors[20]=Color(120,160,180);
		Xcolors[21]=Color(108,140,160);
		Xcolors[22]=Color( 88,120,140);
		Xcolors[23]=Color( 80,100,120);

		Xcolors[32]=Color( 80, 80,100);
		Xcolors[33]=Color( 60, 50,100);
		Xcolors[34]=Color( 10, 70,100);
		Xcolors[35]=Color( 10, 80,115);
		Xcolors[36]=Color( 10,100,125);
		Xcolors[37]=Color( 10,140,160);
		Xcolors[38]=Color(130,170,190);
		Xcolors[39]=Color(150,190,210);

		Xcolors[40]=Color(0xe8,0x10,0x78);
		Xcolors[41]=Color(0xf8,0x48,0x40);
		Xcolors[42]=Color(0xf8,0x68,0x40);
		Xcolors[43]=Color(0xf8,0x88,0x60);
		Xcolors[44]=Color(0xe0,0x98,0x58);
		Xcolors[45]=Color(0xfa,0xaa,0x5a);
		Xcolors[46]=Color(0xfa,0xd2,0xa2);
		Xcolors[47]=Color(0xe0,0xc8,0x80);

		Xcolors[48]=Color(0xb4,0xfa,0xc8);
		Xcolors[49]=Color(0xc0,0xc8,0x80);
		Xcolors[50]=Color(0xa0,0xc8,0x80);
		Xcolors[51]=Color(0x90,0xd8,0x68);
		Xcolors[52]=Color(0xb7,0xc0,0xb0);
		Xcolors[53]=Color(0x90,0xd0,0xa0);
		Xcolors[54]=Color(0x90,0xd0,0xc8);
		Xcolors[55]=Color(0x30,0xd0,0xc8);

		Xcolors[56]=Color(0x30,0xb0,0xc8);
		Xcolors[57]=Color(0x10,0xa0,0xc8);
		Xcolors[58]=Color(0x78,0x80,0xc8);
		Xcolors[59]=Color(0x90,0x70,0xc8);
		Xcolors[60]=Color(0x80,0x60,0x78);
		Xcolors[61]=Color(0x70,0x60,0x60);
		Xcolors[62]=Color(0x68,0x68,0x60);
		Xcolors[63]=Color(0x58,0x68,0x48);

		screen = DefaultScreen(display);

		depth = DefaultDepth( display, screen ) ;
		cmap = DefaultColormap( display, screen ) ;
		black_pixel = BlackPixel( display, screen ) ;
		white_pixel = WhitePixel( display, screen ) ;
		if( depth == 1 )
			for(i=0;i<NUMCOLORS;i++)
				Xcolors[i].pixel=white_pixel;
		else
			for(i=0;i<NUMCOLORS;i++)
				XAllocColor(display,cmap,&Xcolors[i]);
		errno=0;
		if(XLoadQueryFont(display,"-misc-fixed-medium-r-normal--20-200-75-75-c-100-koi8-r")!=NULL)
		{
			font=XLoadFont(display,"-misc-fixed-medium-r-normal--20-200-75-75-c-100-koi8-r");
			koi8=1;
		}
		else
			font=XLoadFont(display,"10x20");

		background=BlackPixel(display,screen);
		foreground=WhitePixel(display,screen);

		wm_del_win = XInternAtom(display,"WM_DELETE_WINDOW",False);

		window = XCreateSimpleWindow(display, DefaultRootWindow(display),
		hints.x, hints.y, hints.width, hints.height, 20,
		foreground, background);

		XSetWMProtocols(display,window,&wm_del_win,1);
		if((long)name<=0 || (long)name>(long)sbrk(0))
			name="";

		for(i=0;i<NUMCOLORS;i++)
		{
			Gcolors[i] = XCreateGC( display, window, 0l, 0 ) ;
			XSetForeground( display, Gcolors[i], Xcolors[i].pixel ) ;
			XSetBackground( display, Gcolors[i], Xcolors[i].pixel ) ;
			XSetFont(display,Gcolors[i],font);
		}

		XSetStandardProperties(display,window,name,name,0,NULL,0,&hints);
		XSelectInput(display,window,ButtonPressMask | KeyPressMask | ExposureMask);
		XMapRaised(display, window);
		XGrabButton(display,AnyButton,AnyModifier,window,1,ButtonMotionMask|ButtonPressMask, GrabModeAsync,GrabModeAsync,window,XCreateFontCursor(display,XC_draft_large));
		XMapSubwindows(display,window);
		XMapWindow(display,window);
		XNextEvent(display, &event);
		XClearWindow(display,window);
		XSetFont(display,DefaultGC(display,screen),font);

		memset(mask,0,256);
		l_y=t.param.ydim-1;
		l_x=t.param.xdim-1;

		ini_screen();
		rus_lat=rus_lat_out=0;
		setcolor(02);
		return(1);
	}
#endif
bend:
	exit(0);
}

void dpinit()
{
	dpp(0,0);
	s_dpp(0,0);
	setcolor(03);
	s_setcolor(03);
	dpo(es);
}

void dpend()
{
	char name[64];

	cursor_visible();

	if(Fs)
		sdps(Fs);
	if(Te)
		sdps(Te);
//        if(Ke)
//                sdps(Ke);

	dpp(0,l_y);
	setcolor(03);
	if(t.ct==15)
	{
		Del_Chart();
		Del_All_Images();
		DrawCircle(0,0,0,0);
		Init_Color();
	}
	MultiColor(0,0,t.param.xdim,t.param.ydim);
	scrbufout();
	free_screen();
	sprintf(name,"/var/www/docs/Files/%d.html",getpid());
	if(!access(name,W_OK))
	{
		sleep(2);
		unlink(name);
	}
	Ttyreset();
	dp_act=0;
}


/**** get terminal setting */

struct s_dpd dpd()
{
	return( t.param );
}

struct s_tdescr dptd()
{
	return(t);
}

/**** char output */
void dpo(unsigned short cc)
{
	unsigned int c=cc;

	switch(c&=0xff)
	{
		case lf:
			dpo(rn);
			return;
		case ta:
			x_c=x_c+8-x_c%8;
			goto END;
		case Cr:
			if(x_c<l_x ) x_c++;
			goto END;
		case cl:
			if(x_c) x_c--;
			goto END;
		case cd:
			if(y_c<l_y) y_c++;
			goto END;
		case cu:
			if(y_c) y_c--;
			goto END;
		case rn:
			x_c=0;
			y_c++;
			goto END;
		default:
			break;
	}
	if(c==ic || c==dc || c==il || c==dl || c==es || c==el)
	{

		if(!stdin_std)
			screen_out();
		dpo_scr(c);
		if(x11)
			return;
		if(c==el && t.ct==11)       /*  LINUX  can't do it */
		{
//                        scrbufout();
			return;
		}
		if(t.ct!=15)
			cp_screen();
		s_dpp(x_c,y_c);
		s_dpo(c);
		return;
	}
	if(!t.ct && c>0xaf && c<0xe0)
	{
		if(c==0xb3 || c==0xba) c='|';
		else
			if(c==0xc4 || c==0xcd) c='-';
			else
				if(c>=0xb0 && c<=0xb2) c=' ';
				else    c='+';
	}
	dpo_scr(c);
	if(c>=' ')  x_c++;
END:
	if(y_c>l_y)
	{
		scroll_up(0);
		y_c=l_y;
		scrbufout();
	}
}
void s_dpo(unsigned short i)
{
	char *c;

	if(!i)
		return;
	if(i==es && t.ct==15)
		return;
	if(t.ct==12)
	{
		if(i>=128 && i<=143)
			i+=32;
		else    if(i>=144 && i<=159)
			i+=80;
	}
	if((i>16 && i<32) || i==1 || i==2)
	{
		if(i==Cr && x_p<l_x ) x_p++;
		if(i==cd && y_p<l_y ) y_p++;
		if(i==cl && x_p ) x_p--;
		if(i==cu && y_p ) y_p--;
		if(x11)
			return;
		c = cvtout[i];
		if(c!=NULL)
		{
			int delay=0;
			while(*c>='0' && *c<='9')
				delay=delay*10+(*c++ - '0');
			while(delay)
			{
				sdpo('\0'); 
				delay--; 
			}
			sdps(c);
		}
		else 
			sdpo(i);
		return;
	}
	sdpo(i);
	if(i>=' ')
	{
		x_c=++x_p;
		if(am && x_c>l_x && !x11)
		{
			y_p=++y_c;
			x_p=x_c=0;
		}

	}
}


static int rus(unsigned char i)
{
	if(i>=' '&&i<='~')
		i=tabl[i-' '];
	return(i);
}
static int ukr(unsigned char i)
{
	if(i>=' '&&i<='~')
		i=tabl2[i-' '];
	return(i);
}
char *get_menu_item()
{
	return(menu_str);
}

#ifdef X11R6
int Xdpin()
{
	short c, i;
	char *ch,buf[2];
	static int temp=5;

	ch=buf;
READ:
	if(stdin_std)
	{
		read(0,ch,1);
		if(*ch==0)
		{
			m_finish();
			scrbufout();
			goto READ;
		}
		return(*ch);
	}
	s_dpp(x_c,y_c);
	while (1)
	{
		int st,kod;
		static int x_std,y_std;
BEG:
		XNextEvent(display, &event);

		st=event.xkey.state;
		switch(event.type)
		{
		case ButtonPress:
			ev.b=event.xbutton.button;
			ev.x=event.xkey.x/LL;
			ev.y=event.xkey.y/HH;
			x_std=event.xbutton.x;
			y_std=event.xbutton.y;
			*ch=0;
			temp=5;
			goto NEXT;
		case KeyPress:
		{

			char strbuf[32];
			KeySym  keysym;
			XComposeStatus compose_status = {NULL, 0};

			temp=5;
			ev.b=event.xbutton.button;
			ev.x=event.xkey.x/LL;
			ev.y=event.xkey.y/HH;

			XLookupString(&event.xkey,strbuf,32,&keysym,&compose_status);

			switch (keysym)
			{
				case XK_F1:
#ifdef LINUX
				case 65386:
#endif
					kod=F1;
					break;
				case XK_F2:
#ifdef LINUX
				case 65488:
#endif
					kod=F2;
					break;
				case XK_F3:
#ifdef LINUX
				case 65489:
#endif
					kod=F3;
					break;
				case XK_F4:
#ifdef LINUX
				case 65485:
#endif
					kod=F4;
					break;
				case XK_F5:
#ifdef LINUX
				case 65487:
#endif
					kod=F5;
					break;
				case XK_F6:
					kod=F6;
					break;
				case XK_F7:
					kod=F7;
					break;
				case XK_F8:
					kod=F8;
					break;
				case XK_F9:
					kod=F9;
					break;
				case XK_F10:
					kod=F10;
					break;
				case XK_F11:
					kod=F11;
					break;
				case XK_F12:
					kod=F12;
					break;
				case XK_Home:
					kod=ru;
					break;
				case XK_End:
					kod=rd;
					break;
				case XK_Left:
					kod=cl;
					break;
				case XK_Up:
					kod=cu;
					break;
				case XK_Right:
					kod=Cr;
					break;
				case XK_Down:
					kod=cd;
					break;
				case XK_Page_Up:
					kod=dl;
					break;
				case XK_Page_Down:
					kod=il;
					break;
				case XK_Insert:
					kod=ic;
					break;
				case XK_Delete:
					kod=de;
					break;
				case XK_BackSpace:
					kod=dc;
					break;
				case XK_Tab:
					kod=ta;
					break;
				case XK_Return:
					kod=rn;
					break;
				default:
					if(keysym&0x80)
					{
						switch(keysym&0x7f)
						{
							case XK_i: /* Alt */
							case XK_j:
							case XK_e: /* Caps/Lock */
							case XK_a: /* Shift */
							case XK_b:
							case XK_c: /* Ctrl */
							case XK_d:
								goto BEG;
						}
					}
					keysym&=0x7f;
					if(keysym>=XK_space && keysym<=0x7e)
						kod=x_code[keysym-XK_space];
					else    goto BEG;
			}
#ifdef LINUX
			if(st==4 || st==20)
#else
			if(st==04)
#endif
			{
				if(kod==rn)
					kod=lf;
				else if(kod=='n' || kod=='N')
				{
					rus_lat=!rus_lat;
					ind_lang();
					scrbufout();
					return(dpin());
				}
				else
					kod&=017;
			}
			if(kod<=0)
				break;
			*ch=kod;
			goto NEXT;
		}
		case MotionNotify:
			if(event.xbutton.y-y_std>temp)
			{
				if(temp)
					temp--;
				*ch=cd;
			}
			else    if(event.xbutton.y-y_std<-temp)
			{
				if(temp)
					temp--;
				*ch=cu;
			}
			else    if(event.xbutton.x-x_std>temp)
			{
				if(temp)
					temp--;
				*ch=Cr;
			}
			else    if(event.xbutton.x-x_std<-temp)
			{
				if(temp)
					temp--;
				*ch=cl;
			}
			else
				goto BEG;
			x_std=event.xbutton.x;
			y_std=event.xbutton.y;
			goto NEXT;
		case Expose:
			if(!event.xexpose.count && event.xany.window==window)
			{
				zero_scrold();
				s_dpp(x_c,y_c);
				screen_out();
				s_dpp(x_c,y_c);
			}
			break;
		default:
			break;
		}
	}
NEXT:
	c=*buf;
OK:
	if(rus_lat)
		c=rus((unsigned)c);
	if(fd_macro)
		write(fd_macro,&c,1);
	return((unsigned) c );
}
#endif

int dpin()
{
	int c, i;
	unsigned char *ch;
	unsigned char buf[256];
#ifdef X11R6
	if(x11)
		return(Xdpin());
#endif
	bzero(buf,sizeof buf);
READ:
	bzero(&ev,sizeof ev);
	if(exit_flg)
		return(F10);
	read_byte(buf);
NEXT:
	ch=buf;
	if(*ch==1)
	{
		clear_scr_old();
		screen_out();
	}
	if(*ch==0x8f && t.ct==12)
	{
		read_byte(buf);
		*ch-=0x40;
	}
	if(stdin_std)
	{
		if(*ch==0)
		{
			m_finish();
			scrbufout();
			goto READ;
		}
		return(*ch);
	}
	if(*ch==14)       /* Ctrl/N */
	{
		if(rus_lat!=1) rus_lat=1;
		else    rus_lat=0;
		ind_lang();
		scrbufout();
		return(dpin());
	}
	if(*ch==033)
	{
		int j;

		if(t.ct==15)
		{
			i=0;
			c++;
			read_byte(++ch);
			if(*ch!='%')
			{
				if(*ch!='!')
				{
					goto beg;
				}
				while(*ch!='~' && i<128)
				{
					read_byte(++ch);
					i++;
				}
				*ch=0;
				if((ch=(unsigned char *)strchr((char *)buf+2,';'))==NULL)
				{
					return(0);
				}
				*ch=0;
				ev.x=atoi((char *)buf+2);
				ev.y=atoi((char *)ch+1);
				if((ch=(unsigned char *)strchr((char *)ch+1,';'))==NULL)
				{
					return(0);
				}
				strcpy(menu_str,(char *)ch+1);
				ev.b=100;
				return(0);
			}
			while(*ch!='H' && i<10)
			{
				read_byte(++ch);
				i++;
			}
			if(i==10)
				return(0);
			*ch=0;
			if((ch=(unsigned char *)strchr((char *)buf+2,';'))==NULL)
				return(0);
			*ch=0;
			ev.x=atoi((char *)buf+2)-1;
			ev.y=atoi((char *)ch+1)-1;
			if((ch=(unsigned char *)strchr((char *)ch+1,';'))==NULL)
				return(0);
			;
			ev.b=atoi((char *)ch+1);
			if(ev.x<0)
				ev.x=0;
			if(ev.y<=0)
				ev.y=0;

			return(0);
		}
beg:
		for(i=0,j=0;i<32;i++)
		{
			int ret=0;

			if(cvtin[i]==NULL) continue;
			if(!(ret=difstr(cvtin[i],(char *)buf)))
			{
				if(!i)
					i=0177;
				c=i;
				goto OK;
			}
			if(ret>0) j=1;
		}
		if(j)
		{
			read_byte(++ch);
			goto beg;
		}
		if(fd_macro)
		{
			*ch=0;
			write(fd_macro,ch,1);
			close(fd_macro);
			s_dpo(bf);
			scrbufout();
			fd_macro=0;
			goto READ;
		}
		return(0);       /* unknown */
	}
	c=*buf;
OK:
	if(rus_lat==1) c=rus(c);
	if(rus_lat==2) c=ukr(c);
	if(fd_macro)
		write(fd_macro,&c,1);
	return( c );
}

int difstr(char *a,char *b)
{
	register int i;
	for(i=0;a[i] && b[i];i++)
		if(a[i]!=b[i]) break;
	if(!a[i] && !b[i]) return(0);  /* equval */
	if(!b[i]) return(1);           /* pattern > string */
	return(-1);
}


int dpi()
{
	if(!stdin_std)
		scrbufout();
	return(dpin());
}



/**** cursor position */
void dpp(int x,int y)
{
	if(x>=t.param.xdim) x=t.param.xdim-1;
	if(y>=t.param.ydim) y=t.param.ydim-1;
	x_c=x;
	y_c=y;
}
void flush_dpi()
{
	x_p=-100;
	y_p=-100;
	_p_color=-1;
}

void set_shadows(int *top,int *bott)
{
	if(*top<8)
		*top=*top+8;
	else    *top=017;

}
#ifdef X11R6
void X_g_line(int x,int y,struct clr clr,int arg,int l)
{
	int i,top,bott;

	l=l*LL-1;
	if(x>=t.param.xdim) x=l_x;
	if(y>=t.param.ydim) y=l_y;
	x*=LL;
	y*=HH;
	top=clr.bg&0x3f;
	bott=0;
	set_shadows(&top,&bott);
	if(clr.atr2==2)
	{
		int i=top;
		top=bott;
		bott=i;
	}
	if(top>=NUMCOLORS)
		top=017;
	if(bott>=NUMCOLORS)
		bott=017;
	i=top<bott;
	if(arg==1)
	{
		XDrawLine(display,window,Gcolors[top],x,y,x+l-i,y);
		XDrawLine(display,window,Gcolors[bott],x,y+HH-1,x+l,y+HH-1);
		if(i)
			XDrawLine(display,window,Gcolors[top],x,y+1,x+l-2,y+1);
		else
			XDrawLine(display,window,Gcolors[bott],x,y+HH-2,x+l,y+HH-2);
	}
	else if(arg==2)
		XDrawLine(display,window,Gcolors[top],x,y,x,y+HH-3+2*i);
	else
		XDrawLine(display,window,Gcolors[bott],x+LL-1,y,x+LL-1,y+HH-2);
}

void Xput_box(int x,int y,int l,struct clr clr)
{
	int color;

	if(l<=0)
		return;
	color=clr.bg;
	if(color>=NUMCOLORS)
		color=017;
	XFillRectangle(display,window,Gcolors[color],x*LL,y*HH,l*LL+1,HH);
}

void set_top(struct clr clr,int *fg,int *bg)
{
	*fg=clr.fg&0x3f;
	*bg=clr.bg&0x3f;

*bg=0;
return;
	if(clr.atr1==2)
	{
		if(clr.fg)
		{
			*bg=clr.fg&0x3f;
			*fg=0;
		}
		else
		{
			*fg=0;
			*bg=07;
		}
	}
	else
	{
		if(clr.fg)
			*bg=0;
		else
		{
			*bg=07;
			*fg=0;
		}
	}
}

void Xdpo_ch(unsigned char ch,struct clr clr,int x,int y)
{
	int i,j,p;
	register int sh;
	int X,Y;
	union kode
	{
		long i[4];
		char chr[16];
	} *kode,*shad;
	int koi8tabl[128] =
	{
		0xe1,0xe2,0xf7,0xe7,0xe4,0xe5,0xf6,0xfa,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,
		0xf2,0xf3,0xf4,0xd5,0xc6,0xe8,0xe3,0xfe,0xfb,0xfd,0xff,0xf9,0xf8,0xfc,0xe0,0xf1,
		0xc1,0xc2,0xd7,0xc7,0xc4,0xc5,0xd6,0xda,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,
		0x90,0x91,0x92,0x81,0x87,0xb2,0xb4,0xa7,0xa6,0xb5,0xa1,0xa8,0xae,0xad,0xac,0x83,
		0x84,0x89,0x88,0x86,0x80,0x8a,0xaf,0xb0,0xab,0xa5,0xbb,0xb8,0xb1,0xa0,0xbe,0xb9,
		0xba,0xb6,0xb7,0xaa,0xa9,0xa2,0xa4,0xbd,0xbc,0x85,0x82,0x8D,0x8c,0x8e,0x8f,0x8b,
		0xd2,0xd3,0xd4,0xd5,0xc6,0xc8,0xc3,0xde,0xdb,0xdd,0xdf,0xd9,0xd8,0xdc,0xc0,0xd1,
		0xb3,0xa3,0x93,0x9d,0x97,0x98,0x99,0x9c,0x9b,0x95,0x9e,0x96,0x9d,0x9f,0x94,0x9a
	};

	X=x*LL;
	Y=y*HH;

	if(ch==176)
	{
		XFillRectangle(display,window,Gcolors[clr.fg==8?8:17],X,Y,LL,HH);
		return;
	}
	if(ch==177)
	{
		XFillRectangle(display,window,Gcolors[clr.fg==8?17:18],X,Y,LL,HH);
		return;
	}
	if(ch==178)
	{
		XFillRectangle(display,window,Gcolors[clr.fg==8?18:19],X,Y,LL,HH);
		return;
	}

	kode=(union kode *)&font8x16[ch*16];
	sh=0;
	shad=(union kode *)&font8x16[0];

	if(ch>=179 && ch<=218)
		sh=2;
	if(ch>=182 && ch<=216)
	{
		for(i=0;i<16;i++)
		{
			register int k=kode->chr[i];
			if(k)
			{
				shad=(union kode *)&shadows[(ch-182)*16];
				sh=1;
				break;
			}
		}

	}

	if(sh==0 && koi8 && ch>=128)
	{
		ch=koi8tabl[(ch-128)&0xff];
	}

	else if(sh || (ch>127 && ch<256))
	{
		for(p=0;p<HH;p++)
		{
			int k1,s1=0,mask=1;
			register int k;
			int s;

			if(y<l_y/2) i=HH-1-p;
			else    i=p;
			k1=i-1;
			if(k1<0)
				k1=0;
			if(k1>15)
				k1=15;
			s=shad->chr[k1];
			k=kode->chr[k1];
			mask<<=7;
			for(j=0;j<LL;j++)
			{
				if((k1=k&mask) || (sh==1 && (s&mask)))
				{
					if(sh==1)
					{
						s1=s&mask;
						if(k1 && s1)
						{
							if(clr.fg<=017)
							{
								if(clr.fg<010)
									k1=0;
								else
									k1=017;
							}
							else    k1=07;
						}
						else
						{
							if(clr.fg<=017)
								k1=clr.fg&07;
							else    k1=010;
						}
						if(k1>=NUMCOLORS)
							k1=017;
						if(!j)
							XDrawPoint(display,window,Gcolors[k1],X,Y+i);
						XDrawPoint(display,window,Gcolors[k1],X+j+1,Y+i);
					}
					else
					{
						if(!j)
							XDrawPoint(display,window,Gcolors[clr.fg&0x3f],X,Y+i);
						XDrawPoint(display,window,Gcolors[clr.fg&0x3f],X+j+1,Y+i);
					}
				}
				if(mask>1)
					mask>>=1;
			}
		}
		return;
	}
	if(clr.bg && clr.fg!=0)
	{
		int fg,bg;

		set_top(clr,&fg,&bg);
		XDrawString(display,window,Gcolors[bg&0x3f],X+SHIFTX+1,Y+SHIFTY+1,(char *)&ch,1);
		XDrawString(display,window,Gcolors[bg&0x3f],X+SHIFTX+1,Y+SHIFTY+0,(char *)&ch,1);
		XDrawString(display,window,Gcolors[fg&0x3f],X+SHIFTX+0,Y+SHIFTY-1,(char *)&ch,1);
	}
	else
		XDrawString(display,window,Gcolors[clr.fg&0x3f],X+SHIFTX,Y+SHIFTY,(char *)&ch,1);
}

void Xdps(unsigned char *str,struct clr clr,int x,int y,int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		Xdpo_ch(str[i],clr,x+i,y);
	}
}

void res_Cur()
{
	if(curs_x!=-1)
	{
		if(!_no_cursor && curs_x>=0)
		{
			struct clr clr=get_clrn(curs_x,curs_y);
			Xput_box(curs_x,curs_y,1,clr);
			Xdpo_ch(get_ch(curs_x,curs_y),clr,curs_x,curs_y);
		}
	}
	curs_x=-1;
}

void Xs_dpp(int x, int y)
{
	if(x>=t.param.xdim) x=l_x;
	if(y>=t.param.ydim) y=l_y;
	if(x<0) x=0;
	if(y<0) y=0;
	res_Cur();
	curs_x=x_c=x_p=x;
	curs_y=y_c=y_p=y;
	if(!_no_cursor)
		XDrawRectangle(display,window,Gcolors[get_bg(x,y)<=8?15:0],x*LL,y*HH,LL-1,HH-1);
	return;
}
#endif

void s_dpp(int x,int y )
{
	register char *c;

#ifdef X11R6
	if(x11)
	{
		Xs_dpp(x,y);
		return;
	}
#endif
	if(x>=t.param.xdim) x=t.param.xdim-1;
	if(y>=t.param.ydim) y=t.param.ydim-1;

	if(y==y_p)
	{
		int l;

		l=x-x_p;
		if(l>0 && l<6)
		{
			int i;
			for(i=0;i<l;i++)
			{
				if(get_clr(x_p,y)==S_color)
					s_dpo(get_ch(x_p,y));
				else break;
			}
		}
		if(x==x_p-1)
			s_dpo(cl);
		else
#ifndef LINUX
			if(x==x_p+1)
#else
				if(x==x_p+1 && x!=1)
#endif
				{
					if(get_clr(x_p,y)==S_color)
						s_dpo(get_ch(x_p,y));
					else
						s_dpo(Cr);
				}
	}

	if(y==y_p+1)
		s_dpo(cd); 
	else
		if(y==y_p-1)
			s_dpo(cu);

	if(x==x_p && y==y_p)
		goto END;

	if(shft==0)
		c = tgoto(cvtout[16], x, y);
	else
	{
		static char str[64];
		sprintf(str,cvtout[16], y+32, x+32);
		c=str;
	}
	sdps(c);
END:
	x_c=x_p=x;
	y_c=y_p=y;
	return;
}


void s_dps(const char *str)
{
	sdps(str);
}

void sdps(const char *str)
{
	register const char *s;
	if(str==NULL)
		return;
	for(s=str;*s;s++)
		sdpo(*s);
}

struct clr color_to_clr(int i)
{
	struct clr clr;

	clr.fg = i&0xf;
	clr.bg =(i>>4)&0xf;
	clr.atr2=(i>>8)&03;
	clr.atr1=(i>>10)&03;
	if(i&0xf000)
	{
		clr.fg+=((i>>12)&03)*16;
		clr.bg+=((i>>14)&03)*16;
		if(Nf && clr.fg>=Nf)
			clr.fg=017;
		if(Nb && clr.bg>=Nb)
			clr.fg=017;
	}
	return(clr);
}
int clr_to_col(struct clr clr)
{
	int i;

	i=(clr.fg&0xf)+(clr.bg&0xf)*16;
	if(clr.atr2)
		i+=((clr.atr2&03)<<8);
	if(clr.atr1)
		i+=((clr.atr1&03)<<10);

	if(clr.fg>=16)
		i+=(((clr.fg/16)&03)<<12);
	if(clr.bg>=16)
		i+=(((clr.bg/16)&03)<<14);
	return(i&0xffff);
}


char *getenv();
static int fol_col=0;

/**** The old version of setcolor */
unsigned setcolor(int i)
{
	struct clr clr;
	int j;

	if(i<0)
		return(S_color);
	clr=color_to_clr(i);
	j=clr.fg+(clr.atr2<<8);
	i=clr.bg+(clr.atr1<<8);
	return(Set_Color(i,j));
}

unsigned  Set_Color(int fon,int let)
{
	struct clr clr;
	int i,atr1,atr2;

	atr1=(let>>8)&03;
	atr2=(fon>>8)&03;
	let&=0x3f;
	fon&=0x3f;
	if(atr1==3)
		return(S_color);
	if(Nf)
	{
		let%=Nf;
		if(let<16)
		{
			if(Mf[let]>='A')
				let=Mf[let]-'A'+10;
			else let=Mf[let]-'0';
		}
	}
	if(Nb)
	{
		fon%=Nb;
		if(fon<16)
		{
			if(Mb[fon]>='A')
				fon=Mb[fon]-'A'+10;
			else fon=Mb[fon]-'0';
		}
	}

	if(t.ct==5)
	{
		if(fon && let) let=0;
	}
	if(t.ct==3 || t.ct==4)
	{
		i=let+(fon<<4);
		if(i==010) i=0;
		if(i==017) i=0101;
		else
			if(i==012) i=0100;
			else
				if(i>=010 && i<020) i=020;
				else
					if(i>=020 && i<0200) i=0160;
					else
						if(i>0200) i=0161;
						else i=0;
		fon=(i>>4)&017;
		let=i&017;
	}
	if(t.ct==6 || t.ct<=0)
	{
		if(fon && fon!=010)
			fon=1;
		else    fon=0;
		let=1;
	}

	clr.fg=let;
	clr.bg=fon;
	clr.atr1=atr1;
	clr.atr2=atr2;

	S_color=clr_to_col(clr);

	return(S_color);
}

static int old_fon=0,old_let=0,old_atr=0;
static int __bord=0;
unsigned s_setcolor(int i)
{
	unsigned char let,fon;
	struct clr clr;
	char str[16];

	if(x11)
		return(_p_color);
	if(_p_color==i)
		return(S_color=_p_color);
	S_color=i;
	clr=color_to_clr(i);

	_p_color=S_color;

	fon=clr.bg&0x3f;
	let=clr.fg&0x3f;

	if(t.ct==15)    /* Telnet Applet */
	{
		if(clr.atr2!=old_atr)
		{
			if(old_atr && __bord && *cvtout[5])
			{
				sdps(cvtout[5]);
				__bord=0;
			}
			if(clr.atr2==1 && *cvtout[3])
			{
				sdps(cvtout[3]);
				__bord=1;
			}
			else if(clr.atr2==2 && *cvtout[4])
			{
				sdps(cvtout[4]);
				__bord=1;
			}
			old_atr=clr.atr2;
		}
		if(cvtout[11]==NULL || cvtout[12]==NULL)
		{
			let+=20;
			fon+=52;
		}
		goto END;
	}
	if(t.ct==1) let=i;
	if(t.ct==6 || t.ct<=0)
	{
		if(fon!=old_fon)
		{
			if(fon) sdps(cvtout[se]);
			else    sdps(cvtout[so]);
			old_fon=fon;
		}
		return(i);
	}
	if(t.ct==1 && let==0)
		let=1;
	if(t.ct==10)    /* for AIX console only */
	{
		if(let>=010)
			let+=82;
		else if(let<010)
			let+=30;
		if(fon>=010)
			fon+=92;
		else if(fon<010)
			fon+=40;
	}
END:
	if(cvtout[11]!=NULL && cvtout[12]!=NULL)
	{
		if(fon!=old_fon)
		{
			sprintf(str,cvtout[12],fon+cshft);
			sdps(str);
		}
		if(let!=old_let)
		{
			if(t.ct==11)
			{
				if(old_let>7 && let<=7)
				{
					sprintf(str,"%c[21m",27);
					sdps(str);
				}
				if(old_let<=7 && let>7)
				{
					sprintf(str,"%c[1m",27);
					sdps(str);
				}
			}
			sprintf(str,cvtout[11],t.ct==11?(let&07):let+cshft);
			sdps(str);
		}
	}
	else if(cvtout[0]!=NULL)
	{
		if(fon!=old_fon)
		{
			sprintf(str,cvtout[0],fon);
			sdps(str);
		}
		if(let!=old_let)
		{
			sprintf(str,cvtout[0],let);
			sdps(str);
		}
	}
	else if(cvtout[29]!=NULL)
	{
		sprintf(str,cvtout[29],let,fon);
		sdps(str);
	}
	else
	{
		if(fon) sdps(cvtout[se]);
		else    sdps(cvtout[so]);
	}
	old_fon=fon;
	old_let=let;
	return(i);
}
void sdpo(char c )
{
	buf[xbuf] = c;
	buf[++xbuf]=0;
	if(xbuf >= lbuf )
	{
		_scrbufout();
	}
}


void scrbufout()
{
	if(!stdin_std)
	{
		screen_out();
		_scrbufout();
	}
}
void _scrbufout()
{
	write(scr_out,buf,xbuf);
	xbuf = 0;
}


void ind_lang()
{
	int x,y;
	x=x_c; 
	y=y_c;
	s_dpp(t.param.xdim-1,0);
	if(rus_lat==1) sdpo('R');
	else    if(rus_lat==2) sdpo('U');
	else    sdpo('L');
	s_dpp(x,y);
}

/****
   Ž¯¥à æ¨¨ á ¬ ªà®ä ©« ¬¨ ¤«ï ConteXt' . Š screen ¯àï¬®£® ®â­®è¥­¨ï
	   ­¥ ¨¬¥îâ, ­® ¢ ¤àã£®¥ ¬¥áâ® ¨å ¢áâ ¢¨âì ­¥«ì§ï.
	*/
int dup22(int a,int b)
{
	close(b);
	fcntl(a,F_DUPFD,b);
	return(0);
}
void macro(char *name)
{
	fd_std=open(name,O_RDONLY);
	stdin_std=dup(0);
	dup22(fd_std,0);
}
int if_macro()
{
	return(fd_macro);
}
int creat_macro(char *name)
{
	if((fd_macro=creat(name,0644))<0)
		return(fd_macro=0);
	s_dpo(bb);
	scrbufout();
	return(fd_macro);
}
static void m_finish()
{
	close(fd_std);
	dup22(stdin_std,0);
	close(stdin_std);
	stdin_std=0;
	zero_scrold();
	screen_out();
}
int macro_act()
{
	return(stdin_std);
}
unsigned get_s_color()
{
	return(S_color);
}
