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


int main(){
	
	
	int fd, newfd, addrlen;
	struct sockaddr_in addr;
	int n;
	char buffer[128] ;
	char * buf_value;
	message1 m_read, m_write;
	int value_length = 3;
	
	
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		exit(1);
	
	if(listen(fd,5) == -1)
	exit(1);//error
	
	
	while(1){
		
		addrlen = sizeof(addr);
		if((newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) == -1 )
			exit(1);//error
			
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
			n = read(newfd, buffer, m_read.size);
			printf("recebi:******%s*****\n", buffer);
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
				printf("enviei:******%s*****\n", buffer);
				n = write(newfd, buffer, strlen(buffer));
				if(n == -1){
					printf("Erro: \n");
					exit(1);//error
				}				
			}						
		}
	}
	
	
	exit(0);
}
