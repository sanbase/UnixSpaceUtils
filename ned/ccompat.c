/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: ccompat.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 *  Цветовая поддержка
*/
#include <stdio.h>
#include <string.h>
#include "ned.h"

typedef struct {
	char *w;
	int c;
} CCM;

static char spec[]="(){}; []<>+-=,.!&:?|%^~`";

#define isspec(a)  (strchr(spec,a)!=NULL || !a)
#define iscchar(a) ((a   &&  S->s[a-1]=='\'' &&  S->s[a+1]=='\'') || \
		    (a>1 &&  S->s[a-2]=='\'' &&  S->s[a-1]=='\\' &&  \
		    S->s[a+1]=='\''))

static int FindReserved(char *);
static int WordFunction(char *);

/* Должна быть сортирована по словам */
CCM cpm[]={
	{ "#define", 28, },
	{ "#else",   28, },
	{ "#endif",  28, },
	{ "#if",     28, },
	{ "#ifdef",  28, },
	{ "#ifndef", 28, },
	{ "#include",28, },
	{ "#undef",  28, },
	{ "auto",    30, },
	{ "break",   27, },
	{ "case" ,   27, },
	{ "char",    29, },
	{ "class" ,  27, },
	{ "const",   27, },
	{ "continue",27, },
	{ "default", 27, },
	{ "defined", 28, },
	{ "delete",   27, },
	{ "do",      27, },
	{ "double",  29, },
	{ "else",    27, },
	{ "enum",    29, },
	{ "esac",    27, },
	{ "extern",  30, },
	{ "fi",      27, },
	{ "float",   29, },
	{ "for",     27, },
	{ "goto",    27, },
	{ "if",      27, },
	{ "implements",27, },
	{ "int",     29, },
	{ "interface",27, },
	{ "long",    29, },
	{ "new",    27, },
	{ "operator",30 },
	{ "private",  30, },
	{ "protected",  30, },
	{ "public",  30, },
	{ "register",30, },
	{ "return",  27, },
	{ "short",   29, },
	{ "static",  30, },
	{ "struct",  29, },
	{ "switch",  27, },
	{ "then",    27, },
	{ "typedef", 29, },
	{ "union",   29, },
	{ "unsigned",29, },
	{ "virtual", 30, },
	{ "void",    29, },
	{ "while",   27, },
	{ "",        0,  },
};

static void  colour_symbol(NEF *W,int p,int *i,int *j,char s,int c)
{
	 if(*i>=GP(W))
		 if(nis_print((unsigned char)s) && *j<=MAXX(W)) {
			 setcolor(c);
			 dpp(W->EB.x+(++(*j)),W->EB.y+p+1);
			 dpo(s);
		 } else
			 (*j)++;
	 (*i)++;
}

