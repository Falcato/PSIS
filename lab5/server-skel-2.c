#include "storyserver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <signal.h>

#define SOCK_ADDRESS "/tmp/sock_1"

void my_sig_handler(int n){
    
    printf("received signal %d\n", n);
    
    exit(2);
    return;
}


int main(){
    
    message m;
    char * story;
	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	int nbytes;
    socklen_t size_addr;
        
	story = strdup("");
	signal(SIGINT, my_sig_handler);
	
	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	

	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);
	int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}
	
	printf(" socket created and binded \n Ready to receive messages\n");
        
    while(1){
        /* read message */
        size_addr = sizeof(client_addr);
		nbytes = recvfrom(sock_fd, &m, sizeof(m), 0, (struct sockaddr *)&client_addr, &size_addr);
		printf("received %d %s\n", nbytes, m.buffer);
		
        /* process message */
        
        
        strcat(story, m.buffer);
        printf("Story: %s", story);
           
        
        nbytes = sendto(sock_fd, 
	                    story, strlen(story)+1, 0, 
	                    (const struct sockaddr *) &client_addr, sizeof(client_addr));
		printf("sent %d %s\n", nbytes, story);
        
     }
    printf("OK\n");
    
    close(sock_fd);
    exit(0);
    
}
