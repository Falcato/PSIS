#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>


void tarefa(int indice){
	
	int start, end;
	
	unsigned int  m_7, m_19, m_7_19;
	m_7 = m_19 = m_7_19 = 0;
	
	if(indice==0) start = 0;
	else start = UINT_MAX/3*indice+1;
	
	end = UINT_MAX/3*(indice+1);
	
	
	for (unsigned int  i = start; i <  end; i++){
			if(i%7 == 0){ 
				m_7 ++;
			}
			if(i%19 == 0){ 
				m_19 ++;
			}
			if((i%7 == 0)	&& (i%19 == 0)){
				m_7_19++;
			}
		}
		printf("m 7    %d\n", m_7);
		printf("m   19 %d\n", m_19);
		printf("m 7 19 %d\n", m_7_19);
	
}


int main(){
		
	int j;
	
	for(j=0; j<3; j++){
		if(fork()==0) break;
	}
	
	//PARENT
	if(j == 3){
		wait(0);
		wait(0);
		wait(0);
	}else{
	
		tarefa(j);
	}
}
