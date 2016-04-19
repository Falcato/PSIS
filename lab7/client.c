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

int kv_write(int kv_descriptor, int key, char * value, int value_length){
	
	int n;

	message1 mensagem;
	message1 mensagem2;
	mensagem.key = key;
	mensagem.type_msg = 1;
	mensagem.size = value_length;
	
	n = write(kv_descriptor, &mensagem, sizeof(mensagem));
	if(n == -1){
		printf("Erro: \n");
		exit(1);//error
	}
	n = read(kv_descriptor, &mensagem2, sizeof(mensagem2));
	if(mensagem2.type_msg == FAIL){
		printf("falhou\n");
	}else{	
		n = write(kv_descriptor, value, value_length);
		printf("enviei:.....%s....\n", value);
		if(n == -1){
			printf("Erro: \n");
			exit(1);//error
		}
	}	
	return 0;
}

int kv_read(int kv_descriptor, int key, char * value, int value_length){
	
	int n;
	
	message1 mensagem;
	message1 mensagem2;
	mensagem.key = key;
	mensagem.type_msg = 2;
	char b[value_length];
	
	n = write(kv_descriptor, &mensagem, sizeof(mensagem));
	if(n == -1){
		printf("Erro: \n");
		exit(1);//errorr
	}
	n = read(kv_descriptor, &mensagem2, sizeof(mensagem2));
	if(mensagem2.type_msg == FAIL){
		printf("falhou\n");
	}else{
		n = read(kv_descriptor, b, value_length);
		value = b;
		printf ("recebi:-------%s--------\n", value);
		if(n == -1){
			printf("Erro: \n");
			exit(1);//errorr
		}
	}

	return 0;
}


int main(){
	
	char e[] = "abdcde";
	char *d;
	int a = kv_connect("127.0.0.1", 9000);
	
	int c = kv_write(a, 3, e, strlen(e));
	kv_close(a);
	
	int x = kv_connect("127.0.0.1", 9000);
	int b = kv_read(x, 3, d, strlen(e));
	
	
	
	exit(0);
	
}
