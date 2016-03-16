#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>



int main(){
	
	clock_t launch = clock();
	
    int i;
		

	int fd1;
	int fd2;
	
	mknod("fifo1", S_IFIFO | 0666, 0);
	mknod("fifo2", S_IFIFO | 0666, 0);
	
	fd1 = open("fifo1", O_RDWR);
	fd2 = open("fifo2", O_RDWR);
	
	int token;

	for(i=0; i < 20; i++){
	  
	  token = random();
		  
	  write(fd1, &token, sizeof(int));
	  
	  read(fd2, &token, sizeof(int));
    
    }

	close(fd1);
	close(fd2);

	clock_t done = clock();
	double diff = (double)(done - launch) / CLOCKS_PER_SEC;
	
	printf("time with FIFO %e\n", diff);


}
