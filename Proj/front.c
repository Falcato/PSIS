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
#define DATA_PORT 9000

//the thread functions
void *connection_handler();
void *keyboard_handler();

//the thread ids
pthread_t thread_id;
pthread_t thread_id2;

//signal global variable
volatile sig_atomic_t stop;



//shutdown signal handler
void inthand(int signum) {
	printf("\nRECEIVED SHIT DOWN SIGNAL\n");
	char * myFIFO = "/tmp/myfifo";
	int fd1;	
	/* create the FIFO (named pipe) */
	mkfifo(myFIFO, 0666);

	/* write to the FIFO */
	fd1 = open(myFIFO, O_RDWR);
	write(fd1, "quit\n", sizeof("quit\n"));
	close(fd1);
	
    exit(2);
}

// This will handle connection for each client
void *connection_handler(){
	
	int fd, newfd, addrlen;
	struct sockaddr_in addr;
	
	//Create socket
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9999);
	
	//Bind
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) exit(1);
	puts("Bind done");
	
	//Listen
	if(listen(fd,5) == -1)
	exit(1);//error
	
	
	//RECIEVES SHUTDOWN SIGNAL 
	signal(SIGINT, inthand);
	
	addrlen = sizeof(addr);    
    
    
    while( (newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) ){
		
		if(-1 == write(newfd, "9000", 4)){
			printf("Erro: \n");
		}
	}
	
}

// This will handle the keyboard inputs
void *keyboard_handler(){
	char buf[32];
	bzero(buf, 32);
	
	//RECIEVES SHUTDOWN SIGNAL 
	signal(SIGINT, inthand);
	
	while (1){
		
		if(read(0, buf, 32) > 0){
			if(strcmp(buf,"quit\n") == 0){
				int fd1;
				char * myFIFO = "/tmp/myfifo";
		
				/* create the FIFO (named pipe) */
				mkfifo(myFIFO, 0666);

				/* write to the FIFO */
				fd1 = open(myFIFO, O_RDWR);

				write(fd1, buf, sizeof(buf));
				close(fd1);
				
				bzero(buf, 32);
				exit(0);
			}
        }
		bzero(buf, 32);
	}
}



int main(){
	
	//RECIEVES SHUTDOWN SIGNAL 
	signal(SIGINT, inthand);
	
	if(pthread_create(&thread_id, NULL, connection_handler, NULL) < 0 ){
        perror("could not create thread");
        exit (1);
    }
	if(pthread_create(&thread_id2, NULL , keyboard_handler, NULL) < 0 ){
        perror("could not create thread");
        exit (1);
    }        
    
	pthread_join(thread_id, NULL);
	pthread_join(thread_id2, NULL);
	
	exit(0);
}
/*
TODO

VERIFICAR NOVAMENTE OS LOCKS DE ACORDO COM A PROCURA ASSOCIADA A UMA ESCRITA OU DELETE->done confirmar que e correcto
FAZER O ALGORITMO DE DETECÇAO DE SAIDA, FRONT ESCREVE IMPAR E DATA ESCREVE PARES NO FIFO->check (FALTA IMPLEMENTAR PARA OS SERVERS)
FAZERF CTRL+C NO FRONT SERVER PARA ENVIAR MENSAGEM DE SAIDA PARA A FIFO->check
FAZERF CTRL+C NO DATA SERVER PARA ENVIAR MENSAGEM DE SAIDA PARA A FIFO->acho que nao e suposto
FAZER BACKUP QUANDO SAI PELA FIFO (DATA)->check
OPTIMIZAR BACKUP (A CADA 100 INSTRUÇOES APAGAR LOG E COPIAR A HASH)->check
ALOCAÇAO E FREES


*/
