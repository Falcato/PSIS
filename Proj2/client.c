#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include "storyserver.h"
#define MAX_VALUES 100

int kv_connect(char * kv_server_ip, int kv_server_port){
	
	int fd, fd2, n;
	struct sockaddr_in addr, ip_addr;
	char ip[128];
	
	fd = socket(AF_INET,SOCK_STREAM,0);//TCP socket	
	if(fd == -1){
		printf("Erro: \n");
		exit(1);//error
	}
	 
	//COPY AND TRANSLATE IP ADRESS
	strcpy(ip, kv_server_ip);
	inet_aton(ip, &ip_addr.sin_addr); 
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = ip_addr.sin_addr;
	addr.sin_port = htons(kv_server_port);
	
	n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1){
		printf("Erro on front server\n");
		exit(1);//error
	}
	
	char port[32];
	if(-1 == write(fd, "1",sizeof(char))){
		printf("Error\n");
		exit(1);
	}
	
	if(-1 == read(fd, port, sizeof(int))){
		printf("Error reading the data server port\n");
		exit(1);//error
	}		 
	
	close(fd);
	
	
	fd2 = socket(AF_INET,SOCK_STREAM,0);//TCP socket	
	if(fd == -1){
		printf("Error\n");
		exit(1);//error
	 }
	
	int x = atoi(port);
	addr.sin_port = htons(x);
	
	n = connect(fd2,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1){
		printf("Erro connecting to data server \n");
		exit(1);//error
	}
	
	return fd2;
}

void kv_close(int kv_descriptor){
	
	close(kv_descriptor);
}

int kv_write(int kv_descriptor, uint32_t key, char * value, uint32_t value_length, int kv_overwrite){

	message1 mensagem;
	message1 mensagem2;
	mensagem.key = key;
	mensagem.type_msg = 1;
	mensagem.size = value_length;
	if(kv_overwrite == 1) mensagem.type_msg = 4;
	
	//SEND INTENTION MESSAGE
	if(-1 == write(kv_descriptor, &mensagem, sizeof(mensagem))){
		printf("Erro: \n");
		exit(1);//error
	}
	
	//SEND CONTENT TO WRITE
	if(-1 == write(kv_descriptor, value, value_length)){
		printf("Erro: \n");
		exit(1);//error
	}
	puts(value);
	
	//RECEIVE THE RESULT
	if(-1 == read(kv_descriptor, &mensagem2, sizeof(mensagem2))){
		printf("Error read at kv_write for %s: \n", value);
		exit(1);//error
	}		
	if(mensagem2.type_msg == FAIL){
		printf("FAIL TO WRITE KEY[%u] - CLIENT SIDE (OVERWRITE ISSUE)\n", key);
		return -2;
	}else{
		return 0;
	}	
		
	return -1;
}

int kv_read(int kv_descriptor, uint32_t key, char * value, uint32_t value_length){
	
	message1 mensagem;
	message1 mensagem2;
	mensagem.key = key;
	mensagem.type_msg = 2;
	
	//SEND THE INTENTION
	if(-1 == write(kv_descriptor, &mensagem, sizeof(mensagem))){
		printf("Erro: \n");
		exit(1);//errorr
	}
	//RECEIVE THE RESULT
	if(-1 == read(kv_descriptor, &mensagem2, sizeof(mensagem2))){
		printf("Error read at kv_read \n");
		exit(1);//errorr
	}
	//INTERPRET THE RESULT
	if(mensagem2.type_msg == FAIL){
		printf("FAIL TO READ KEY[%u] - CLIENT SIDE\n", key);
		return -2;
	}else{
		/*char *b = malloc(sizeof(value_length));
		bzero(b, value_length);*/
		if(-1 == read(kv_descriptor, value, value_length)){
			printf("Error: \n");
			exit(1);//errorr
		}
		
		//b[value_length-1]='\0';
		/*b[mensagem2.size-1] = '\0';
		value = b;*/
		printf ("recebi:-------%s--------\n", value);
		//printf ("size:-------%d--------\n", mensagem2.size);
		return mensagem2.size;
	}

	return -1;
}

int kv_delete(int kv_descriptor, uint32_t key){
	
	message1 mensagem;
	message1 mensagem2;
	mensagem.key = key;
	mensagem.type_msg = 3;
	
	//SEND THE INTENTION
	if(-1 == write(kv_descriptor, &mensagem, sizeof(mensagem))){
		printf("Erro: \n");
		exit(1);//errorr
	}
	//RECEIVE THE RESULT
	if(-1 == read(kv_descriptor, &mensagem2, sizeof(mensagem2))){
		printf("Erro read at kv_delete\n");
		exit(1);//errorr
	}
	if(mensagem2.type_msg == FAIL){
		printf("KEY[%u] TO DELETE ISNT STORED\n", key);
		return -1;
	}else{
		printf("KEY[%u]-VALUE DELETED\n", key);
		return 0;
	}
	return 0;
}


int main(){
	
	char linha[100];
	int kv = kv_connect("127.0.0.1", 9999);

	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}

	printf("press enter to read values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s", i, linha);
		}
	}

	printf("press enter to delete even values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i +=2){
		kv_delete(kv, i);
	}

	printf("press enter to read values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s", i, linha);
		}
	}
	printf("press enter to write values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		sprintf(linha, "%u", i*10);
		kv_write(kv, i , linha, strlen(linha)+1, 0); /* will not overwrite*/
	}

	printf("press enter to read new values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s", i, linha);
		}
	}

	
	kv_close(kv);
}
