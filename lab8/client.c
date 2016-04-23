#include <stdlib.h>
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
#define MAX_VALUES 10

int kv_connect(char * kv_server_ip, int kv_server_port){
	
	int fd, n;
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
		printf("Erro: \n");
		exit(1);//error
	 }
	return fd;
}

void kv_close(int kv_descriptor){
	
	close(kv_descriptor);
}

int kv_write(int kv_descriptor, int key, char * value, int value_length, int kv_overwrite){

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
	printf("enviei:.....%s....\n", value);
	
	//RECEIVE THE RESULT
	if(-1 == read(kv_descriptor, &mensagem2, sizeof(mensagem2))){
		printf("Erro: \n");
		exit(1);//error
	}		
	if(mensagem2.type_msg == FAIL){
		printf("FAIL ON WRITE CLIENT SIDE\n");
		return -2;
	}else{
		return 0;
	}	
		
	return -1;
}

int kv_read(int kv_descriptor, int key, char * value, int value_length){
	
	message1 mensagem;
	message1 mensagem2;
	mensagem.key = key;
	mensagem.type_msg = 2;
	char b[value_length];
	
	//SEND THE INTENTION
	if(-1 == write(kv_descriptor, &mensagem, sizeof(mensagem))){
		printf("Erro: \n");
		exit(1);//errorr
	}
	//RECEIVE THE RESULT
	if(-1 == read(kv_descriptor, &mensagem2, sizeof(mensagem2))){
		printf("Erro: \n");
		exit(1);//errorr
	}
	//INTERPRET THE RESULT
	if(mensagem2.type_msg == FAIL){
		printf("FAIL ON READ CLIENT SIDE\n");
		return -2;
	}else{
		
		if(-1 == read(kv_descriptor, b, value_length)){
			printf("Erro: \n");
			exit(1);//errorr
		}
		
		b[value_length]='\0';
		value = b;
		printf ("recebi:-------%s--------\n", value);
		return 0;
	}

	return -1;
}

int kv_delete(int kv_descriptor, int key){


	return 0;
}


int main(){
	
	

	char linha[100];
	
	
	if(fork() == 0){
			
		int kv = kv_connect("127.0.0.1", 9000);

		for (int i = 0; i < MAX_VALUES; i +=2){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
		}
		kv_close(kv);


	}else{
		int kv = kv_connect("127.0.0.1", 9000);

		for (int i = 1; i < MAX_VALUES; i +=2){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
		}


		wait(NULL);
		printf("writing values\n");
		for (int i = 1; i < MAX_VALUES; i +=2){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
		}
			
		
		kv_close(kv);

	}
	exit(0);
	/*
	int a = kv_connect("127.0.0.1", 9000);
	char e[] = "millll";
	int c = kv_write(a, 1000, e, strlen(e), 0);
	kv_close(a);
	
	int q = kv_connect("127.0.0.1", 9000);
	char m[] = "cemm";
	int l = kv_write(q, 4, m, strlen(m), 0);
	kv_close(q);
	
	
	int x = kv_connect("127.0.0.1", 9000);
	char *d;
	int b = kv_read(x, 1000, d, strlen(e));
	kv_close(x);
	
	

	int y = kv_connect("127.0.0.1", 9000);
	char *z;
	int t = kv_read(y, 4, z, strlen(m));
	
	
	exit(0);*/
	
}
