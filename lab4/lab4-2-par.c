
#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



int main(){
	
    int i;

	i = 0;
	int fd_token, fd1, fd2;
	
	fd1 = open("fifo1", O_RDWR);
	fd2 = open("fifo2", O_RDWR);
	
	
	while(1){
		
		read(fd1, &fd_token, sizeof(int));
		
  	  if(fd_token%2 == 0){
		  
	    printf("%d %d\n", i++, fd_token);
	    write(fd2, &fd_token, sizeof(int));
	    
	  }else write(fd1, &fd_token, sizeof(int));
	
	}

}
