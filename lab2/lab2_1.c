
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
	
	while(i<10){
	
		int pid2 = fork();
		int randnr=rand()%10;
		
		i++;
		if(pid2==0){
			//RESTANTES FILHOS
			printf("BEFORE SLEEP PID - %d SLEEP TIME - %d\n", getpid(), randnr);

			sleep(randnr);

			printf("AFTER  SLEEP PID - %d SLEEP TIME - %d\n", getpid(), randnr);
			exit(randnr);
		}
	//PAI
	
	}
	
	exit(0);
}