void ColourWords(NEF *W,STR *S,int p,int arg)
{
	int i,j,c,col;

	col=setcolor(-1); stor_poz();
	if(cmn.UseColour && W->colour && S->rlen>GP(W)) {
		for(i=GP(W);i>0 && !isspec(S->s[i]);i--);
		for(j=0;S->s[i] && j<=MAXX(W);) {
			for(;S->s[i] && isspec(S->s[i]) && j<=MAXX(W);) {
				if(S->s[i]=='{' || S->s[i]=='}') {
					if(!iscchar(i)) {
						colour_symbol(W,p,&i,&j,S->s[i],COLFIG);
						continue;
					}
				}
				if(S->s[i]=='(' || S->s[i]==')') {
					if(!iscchar(i)) {
						colour_symbol(W,p,&i,&j,S->s[i],COLROU);
						continue;
					}
				}
				if(!arg) {
					if(i++>=GP(W)) j++;
				}
				else
					colour_symbol(W,p,&i,&j,S->s[i],W->EB.colortext);
			}
			if((c=FindReserved(S->s+i))>0){
				for(;j<=MAXX(W) && !isspec(S->s[i]);)
					colour_symbol(W,p,&i,&j,S->s[i],c);
				continue;
			}
			if((c=WordFunction(S->s+i))>0) {
				int t;

				for(t=0;j<=MAXX(W) && t<c && S->s[i];t++)
					colour_symbol(W,p,&i,&j,S->s[i],COLFUN);
				continue;
			}
			for(;!isspec(S->s[i]) && S->s[i];) {
				c=(unsigned char)S->s[i];
				if((c>=128 && c<=154) || (c>=156 && c<=175) ||
				   (c>=224 && c<=249)) {
					colour_symbol(W,p,&i,&j,S->s[i],COLRUS);
					continue;
				}
				if((S->s[i]=='/' && (S->s[i+1]=='/' || S->s[i+1]=='*')) ||
				   (S->s[i]=='*' && S->s[i+1]=='/')) {
					colour_symbol(W,p,&i,&j,S->s[i],COLCOM);
					colour_symbol(W,p,&i,&j,S->s[i],COLCOM);
					continue;
				}
				if(S->s[i]=='"') {
					colour_symbol(W,p,&i,&j,S->s[i],COLQUO);
					continue;
				}
				if(!arg) {
					if(i++>=GP(W)) j++;
				} else
					colour_symbol(W,p,&i,&j,S->s[i],W->EB.colortext);
			}
		}
	}
	if(RP(W)>GP(W) && RP(W)<GP(W)+MAXX(W))
		if(get_ch(W->EB.x+RP(W)-GP(W),W->EB.y+p+1)==' ') {
			setcolor(W->EB.colorborder);
			Dpp(&W->EB,RP(W)-GP(W)-1,p); dpo('|');
		}
	if(W->PozAF && W->PozAF>GP(W) && W->PozAF<GP(W)+MAXX(W))
		if(get_ch(W->EB.x+W->PozAF-GP(W)+2,W->EB.y+p+1)==' ') {
			setcolor(W->EB.colorborder);
			Dpp(&W->EB,W->PozAF-GP(W)+1,p); dpo('|');
		}
	rest_poz(); setcolor(col);
}

static int FindReserved(char *s)
{
	int i,j;

	for(i=0;cpm[i].c;i++) {
		for(j=0;cpm[i].w[j] && cpm[i].w[j]==s[j];j++);
		if(!cpm[i].w[j] && isspec(s[j])) return(C[cpm[i].c]);
		if(cpm[i].w[j]>s[j]) break;
	}
	return(-1);
}

static int WordFunction(char *s)
{
	int i,j;

	for(i=0;s[i] && s[i]!='(' && (isalnum(s[i]) || s[i]==':' ||
		s[i]=='_'|| (!i && s[i]=='*'));i++)
			if(s[i]==':')
				if(s[i+1]==':')
			                i++;
			        else
				        break;
	for(j=i;s[j]==' ';j++);
	if((i && s[i-1]=='*') || s[j]!='(') i=0;
	return(i);
}

void ManWords(NEF *W,int k,int p)
{
	STR *S;
	int i,j;

	if(k>=NMAX(W) || (S=GetSTR(W->AS,k))==NULL || S->rlen<=GP(W))
		return;
	for(i=j=0;S->s[i] && j<=MAXX(W);i++) {
		if(S->s[i]==SPECMANCODE1) {
			if(i>=GP(W)) {
				if(j>0)
					put_clr(W->EB.x+j,W->EB.y+p+1,COLNOPRNT);
			} else
				j--;
			j--;
			continue;
		}
		if(i>=GP(W)) j++;

	}
}


/* Определяет, нуждается ли файл в подсветке */
int IfNeedColour(char *name)
{
	static char *suffix[]= {
		".c",
		".cc",
		".cpp",
		".java",
		".h",
		".c.b",
		".cc.b",
		".java.b",
		".h.b",
		"",
	};
	char **ch=suffix;

	for(;**ch;ch++) {
		char *temp=strstr(name,*ch);

		if(temp!=NULL && !strcmp(temp,*ch)) return(1);
	}
	return(0);
}
