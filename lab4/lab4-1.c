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

#include "lab4-1.h"

void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(){
	
	clock_t launch = clock();
	
	char *memname = "odd_even";
	int i;
		
	shm_unlink(memname);
	int fd = shm_open(memname, O_CREAT | O_RDWR, 0666);
	if (fd == -1)
		error_and_die("shm_open");

	int r = ftruncate(fd, sizeof(shm_region));
	if (r != 0)
		error_and_die("ftruncate");
		
	shm_region *ptr = mmap(0, sizeof(shm_region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
		error_and_die("mmap");
	close(fd);

	int fd1;
	int fd2;
	
	mknod("fifo1", S_IFIFO | 0666, 0);
	mknod("fifo2", S_IFIFO | 0666, 0);
	
	fd1 = open("fifo1", O_RDWR);
	fd2 = open("fifo2", O_RDWR);
	
	int token;

	for(i=0; i < 20; i++){
	  
	  ptr->value = random();
	  
	  token = ptr->value;
		  
	  write(fd1, &token, sizeof(int));
	  
	  read(fd2, &token, sizeof(int));
    
    }

	close(fd1);
	close(fd2);
	
	shm_unlink(memname);
	
	clock_t done = clock();
	double diff = (double)(done - launch) / CLOCKS_PER_SEC;
	
	printf("time with shmem and pipe %e\n", diff);

}
