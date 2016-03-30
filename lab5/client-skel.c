#include "storyserver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_ADDRESS "/tmp/sock_1"

int main(){
    
    struct sockaddr_un server_addr;
    message m;
    int nbytes;
    
    printf("message: ");
    fgets(m.buffer, MESSAGE_LEN, stdin);

	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	
	printf(" socket created \n Ready to send\n");

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

    /* write message */
    nbytes = sendto(sock_fd, 
	                    &m, sizeof(m), 0, 
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
	printf("sent %d %s\n", nbytes, m.buffer);
    
    
    /* receive story */
/*
	nbytes = recv(sock_fd, &m, sizeof(m), 0);
	printf("received %d %s\n", nbytes, m.buffer);
*/

    printf("OK\n");
    
    close(sock_fd);
    exit(0);
    
}
