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
#define PORT 9000

//the thread function
void *connection_handler(void *);
void *FIFO_handler();

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

//SHUTDOWN SIGNAL HANDLER
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

//NEW CLIENT HANDLER
void *connection_handler(void *socket_desc){
    int a;
    int n;
    int newfd = *(int*)socket_desc;
	char buffer[128];
	char * buf_value;
	message1 m_read, m_write;
	
	
	while(read(newfd, &m_read, sizeof(m_read)) > 0){ 
		

		if(x == 100){
			//OPEN FILE TO WRITE
			f = fopen("file.txt", "w");
			
			//SAVE THE HASH TO THE FILE
			ht_save(hashtable, f);
			
			//CLOSE THE FILE
			fclose(f);
			
			x = 0;
		}




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
			
			char * newbuff = malloc(sizeof(char)*m_read.size);
			
			//RECEIVE THE CONTENT TO SAVE IN THE HASH
			read(newfd, newbuff, m_read.size);
			//buf_value = buffer;
			//printf("recebi:******%s*****\n", newbuffer);
			printf("recebi:******%s*****\n", newbuff);
			

			
			if(m_read.type_msg == WRITE){
				//a = ht_set(hashtable, m_read.key, buf_value, 0);
				a = ht_set(hashtable, m_read.key, newbuff, m_read.size, 0);
			}else{
				//a = ht_set(hashtable, m_read.key, buf_value, 1);
				a = ht_set(hashtable, m_read.key, newbuff, m_read.size, 0);
			}
			
			
			//SEND THE RESULT
			if(a == -2){
				m_write.type_msg = FAIL;
				m_write.key = m_read.key;
				free(newbuff);
				if( -1 == write(newfd, &m_write, sizeof(m_write)) ){
					printf("Erro: \n");
					exit(1);//error
				}
			}else{
				//OPEN FILE TO APPEND
				f = fopen("file.txt", "a");
				
				//WRITE LOG MESSAGE TO FILE		
				if(m_read.type_msg == WRITE ){
					fprintf(f, "%s %u %s %u\n", "wr0", m_read.key, newbuff, m_read.size);
				}else{
					fprintf(f, "%s %u %s %u\n", "wr1", m_read.key, newbuff, m_read.size);
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
			
			
			//Try to read the value from the hash
			char * newbuff2 = malloc(sizeof(char)*m_read.size);
			//memcpy(newbuff2, ht_get( hashtable, m_read.key), m_read.size);
			newbuff2 = ht_get( hashtable, m_read.key);
			
			
			//IF THE KEY DOESNT EXIST IN THE HASH
			if( newbuff2 == NULL){
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
				
				
				m_write.size = ht_get_length(hashtable, m_read.key);
				//SEND THE RESULT 
				if(-1 == write(newfd, &m_write, sizeof(m_write))){
					printf("Erro: \n");
					exit(1);//error
				}
				
				//SEND THE CONTENT STORED WITH THAT KEY
				if(-1 == write(newfd, newbuff2, m_write.size) ){
					printf("Erro: \n");
					exit(1);//error
				}
				printf("enviei:******%s*****\n", newbuff2);
				bzero(buffer,128);			
			}						
		}   
		
		//IF TYPE IS DELETE
		if(m_read.type_msg == DELETE){
			int flag = 0;
			
			
			if(ht_get(hashtable, m_read.key) == NULL){
				printf("The key %u is not stored\n", m_read.key);
				flag = -1;
			}else{
				flag = ht_remove(hashtable, m_read.key);
			}

			
			if(flag == 1){
				//OPEN FILE TO APPEND
				f = fopen("file.txt", "a");
				//WRITE LOG MESSAGE TO FILE		
				fprintf(f, "%s %u %s %u\n", "rm", m_read.key, "remove", 0);
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
        
    return 0;
} 

//FIFO HANDLER
void *FIFO_handler(){
    int c2s, s2c, c=0, i;
	char msg[80], buf[10];
    char fifo_name1[] = "/tmp/fifo1";
    char fifo_name2[] = "/tmp/fifo2";
    pid_t id;
    
    //OPEN FIFOS TO READ AND WRITE
    s2c= open(fifo_name1, O_RDWR);
    c2s= open(fifo_name2, O_RDWR);
    //NON-BLOCK READ FROM FIFO
	fcntl(s2c, F_SETFL, O_NONBLOCK);
	fcntl(c2s, F_SETFL, O_NONBLOCK);
	
	//COUNTER TO CHECK FOR NON RESPONSIVE PROGRAM
	c = 0;	
    while(1){
		//READ FROM THE FIFO
		bzero(buf,10);
		i = read(s2c, buf, sizeof(char)*10);
		//if(i > 0) printf("----%s----\n", buf);
		if( i > 0 && strcmp(buf, "quit") == 0){
			
			//unlink(fifo_name1);
			//unlink(fifo_name2);
			printf("\nRECEIVED SHIT DOWN SIGNAL\n");

			
			ht_print(hashtable);
			
			//OPEN FILE TO WRITE
			f = fopen("file.txt", "w");
			ht_save(hashtable, f);

			fclose(f);

			exit(2);   
		}else if(i > 0){
			//printf("data received: %s \n", buf);
			c=0;
			
			//WRITE TO THE FIFO
			strcpy(msg,"data");
			write(c2s, msg, strlen(msg)+1);
			
		}
		
		sleep(1);
		c++;    
		if (c>6){
			//WAITS 5SEC THEN REBOOTS THE SERVER
			id = fork();
			if (id == -1){
				perror("Fork failed");
			}else if (id > 0){
				//FATHER
				c = 0;
				sleep(3);
				
				//SENDING FIRST MESSAGE
				strcpy(msg,"data");
				write(s2c, msg, strlen(msg)+1);
			}else if(id == 0){
				//CHILD
				char *envp[] = { NULL };
				char *argv[] = { NULL };
				printf("Front Server Rebooted\n");
				execve("./front", argv, envp );
			}

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
	addr.sin_port = htons(9999);
	
	//SEND THE DATA SERVER PORT TO THE FRONT SERVER
	int front = socket(AF_INET, SOCK_STREAM, 0);
	int n = connect(front,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1){
		printf("Erro on front server\n");
		exit(1);//error
	}
	
	char port[32];
	if(-1 == write(front, "0",sizeof(char))){
		printf("Error\n");
		exit(1);
	}
	sprintf(port,"%d",PORT);
	if(-1 == write(front, port,sizeof(char)*4)){
		printf("Error\n");
		exit(1);
	}
	close(front);
	
	
	
	
	addr.sin_port = htons(PORT);
	
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
			
		
		if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &newfd) < 0 ){
            perror("could not create thread");
            
            exit (1);
        }
        pthread_join(thread_id, NULL);
        
	}
	exit(0);
}
