#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>



pthread_mutex_t mux;

#define N_THREADS 4
#define MAX_VAL UINT_MAX 

unsigned int counter;

void * thread_code(void *arg){
	unsigned int start;
	unsigned int  end;
	unsigned int priv_count = 0;
	int thread_indx =(int)  arg;
	if (thread_indx == 0){
		start = 0;
	}else{
		start = MAX_VAL/N_THREADS * thread_indx +1;
	}
	if (thread_indx == N_THREADS -1){
		end = MAX_VAL;
	}else{
		end = MAX_VAL/N_THREADS * (thread_indx +1) ;
	}
	
	printf("thread %d \t start (%u) \t end %u \t thread_id %ul\n", thread_indx, start, end, pthread_self());

	for (unsigned int  i = start; i <  end; i++){
			
		pthread_mutex_lock(&mux);
				
		// critical region
		counter ++;
//		printf(".\n");
		// critical region

		pthread_mutex_unlock(&mux);
		
		
	}
	printf("thread end %ul %u\n", pthread_self(), priv_count);
}

void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}


int main(){
	counter = 0;
	pthread_t thread_id[20];
		
	
	if(0 != pthread_mutex_init(&mux, NULL)){
		printf("mutex creation error\n");
		exit(-1);
	}
	
	for (int i = 0 ; i < N_THREADS; i++){
		pthread_create(&thread_id[i], 
						NULL, 
						thread_code, 
						(void *) i);
	}


	for (int i = 0; i < N_THREADS; i++){
		pthread_join(thread_id[i], NULL);
	}
	printf("resultado %u\n", counter);
	pthread_mutex_destroy(&mux);
	
}
