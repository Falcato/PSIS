//gcc server.c -lpthread -o server
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include "storyserver.h"
#include <pthread.h>
#include "hash.h"
 
//the thread function
void *connection_handler(void *);

hashtable_t *hashtable;

int main(){
	
	
	int fd, newfd, addrlen;
	struct sockaddr_in addr;
	//int n;
	
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
	puts("bind done");
	
	//Listen
	if(listen(fd,5) == -1)
	exit(1);//error
	
	addrlen = sizeof(addr);
	pthread_t thread_id;
	
	hashtable = ht_create( 300 );
	
	while( (newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) ){
			
		puts("Connection accepted");
		if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &newfd) < 0 ){
            perror("could not create thread");
            exit (1);
        }
        puts("Thread created and handle");
	}
	exit(0);
}

// This will handle connection for each client
void *connection_handler(void *socket_desc){
    
    int newfd = *(int*)socket_desc;
	int n;
	char buffer[128];
	char * buf_value;
	message1 m_read, m_write;
	//int value_length = 3;
    
    
	//READ THE MESSAGE TYPE AND ASSOCIATED KEY
	n = read(newfd, &m_read, sizeof(m_read));
	if(n == -1){
		printf("Erro: \n");
		exit(1);//error
	}
	
	printf("tipo:%d, chave:%d \n", m_read.type_msg, m_read.key);
	
	//IF TYPE IS WRITE
	if(m_read.type_msg == WRITE){
		
		//ver se o sucesso é antes ou depois de escrever a key,value
					//send success or fail message
		m_write.type_msg = SUCCESS; //or FAIL
		m_write.key = m_read.key;
		n = write(newfd, &m_write, sizeof(m_write));
		if(n == -1){
			printf("Erro: \n");
			exit(1);//error
		}
		
		//read the next message
		n = read(newfd, buffer, m_read.size);
		
		buf_value = buffer;
		ht_set(hashtable, m_write.key, buf_value);
		printf("recebi:******%s*****\n", buffer);
		bzero(buffer,128);
		
	}
	
	//IF TYPE IS READ
	if(m_read.type_msg == READ){
		buf_value = ht_get( hashtable, m_read.key);
		//Try to read the value from the hash
		if( buf_value == NULL){
			m_write.type_msg = FAIL;
			m_write.key = m_read.key;
			n = write(newfd, &m_write, sizeof(m_write));
			if(n == -1){
				printf("Erro: \n");
				exit(1);//error
			}
		}else{
			m_write.type_msg = SUCCESS;
			m_write.key = m_read.key;
			
			strcpy(buffer, buf_value);
			m_write.size = strlen(buffer);
			
			n = write(newfd, &m_write, sizeof(m_write));
			if(n == -1){
				printf("Erro: \n");
				exit(1);//error
			}
			
			n = write(newfd, buffer, m_write.size);
			printf("enviei:******%s*****\n", buffer);
			bzero(buffer,128);
			if(n == -1){
				printf("Erro: \n");
				exit(1);//error
			}				
		}						
	}   
    
    //IF TYPE IS DELETE
    if(m_read.type_msg == DELETE){
		
		if(ht_get(hashtable, m_read.key) == NULL){
			printf("The key %d is not stored\n", m_read.key);
		}else{
			ht_remove(hashtable, m_read.key);
		}
		
	}
    
    if(n == 0){
        puts("Client disconnected");
        fflush(stdout);
    }else if(n == -1){
        perror("recv failed");
    }
         
    return 0;
} 




