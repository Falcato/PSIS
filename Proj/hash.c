#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include "hash.h"

extern pthread_rwlock_t  rwlock;
extern pthread_rwlock_t  rwlock2;

/* Create a new hashtable. */
hashtable_t *ht_create( int size ) {

	hashtable_t *hashtable = NULL;
	int i;

	if( size < 1 ) return NULL;

	/* Allocate the table itself. */
	if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < size; i++ ) {
		hashtable->table[i] = NULL;
	}

	hashtable->size = size;

	return hashtable;	
}

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, char *key ) {

	unsigned long int hashval;
	int i = 0;

	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen( key ) ) {
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}

	return hashval % hashtable->size;
}


uint32_t ht_hash1(hashtable_t *hashtable, uint32_t a){
    /*a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);*/
    return a % hashtable->size;
}


//Create a pair
entry_t *ht_newpair( uint32_t key, char *value ) {
	entry_t *newpair;

	if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
		return NULL;
	}

	newpair->key = key;
		
	if( ( newpair->value = strdup( value ) ) == NULL ) {
		return NULL;
	}

	newpair->next = NULL;

	return newpair;
}

//Insert a pair into a hash table
int ht_set( hashtable_t *hashtable, uint32_t key, char *value, int flag) {
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );

	next = hashtable->table[ bin ];
	
	//READ LOCK
	pthread_rwlock_rdlock(&rwlock);
	
	while( next != NULL && key != next->key ) {
		last = next;
		next = next->next;
	}
	
	//READ UNLOCK
	
	//lck(&rwlock2)
	pthread_rwlock_unlock(&rwlock);
	
	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && key == next->key ) {
		
		if(flag == 1){
			
			//WRITE LOCK
			pthread_rwlock_wrlock(&rwlock2);
				
			free( next->value );
			next->value = strdup( value );
			
			//WRITE UNLOCK
			pthread_rwlock_unlock(&rwlock2);
			
			return 2;
		}else{
			return -2;
		}

	/* Nope, could't find it.  Time to grow a pair. */
	}else{
		newpair = ht_newpair( key, value );

		/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			//WRITE LOCK
			pthread_rwlock_wrlock(&rwlock2);
			
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
			
			//WRITE UNLOCK
			pthread_rwlock_unlock(&rwlock2);
			
			return 0;
		/* We're at the end of the linked list in this bin. */
		}else if( next == NULL ){
			//WRITE LOCK
			pthread_rwlock_wrlock(&rwlock2);

			last->next = newpair;
			newpair->next = NULL;
			
			//WRITE UNLOCK
			pthread_rwlock_unlock(&rwlock2);
			
			return 0;
		/* We're in the middle of the list. */
		}else{
			//WRITE LOCK
			pthread_rwlock_wrlock(&rwlock2);
			
			newpair->next = next;
			last->next = newpair;
			
			//WRITE UNLOCK
			pthread_rwlock_unlock(&rwlock2);
			
			return 0;
		}
	}
	return 0;
}

//Retrieve a pair from a hash table
char *ht_get( hashtable_t *hashtable, uint32_t key ) {
	uint32_t bin = 0;
	entry_t *pair;

	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );
	
	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	printf("Position:%d", bin);
	
	//READ LOCK
	pthread_rwlock_rdlock(&rwlock);
	
	while( pair != NULL && key != pair->key ) {
		printf(" ------------------->[%d,%s]\n", pair->key, pair->value);
		pair = pair->next;
	}

	//READ UNLOCK
	pthread_rwlock_unlock(&rwlock);

	/* Did we actually find anything? */
	if( pair == NULL || key != pair->key ) {
		return NULL;

	}else{
		printf(" ->[%d,%s]\n", pair->key, pair->value);
		return pair->value;
	}
	
}

//print the hash
void ht_print( hashtable_t *hashtable) {
	uint32_t bin = 0;
	entry_t *pair;

	//bin = ht_hash( hashtable, key );
	
	
	for(bin = 0; bin < hashtable->size; bin++){
		
		/* Step through the bin, looking for our value. */
		pair = hashtable->table[ bin ];
		if(pair != NULL){
			printf("\n");
			printf("Position:%u", bin);
		}
		while( pair != NULL ) {
			
			printf(" ->[%d,%s]", pair->key, pair->value);
			pair = pair->next;
		}
	}
	printf("\n");
}

//save the hash to a file
void ht_save( hashtable_t *hashtable, FILE *f ) {
	uint32_t bin = 0;
	entry_t *pair;

	for(bin = 0; bin < hashtable->size; bin++){
		
		/* Step through the bin, looking for our value. */
		pair = hashtable->table[ bin ];
		
		//READ LOCK
		pthread_rwlock_rdlock(&rwlock);
		
		while( pair != NULL ) {	
			fprintf(f, "%s %u %s\n", "rd", pair->key, pair->value);
			pair = pair->next;
		}
		
		//READ UNLOCK
		pthread_rwlock_unlock(&rwlock);
		
	}
}

//read the hash from a file
void ht_read( hashtable_t *hashtable, FILE *f) {
	uint32_t key = 0;
	char *value;
	char buff[256];
	char instr[4];
	
	while(fscanf(f, "%s" "%u" "%s",instr, &key, buff) != EOF){
		if(strcmp("rd", instr)){
			value = buff;
			ht_set(hashtable, key, value, 0);
		}else if(strcmp("wr0", instr)){
			value = buff;
			ht_set(hashtable, key, value, 0);
		}else if(strcmp("wr1", instr)){
			value = buff;
			ht_set(hashtable, key, value, 1);
		}else if(strcmp("rm", instr)){
			ht_remove(hashtable, key);
		}
		
	}
}

//remove one pair from the hash
int ht_remove(hashtable_t *hashtable, uint32_t key){
	uint32_t bin = 0;
	int flag = -1;
	entry_t *next;
	entry_t *last;
	//entry_t *aux;
	//entry_t *pair;



	bin = ht_hash1( hashtable, key );

	next = hashtable->table[ bin ];	
	last = next;
//passar write lock para tudo 
	//READ LOCK
	pthread_rwlock_rdlock(&rwlock);
	
	while(key != next->key ) {
		last = next;
		next = next->next;
	}
	
	//READ UNLOCK
	pthread_rwlock_unlock(&rwlock);
	
	
	if(next->key == key){
		//WRITE LOCK
		pthread_rwlock_wrlock(&rwlock2);	
	
		//ITS THE ONLY ONE IN THAT ENTRY
		if(last == next && next->next == NULL){
			free(next->value);
			free(next->next);
			free(hashtable->table[bin]);
			hashtable->table[bin] = NULL;
			flag = 1;
			return flag;
		}
		
		last->next = next->next;
		free(next->value);
		free(next->next);
		free(next);

		flag = 1;
	
	}
		//WRITE UNLOCK
		pthread_rwlock_unlock(&rwlock2);	
	
/*
	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );	
	
	//search the position 
	pair = hashtable->table[ bin ];
	aux = pair;
	
	
	while( pair != NULL ) {
		//WRITE LOCK
		pthread_rwlock_wrlock(&rwlock2);
		
		if(pair->key == key){
			aux->next = pair->next;
			free(pair->value);
			free(pair);
			
			flag = 1;
			return flag;
		}
		
		//WRITE UNLOCK
		pthread_rwlock_unlock(&rwlock2);
		
		//READ LOCK
		pthread_rwlock_rdlock(&rwlock);
		
		aux = pair;
		pair = pair->next;
		
		//READ UNLOCK
		pthread_rwlock_unlock(&rwlock);
		
	}
*/
	
	return flag;
}
