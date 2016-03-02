#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <signal.h>


void  ALARMhandler(int sig)
{

  printf("Hello");
  signal(SIGALRM, ALARMhandler);     /* reinstall the handler    */
}


int main(){
	
	int nr=0;
	signal(SIGALRM, ALARMhandler);
	
	while(1){
	
		printf("%d\n", nr);
		nr ++;
		alarm(rand() % 10);
		sleep(1);
	}
	
	exit(0);
	
}
