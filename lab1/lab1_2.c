#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char * argv[]){

	int i = 0;
	char c;
	
	for(i=0; argv[i]!=NULL; i++){
		
		int j=0;
		while(argv[i][j]!='\0'){
			
			if(argv[i][j]<123 && argv[i][j]>96)	c=argv[i][j]-32;
			else c=argv[i][j];
			
			printf("%c", c);
			j++;
		
		}
	}
	printf("\n");
	exit(0);
}
