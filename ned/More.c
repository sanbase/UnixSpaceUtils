/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev
	Copyright (c) 1996 SOFTEX PLUS, inc. Toronto ON, Canada.
	All rights reserved.
	Writen by Alexander Lashenko.
	Report problems to san@interlog.com, unitex@public.ua.net
*/
#include <fcntl.h>
#include <sys/stat.h>
#include "tty_codes.h"
#include <stdio.h>
extern int l_y,l_x;
extern struct mouse ev;
main(argc,argv)
int argc;
char **argv;
{
	int fd,i,j,num_strings=0,pos=0,found=0;
	int x=0,y=0;
	char *buf,**strings=0,obr[256];
	struct stat st;
	char *help="Page/Up -page Page/Do +page Up -line Down +line F10 exit ";

	if(argc<2)
	{
		buf=(char *)malloc(1024);

		fd=1024; i=0;
		for(st.st_size=0;;st.st_size++)
		{
			int j=getchar();

			if(j==EOF || j==0)
				break;
			buf[st.st_size]=j;
			if(i>=1023)
			{
				buf=(char *)realloc(buf,(fd+=1024));
				i=0;
			}
			i++;
		}
	}
	else
	{
	        if(stat(argv[1],&st))
		        exit(-1);
	        buf=(char *)malloc(st.st_size+1);
		buf[st.st_size]=0;
	        fd=open(argv[1],O_RDONLY);
	        read(fd,buf,st.st_size);
	        close(fd);
	}
	strings=(char **)malloc(sizeof (char *));
	strings[num_strings++]=buf;
	for(i=0;i<st.st_size;i++)
	{
		if(buf[i]=='\n' && i<st.st_size)
		{
			num_strings++;
	                strings=(char **)realloc(strings,num_strings*sizeof (char *));
		        buf[i]=0;
		        strings[num_strings-1]=buf+i+1;
		}
	}
	dpbeg("MORE");
	dpinit();
	fd=0;
BEG:
	setcolor(0140+012);
/*        dpo(es); */


	for(i=0;i<l_y;i++)
	{
		dpp(0,i);
	        for(j=0;j<l_x;j++)
			dpo(' ');
	}
	dpp(0,0);

	if(fd+l_y>num_strings)
		fd=num_strings-l_y;
	if(fd<0) fd=0;
	for(i=0;i<l_y && i<num_strings;i++)
	{
		dps(strings[fd+i]+pos);
		dpp(0,i+1);
	}
	help_line(help);
	dpp(x,y);
	x=y=0;
	switch(Xmouse(dpi()))
	{
		case cu:
			fd--;
			goto BEG;
		case 0:
			if(ev.b==3)
				goto END;
		case cr:
			pos++;
			goto BEG;
		case cl:
			if(pos)
				pos--;
			goto BEG;
		case cd:
		case ' ':
			fd++;
			goto BEG;
		case il:
			fd+=l_y-1;
			goto BEG;
		case dl:
			fd-=l_y-1;
			goto BEG;
		case rd:
			fd=num_strings;
			goto BEG;
		case ru:
			fd=0;
			goto BEG;
		case F3:
		{
			char *ch;
			int i;

			if(!*obr)
				goto BEG;
			for(i=found+1;i<num_strings;i++)
			{
				if((ch=strstr(strings[i],obr))!=NULL)
					break;
			}
			if(ch!=NULL)
			{
				y=found=i;
				if(y>=l_y)
				{
					fd=y-l_y/2;
					y=l_y/2;
				}
				x=ch-strings[i];
				x=get_pos(strings[i],x);
				if(x>=l_x)
				{
					pos=x-l_x/2;
					x=l_x/2;
				}
			}
			goto BEG;
		}
		case '/':
		{
			char *ch;
			int i;

			bzero(obr,254);
			edit(0,obr,255,l_x-1,0,l_y-1,0);
			for(i=0;i<num_strings;i++)
			{
				if((ch=strstr(strings[i],obr))!=NULL)
					break;
			}
			if(ch!=NULL)
			{
				y=found=i;
				if(y>=l_y)
				{
					fd=y-l_y/2;
					y=l_y/2;
				}
				x=ch-strings[i];
				x=get_pos(strings[i],x);
				if(x>=l_x)
				{
					pos=x-l_x/2;
					x=l_x/2;
				}
			}
			goto BEG;
		}
		case 'q':
		case 'Q':
		case lf:
		case F10:
			goto END;
		default:
			goto BEG;
	}
END:
	dpend();
	exit(0);
}

static char *Keys[]=
{
       "Esc",  "",    "",           "F12",     "F3",  "",      "",     "",
       "Del",  "Tab", "Ctrl/Enter", "F1",      "F2",  "Enter", "F4",   "F5",
       "F6",   "F7",  "End",        "Home",    "Down","Up",    "Right","Left",
       "Ins",  "",    "Page/Up",    "Page/Do", "F8",  "F9",    "F10",  "F11"
};
/* функция центровки сообщения */
char *centr(mess)
char *mess;
{
	int size,num;
	register char *ch;
	register int i;
	static char str[256],str1[256];

	strcpy(str,mess);
	if(!(size=strlen(str)))
		return(str);
	num=l_x-size+1;      /* столько пробелов добавить */
	while(num>0)
	{
		size=strlen(str)+1;
		for(i=0,ch=str1;i<size;ch++,i++)
		{
			if(str[i]==' ' && num)
			{
				*ch=' '; ch++;
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
help_line(str)
char  *str;
{
	register int i,j,k;
	char word[80],str1[81];

	strcpy(str1,str);
	str=centr(str1);
	dpp(0,l_y);
	setcolor(0x53a);
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
				dpp(i-strlen(word),l_y);
				setcolor(07);
				dps(word);
				break;
			}
	}
}
get_pos(char *str,int pos)
{
	int x=0,i;

	for(i=0;i<pos;i++)
	{
		if(str[i]=='\t')
		        x=x+8-x%8;
		else    x++;
	}
	return(x);
}
