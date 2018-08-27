#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include "tty_codes.h"
main()
{
	char ch;
	dpbeg("MORE");
	dpinit();
	for(;;)
	{
		int i=dpi();
//                read(0,&ch,1);
		printf("%d ",i);
		fflush(stdout);
	}
}
