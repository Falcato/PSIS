#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){

	int i = 0;
	
	for(i=0; argv[i]!=NULL; i++){
		
		printf("%s", argv[i]);

	}
	printf("\n");
	exit(0);
}
