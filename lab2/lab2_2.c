
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include  <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

int main(){
	
	int i=0;
	int status;
	
	while(i<10){
	
		int pid2 = fork();
		int randnr=rand()%10;
		
		i++;
		if(pid2==0){
			//RESTANTES FILHOS

			sleep(randnr);

			printf("AFTER  SLEEP PID - %d SLEEP TIME - %d\n", getpid(), randnr);
			exit(randnr);
		}
	//PAI
	
	}
	int j, pid_child, total_time=0;
	for(j=0; j<10; j++){
		pid_child = wait(&status);
		printf("PARENT PID_CHILD - %d TIME_CHILD - %d\n", pid_child,  WEXITSTATUS(status));
		
		total_time = total_time + WEXITSTATUS(status);
	}
	
	printf("TOTAL SLEEP TIME - %d\n", total_time);
	
	
	exit(0);
}
