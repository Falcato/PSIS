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

//the thread functions
void *connection_handler();
void *keyboard_handler();
void *FIFO_handler();

//the thread ids
pthread_t thread_id;
pthread_t thread_id2;
pthread_t thread_id3;

//signal global variable
volatile sig_atomic_t stop;

//FIFO FILES
char fifo_name1[] = "/tmp/fifo1";
char fifo_name2[] = "/tmp/fifo2";

//SHUTDOWN SIGNAL HANDLER
void inthand(int signum) {
	printf("\nRECEIVED SHIT DOWN SIGNAL\n");
	int fd1;	
	char msg [32];
	
	/* write to the FIFO */
	fd1 = open(fifo_name1, O_RDWR);
	strcpy(msg,"quit");
	write(fd1, msg, strlen(msg)+1);

	sleep(3);
	
    exit(2);
}

//NEW CLIENTS HANDLER
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
    
    
    char port[]="1234";
    char buff[4];
    while( (newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) ){
		bzero(buff, 4);
		if(read(newfd, buff,sizeof(char)*2) > 0){
			if(strcmp(buff, "1") == 0){
				if(-1 == write(newfd, port, 4)){
					printf("Erro: sending port\n");
				}
			}else if(strcmp(buff, "0") == 0){
				if(read(newfd, buff,sizeof(char)*4) > 0)
				strcpy(port, buff);
				port[4]='\0';
			}
		}
	}
}

//KEYBOARD HANDLER
void *keyboard_handler(){
	char buf2[32];
	char msg[32];
	bzero(buf2, 32);
	
    //OPEN FIFO TO READ AND WRITE
    int s2c = open(fifo_name1, O_RDWR);
    
	//RECIEVES SHUTDOWN SIGNAL 
	signal(SIGINT, inthand);
	
	while (1){
		
		if(read(0, buf2, 32) > 0){
			if(strcmp(buf2,"quit\n") == 0){
				
				strcpy(msg,"quit");
				write(s2c, msg, strlen(msg)+1);
								
				sleep(3);
				exit(0);
			}
        }
		bzero(buf2, 32);
	}
}

//FIFO HANDLER
void *FIFO_handler(){
    int c2s, s2c, c=0, i;
	char msg[80], buf[10];
    pid_t id;
    struct stat st;		
    
    // CREATE FIFOS IF THEY DONT EXIST
    if (stat(fifo_name1, &st) != 0)
        mkfifo(fifo_name1, 0666);
    if (stat(fifo_name2, &st) != 0)
        mkfifo(fifo_name2, 0666);
        
    //OPEN FIFOS TO READ AND WRITE
    s2c= open(fifo_name1, O_RDWR);
    c2s= open(fifo_name2, O_RDWR);
    //NON-BLOCK READ FROM FIFO
	fcntl(s2c, F_SETFL, O_NONBLOCK);
	fcntl(c2s, F_SETFL, O_NONBLOCK);
	
	//COUNTER TO CHECK FOR NON RESPONSIVE PROGRAM
	c = 0;	
	
    //SENDING FIRST MESSAGE
	strcpy(msg,"front");
	write(s2c, msg, strlen(msg)+1);
    while (1){

		//READ FROM THE FIFO
		i = read(c2s, buf, sizeof(char)*10);
		
		if ( i > 0){
			
			c=0;
			//WRITE TO THE FIFO
			strcpy(msg,"front");
			write(s2c, msg, strlen(msg)+1);
		}
		
		sleep(2);
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
				strcpy(msg,"front");
				write(s2c, msg, strlen(msg)+1);

			}else if(id == 0){
				//CHILD
				char *envp[] = { NULL };
				char *argv[] = { NULL };
				printf("Data server Rebooted\n");
				execve("./data", argv, envp );
			}
		}	
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
 	if(pthread_create(&thread_id3, NULL , FIFO_handler, NULL) < 0 ){
        perror("could not create thread");
        exit (1);
    }
	pthread_join(thread_id, NULL);
	pthread_join(thread_id2, NULL);
	pthread_join(thread_id3, NULL);
	
	exit(0);
}

