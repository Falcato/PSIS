//gcc server.c -lpthread -o server
 
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include "hash.h"
#include "storyserver.h" 


//the thread function
void *connection_handler(void *);

//hash global variable
hashtable_t *hashtable;

//mutex global variable
pthread_mutex_t mux;

//log counter
int x = 0;

// read/write lock
pthread_rwlock_t  rwlock;
pthread_rwlock_t  rwlock2;


//signal global variable
volatile sig_atomic_t stop;

//FILE global variable
FILE *f;

//FIFO global variable
char * myFIFO = "/tmp/myfifo";

//shutdown signal handler
void inthand(int signum) {
	printf("\nRECEIVED SHIT DOWN SIGNAL\n");
	//PRINT THE HASH TO TERMINAL
	ht_print(hashtable);
	
	//OPEN FILE TO WRITE
	f = fopen("file.txt", "w");
	
	//SAVE THE HASH TO THE FILE
	ht_save(hashtable, f);
	
	//CLOSE THE FILE
	fclose(f);
    exit(2);
}


// This will handle connection for each client
void *connection_handler(void *socket_desc){
    int a;
    int n;
    int newfd = *(int*)socket_desc;
	char buffer[128];
	char * buf_value;
	message1 m_read, m_write;
	
	if(x == 100){
		//OPEN FILE TO WRITE
		f = fopen("file.txt", "w");
		
		//SAVE THE HASH TO THE FILE
		ht_save(hashtable, f);
		
		//CLOSE THE FILE
		fclose(f);
	}
	
	while(read(newfd, &m_read, sizeof(m_read)) > 0){ 
		
		//RECEIVE THE MESSAGE TYPE AND ASSOCIATED KEY
		n = 9;
		if( -1 == n ){
			printf("Error reading the message type\n");
			exit(1);//error
		}
		if(n == 0){
			puts("Client disconnected");
			fflush(stdout);
			return 0;
		}
		printf("tipo:%d, chave:%u \n", m_read.type_msg, m_read.key);
		
		//IF TYPE IS WRITE OR OVERWRITE
		if( m_read.type_msg == WRITE  || m_read.type_msg == OVERWRITE){
			
			//RECEIVE THE CONTENT TO SAVE IN THE HASH
			read(newfd, buffer, m_read.size);
			buf_value = buffer;
			printf("recebi:******%s*****\n", buffer);
			//bzero(buffer,128);	
			
			//MUTEX LOCK
			//pthread_mutex_lock(&mux);
			
			if(m_read.type_msg == WRITE){
				a = ht_set(hashtable, m_read.key, buf_value, 0);
			}else{
				a = ht_set(hashtable, m_read.key, buf_value, 1);
			}
		
			//MUTEX UNLOCK
			//pthread_mutex_unlock(&mux);
			
			
			//SEND THE RESULT
			if(a == -2){
				m_write.type_msg = FAIL;
				m_write.key = m_read.key;
				
				if( -1 == write(newfd, &m_write, sizeof(m_write)) ){
					printf("Erro: \n");
					exit(1);//error
				}
			}else{
				//OPEN FILE TO APPEND
				f = fopen("file.txt", "a");
				
				//WRITE LOG MESSAGE TO FILE		
				if(m_read.type_msg == WRITE ){
					fprintf(f, "%s %u %s\n", "wr0", m_read.key, buffer);
				}else{
					fprintf(f, "%s %u %s\n", "wr1", m_read.key, buffer);
				}
				x++;
				fclose(f);
				
				m_write.type_msg = SUCCESS;
				m_write.key = m_read.key;
		
				if( -1 == write(newfd, &m_write, sizeof(m_write)) ){
					printf("Erro: \n");
					exit(1);//error
				}
			}
			
		}
		
		//IF TYPE IS READ
		if(m_read.type_msg == READ){
			
			//MUTEX LOCK
			//pthread_mutex_lock(&mux);
			
			//Try to read the value from the hash
			buf_value = ht_get( hashtable, m_read.key);
			
			//MUTEX UNLOCK
			//pthread_mutex_unlock(&mux);
			
			//IF THE KEY DOESNT EXIST IN THE HASH
			if( buf_value == NULL){
				m_write.type_msg = FAIL;
				m_write.key = m_read.key;
				if(-1 == write(newfd, &m_write, sizeof(m_write)) ){
					printf("Erro: \n");
					exit(1);//error
				}
			}else{
				//IF THE KEY EXISTS
				m_write.type_msg = SUCCESS;
				m_write.key = m_read.key;
				
				strcpy(buffer, buf_value);// verificar se é feito atraves de strcpy e depois strlen
				m_write.size = strlen(buffer);//ou se se deve fazer strncpy com n a ser o numero de value_length
				
				//SEND THE RESULT 
				if(-1 == write(newfd, &m_write, sizeof(m_write))){
					printf("Erro: \n");
					exit(1);//error
				}
				
				//SEND THE CONTENT STORED WITH THAT KEY
				if(-1 == write(newfd, buffer, m_write.size) ){
					printf("Erro: \n");
					exit(1);//error
				}
				printf("enviei:******%s*****\n", buffer);
				bzero(buffer,128);			
			}						
		}   
		
		//IF TYPE IS DELETE
		if(m_read.type_msg == DELETE){
			int flag = 0;
			
			//MUTEX LOCK
			//pthread_mutex_lock(&mux);
			
			if(ht_get(hashtable, m_read.key) == NULL){
				printf("The key %u is not stored\n", m_read.key);
				flag = -1;
			}else{
				flag = ht_remove(hashtable, m_read.key);
			}
			//MUTEX UNLOCK
			//pthread_mutex_unlock(&mux);
			
			if(flag == 1){
				//OPEN FILE TO APPEND
				f = fopen("file.txt", "a");
				//WRITE LOG MESSAGE TO FILE		
				fprintf(f, "%s %u %s\n", "rm", m_read.key, "remove");
				x++;
				fclose(f);
				
				m_write.type_msg = SUCCESS;
				m_write.key = m_read.key;
				if(-1 == write(newfd, &m_write, sizeof(m_write)) ){
					printf("Erro: \n");
					exit(1);//error
				}
			}else{
				m_write.type_msg = FAIL;
				m_write.key = m_read.key;
				if(-1 == write(newfd, &m_write, sizeof(m_write)) ){
					printf("Erro: \n");
					exit(1);//error
				}
			}
			
		}
		
		
	}
	puts("Client disconnected");
	fflush(stdout);
    /*else if(n == -1){
        perror("recv failed");
    }*/
         
    return 0;
} 

