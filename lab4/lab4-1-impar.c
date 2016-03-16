#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lab4-1.h"

void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(){
	
	char *memname = "odd_even";
  int i;
		
	int fd = shm_open(memname,  O_RDWR, 0666);
	if (fd == -1)
		error_and_die("shm_open");

		
	shm_region *ptr = mmap(0, sizeof(shm_region), PROT_READ , MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
		error_and_die("mmap");
	close(fd);





	i = 0;

	int fd_token;
	
	int fd1, fd2;

	fd1 = open("fifo1", O_RDWR);
	fd2 = open("fifo2", O_RDWR);
	

	while(1){
		
		read(fd1, &fd_token, sizeof(int));
		
  	  if(fd_token%2 != 0){
	    printf("%d %d\n", i++, fd_token);
	    write(fd2, &fd_token, sizeof(int));
	    
	  }else write(fd1, &fd_token, sizeof(int));
	
	}

}
