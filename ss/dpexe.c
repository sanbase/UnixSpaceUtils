main(int argc,char **argv)
{
	int i;
	char *cmd=(char *)malloc(1);
	*cmd=0;

	for(i=1;i<argc;i++)
	{
		cmd=(char *)realloc(cmd,strlen(cmd)+strlen(argv[i])+5);
		strcat(cmd,argv[i]);
		strcat(cmd," ");
	}
	system(cmd);
	printf("\nOK\n");
	getchar();
	exit(0);
}
