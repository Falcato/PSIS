/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
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
#include <pthread.h> //for threading , link with lpthread
 
//the thread function
void *connection_handler(void *);


// This will handle connection for each client
void *connection_handler(void *socket_desc){
    
    int newfd = *(int*)socket_desc;
	int n;
	char buffer[128] ;
	char * buf_value;
	message1 m_read, m_write;
	int value_length = 3;
    
    
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
		printf("recebi:******%s*****\n", buffer);
		//buf = buffer;
		//do stuff
		
		//send success or fail message
		/*m_write.type_msg = SUCCESS; //or FAIL
		m_write.key = m_read.key;
		n = write(newfd, &m_write, sizeof(m_write));
		if(n == -1){
			printf("Erro: \n");
			exit(1);//error
		}*/
	}
	
	//IF TYPE IS READ
	if(m_read.type_msg == READ){
		//If success on reading the hash send the pair (key,value)
		//else send FAIL message
		if(1==2/*fail to read the hash*/){
			
			m_write.type_msg = FAIL;
			m_write.key = m_read.key;
			n = write(newfd, &m_write, sizeof(m_write));
			if(n == -1){
				printf("Erro: \n");
				exit(1);//error
			}
		}
		
		if(3==3/*success to read the hash*/){
			m_write.type_msg = SUCCESS;
			m_write.key = m_read.key;
			//m_write.size = m_read.size;
			n = write(newfd, &m_write, sizeof(m_write));
			if(n == -1){
				printf("Erro: \n");
				exit(1);//error
			}
			/*printf("enviei:******%s*****\n", *(char*)buffer);
			n = write(newfd, *(char*)buffer, strlen(*(char*)buffer));*/
			char *temp;
			temp = "abcde";
			n = write(newfd, temp, strlen(temp));
			printf("enviei:******%s*****\n", temp);
			if(n == -1){
				printf("Erro: \n");
				exit(1);//error
			}				
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



int main(){
	
	
	int fd, newfd, addrlen;
	struct sockaddr_in addr;
	int n;
	char buffer[128];
	char * buf_value;
	message1 m_read, m_write;
	int value_length = 3;
	
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




