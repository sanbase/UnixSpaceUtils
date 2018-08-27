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

int in_fd=0;

void read_byte(unsigned char *buf)
{
	struct timeval timeout;
	fd_set read_set;
	int i;
BEGIN:
	if(in_fd==0)
	{
		FD_ZERO(&read_set);
		FD_SET(0, &read_set);
		bzero(&timeout,sizeof timeout);
		timeout.tv_sec=60;
		if((i=select(1,&read_set,NULL,NULL,&timeout))==0)
		{
			if(getppid()==1)
			{
				kill(getpid(),SIGHUP);
				dpend();
				exit(0);
			}
			goto BEGIN;
		}
		if(i<0)
		{
			dpend();
			exit(0);
		}
	}
	read(in_fd,buf,1);
}
