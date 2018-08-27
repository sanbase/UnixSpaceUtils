/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: steck.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <tty_codes.h>
#include "ned.h"

typedef struct steck {
	char *buf;  /* буфер со строками                                   */
	int  nbuf;  /* занято места в стеке                                */
	int  nmem;  /* зарезервировано места в памяти под стек             */
} STK;

static STK *stk;           /* массив стеков                                       */
static int NumStk=0;       /* количество стеков                                   */

/* Инициализация нового стека */
int init_stk(int mlen)
{
	int err=0;

	if(!NumStk) {
		if((stk=(STK *)Malloc(sizeof(STK)))==NULL) {
			err=-1; goto END;
		}
	}
	else {
		STK *tmp;

		if((tmp=(STK *)Realloc(stk,(NumStk+1)*sizeof(STK)))==NULL) {
			err=-1; goto END;
		}
		stk=tmp;
	}
	if((stk[NumStk].buf=Malloc(mlen))==NULL) {
		err=-1; goto END;
	}
	stk[NumStk].nbuf=0; stk[NumStk].nmem=mlen;
	NumStk++;
END:
	return(err?err:NumStk-1);
}

int put_l_s(int d,char *str)
{
	int len,err=0;

	if(!*str) goto END;
	if(d<0) {
		err=-1; goto END;
	}
	len=strlen(str);
	if(NumBegSpace(str)==len) goto END;
	if(stk[d].nbuf) {
		char *ch;
		int k;

		for(k=0;ch=get_l_s(d,k);k++) {
			if(!strcmp(ch,str)) {
				if(ch!=stk[d].buf) {
				        memcpy(ch,ch+len+1,stk[d].nbuf-len-(int)(ch-stk[d].buf));
					stk[d].nbuf-=(len+1);
					break;
				}
				else
				        goto END;
			}
		}
	}
	if(stk[d].nbuf+len+1<=stk[d].nmem) {
	        memmove(stk[d].buf+len+1,stk[d].buf,stk[d].nbuf);
		stk[d].nbuf+=len+1;
	}
	else {
		memmove(stk[d].buf+len+1,stk[d].buf,stk[d].nmem-len-1);
		for(stk[d].nbuf=stk[d].nmem-1;stk[d].buf[stk[d].nbuf];stk[d].nbuf--)
			stk[d].buf[stk[d].nbuf]=0;
	}
	memcpy(stk[d].buf,str,len+1);
END:
	return(err);
}

char *get_l_s(int d,int n)
{
	int i,l;

	if(d<0) return(NULL);
	for(l=i=0;i<stk[d].nbuf;i++) {
		if(l==n) {
			int j;

			if(!stk[d].buf[i])
				break;  /* не может быть строк нулевой длины */
			for(j=i;stk[d].buf[j] && j<stk[d].nmem;j++);
			if(j==stk[d].nmem)
				stk[d].buf[stk[d].nmem-1]=0;
			return(stk[d].buf+i);
		}
		if(!stk[d].buf[i]) l++;
	}
	return(NULL);
}

static int curr_d,maxlen;

int get_one_steck(int x,int y,int j,int c)
{
	char *ch;

	dpp(x,y);
	setcolor(c);
	if((ch=get_l_s(curr_d,j))!=NULL) {
		int i;

		for(i=0;ch[i] && i<maxlen;i++) dpo(ch[i]);
		for(;i<maxlen;i++) dpo(' ');
	}
}

int menu_steck(int d,int x,int y,int show,char *str)
{
	int i,j,c=0,mes=0,col;
	char *ch;
	int nb=-1,size;

	col=setcolor(-1);
	if(d<0) return(C_STOP);
	curr_d=d;
	for(size=0,maxlen=0;ch=get_l_s(curr_d,size);size++)
		maxlen=max(maxlen,strlen(ch));
	if(!size || !maxlen) goto END;
	if(maxlen>l_x-x-3) maxlen=l_x-x-3;
	if(maxlen<10) maxlen=10;
	if(show>size) show=size;
	nb=get_box(x,y,maxlen+4,show+3);
	i=j=0;
	mes=line_status(146);
	TmpUnsetProt();
	while(1) {
		switch(c=amenu(x,y,maxlen,show,size,get_one_steck,
		       COLSBGR,COLSSTR,&i,&j,0))
		{
		case C_STOP:
			goto END;
		case C_RETURN:
			if((ch=get_l_s(curr_d,j))==NULL)
				*str=0;
			else
				strcpy(str,ch);
			goto END;
		default:
			dpo(bl); continue;
		}
	}
END:
	TmpRestProt();
	if(nb>=0) {
		put_box(x,y,nb); free_box(nb);
	}
	if(mes) line_status(mes);
	setcolor(col);
	return(!c?C_STOP:c);
}

void SaveStecksConf(int fd)
{
	int i;
	int *buf;

	for(i=0,buf=(int *)(&cmn.sarg);i<4;i+=1)
		if(buf[i]>=0) {
		        write(fd,&stk[buf[i]].nbuf,4);
		        write(fd,stk[buf[i]].buf,stk[buf[i]].nbuf);
	        }
		else {
			int k=0;

	                write(fd,&k,sizeof(int));
	        }
}

void ReadStecksConf(int fd)
{
	int i;
	int *buf;

	for(i=0,buf=(int *)(&cmn.sarg);i<4;i++)
	{
		int k;

		if(read(fd,&k,sizeof(int))<=0) break;
		if(k>0) {
			buf[i]=init_stk(max(min(k,BUFSIZ),LINESIZE));
			if(read(fd,stk[buf[i]].buf,k)<=0) break;
			stk[buf[i]].nbuf=k;
		}
	}
}

