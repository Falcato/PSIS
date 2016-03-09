#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

struct res{

	unsigned int res_7;
	unsigned int res_19;
	unsigned int res_7_19;

};


void tarefa(int indice, struct res *v){
	
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
		
		(*v).res_7 = m_7;
		(*v).res_19 = m_19;
		(*v).res_7_19 = m_7_19;
		
	
}


int main(){
		
	int j;
	
	struct res * v; 
	
	
	v = mmap(NULL, 3*sizeof(struct res), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	
	
	
	for(j=0; j<3; j++){
		if(fork()==0) break;
	}
	
	//PARENT
	if(j == 3){
		
		wait(0);
		wait(0);
		wait(0);
		
		printf("m 7     %d\n", v[0].res_7+v[1].res_7+v[2].res_7);
		printf("m 19    %d\n", v[0].res_19+v[1].res_19+v[2].res_19);
		printf("m 7 19  %d\n", v[0].res_7_19+v[1].res_7_19+v[2].res_7_19);
			
	}else tarefa(j, &v[j]);
	
}