void *FIFO_handler(){
	char buf[128];
	int fda;
    /* open, read, and display the message from the FIFO */
    fda = open(myFIFO, O_RDWR);
    printf("aaa\n");
    while(1){
		read(fda, buf, 128);
		
		if( strcmp(buf, "quit\n") == 0){
			close(fda);
			//unlink(myFIFO);
			printf("\nRECEIVED SHIT DOWN SIGNAL\n");
			ht_print(hashtable);
			ht_save(hashtable, f);
			fclose(f);
			exit(2);   
		}
	}
}

int main(){
	int fd, newfd, addrlen;
	struct sockaddr_in addr;
	int open = 1;
	
	pthread_rwlock_init(&rwlock, NULL);	
	pthread_rwlock_init(&rwlock2, NULL);	
	
	//OPEN FILE IN READ MODE
	f = fopen("file.txt", "r");
	if (f == NULL){
		open = -1;
	}	

	
	//Create socket
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);
	
	//Bind
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) exit(1);
	puts("Bind done");
	
	//Listen
	if(listen(fd,5) == -1)
	exit(1);//error
	
	addrlen = sizeof(addr);
	pthread_t thread_id;
	pthread_t thread_id2;
	
	//CREATE HASH
	hashtable = ht_create( 300 );
	
	//READ HASH FROM FILE
	if(open == 1){
		ht_read(hashtable, f);
		fclose(f);
	}
	
	//RECIEVES SHUTDOWN SIGNAL 
	signal(SIGINT, inthand);
	
	
	pthread_create( &thread_id2 , NULL ,  FIFO_handler , NULL);
	
	
	while( (newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) ){
			
		//puts("Connection accepted");
		if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &newfd) < 0 ){
            perror("could not create thread");
            exit (1);
        }
        pthread_join(thread_id, NULL);
        //puts("Thread created and handle");
	}
	exit(0);
}
