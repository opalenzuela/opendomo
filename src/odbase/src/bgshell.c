
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		printf("%s <shell script>\n", argv[0]);	
		return 0;
	}


	setsid();
	chdir("/");
	close(0);
	close(1);
	close(2);


	if(fork()==0)
	{
		system(argv[1]);
		exit(0);
	}

	
	return 0;
}



