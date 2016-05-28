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
#include "kv_lib.h"
#define MAX_VALUES 10


int main(){
	
	char linha[100];
	int kv = kv_connect("127.0.0.1", 9999);

	printf("press enter to fork and write values\n");
	getchar();
	
	fork();
	
	for (int i = 0; i < 10; i++){
		sprintf(linha, "%u", i);
		kv_write(kv, 0 , linha, strlen(linha)+1, 1);
	}

	if(kv_read(kv, 0 , linha, 1000) > 0){
		printf ("key - 0 value %s", linha);
	}


	kv_close(kv);
	
	exit(0);

}





/*
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

		}
	}
	printf("press enter to write values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		sprintf(linha, "%u", i*10);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}

	printf("press enter to read new values\n");
	getchar();
	for (uint32_t i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){

		}
	}

	
	kv_close(kv);
}
*/
