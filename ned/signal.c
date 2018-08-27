/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: signal.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <tty_codes.h>
#include "ned.h"

#define NUMSIG 8

void errore(),sigINT(),sigOTHER(),sigUSR1();

/* Обрабатываемые nedом сигналы и реакция на них */
typedef struct sig_action {
	int sig;         /* сигнал                                          */
	void (*func)();  /* реакция на сигнал                               */
} SA; /* Signal Action */

static SA sa[NUMSIG]=
{
	{ SIGINT,  sigINT   },
	{ SIGTERM, sigOTHER },
	{ SIGQUIT, sigOTHER },
	{ SIGUSR1, sigUSR1  },
	{ SIGUSR2, sigOTHER },
	{ SIGFPE,  errore   },
	{ SIGSEGV, errore   },
	{ SIGBUS,  errore   },
};

void errore(int sig)
{
	emess(pmes(30));
	signal(sig,SIG_IGN);
	ClrScr();
	clean_temporary();
	dpend();
	exit(1);
}

void sigINT(int sig)
{
	int i=0;

	signal(SIGINT,SIG_IGN);
	TmpUnsetProt();
	switch(y_n_m(8,pmes(104),"",&i,pmes(101),pmes(102),NULL)) {
	case C_STOP:
		i=0; break;
	case C_RETURN:
		switch(i) {
		case 0:
			i=1; break;
		case 1:
			i=0; break;
		}
	}
	TmpRestProt();
	if(!i) {
		signal(SIGINT,sigINT); scrbufout(); return;
	}
	ClrScr();
	clean_temporary();
	dpend();
	exit(2);
}

void sigUSR1()
{
	WriteAll(0);
	clean_temporary();
	SaveS();
	ClrScr();
	dpend();
	exit(2);
}

void sigOTHER(int sig)
{
	char str[SMALLSTR];

	ClrScr();
	clean_temporary();
	dpend();
	sprintf(str,pmes(112),sig);
	printf(str);
	exit(2);
}

void make_sig_action()
{
	int i;

	for(i=0;i<NUMSIG;i++) signal(sa[i].sig,sa[i].func);
}

void def_sig_action()
{
	int i;

	for(i=0;i<NUMSIG;i++) signal(sa[i].sig,SIG_DFL);
}
