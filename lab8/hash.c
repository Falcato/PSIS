#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "hash.h"

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


int ht_hash1(hashtable_t *hashtable, int a){
    /*a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);*/
    return a % hashtable->size;
}


/* Create a key-value pair. */
entry_t *ht_newpair( int key, char *value ) {
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

/* Insert a key-value pair into a hash table. */
int ht_set( hashtable_t *hashtable, int key, char *value, int flag ) {
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );

	next = hashtable->table[ bin ];

	while( next != NULL && key != next->key ) {
		last = next;
		next = next->next;
	}

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && key == next->key ) {
		
		if(flag == 1){
			free( next->value );
			next->value = strdup( value );
			return 2;
		}else{
			return -2;
		}

	/* Nope, could't find it.  Time to grow a pair. */
	}else{
		newpair = ht_newpair( key, value );

		/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;

		/* We're at the end of the linked list in this bin. */
		}else if( next == NULL ){
			last->next = newpair;

		/* We're in the middle of the list. */
		}else{
			newpair->next = next;
			last->next = newpair;
		}
	}
	return 0;
}

/* Retrieve a key-value pair from a hash table. */
char *ht_get( hashtable_t *hashtable, int key ) {
	int bin = 0;
	entry_t *pair;

	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );
	
	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	printf("Position:%d", bin);
	while( pair != NULL && key != pair->key ) {
		pair = pair->next;
	}

	/* Did we actually find anything? */
	if( pair == NULL || key != pair->key ) {
		return NULL;

	}else{
		printf(" ->[%d,%s]\n", pair->key, pair->value);
		return pair->value;
	}
	
}

//print the hash position
char *ht_print( hashtable_t *hashtable, int key ) {
	int bin = 0;
	entry_t *pair;

	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );
	
	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	printf("Position:%d", bin);
	while( pair != NULL ) {
		
		printf(" ->[%d,%s]", pair->key, pair->value);
		pair = pair->next;
	}

	/* Did we actually find anything? */
	if( pair == NULL || key != pair->key ) {
		printf("\n");
		return NULL;

	} else {
		printf("\n");
		return pair->value;
	}
	printf("\n");
}

void ht_remove(hashtable_t *hashtable, int key){
	int bin = 0;
	entry_t *pair;
	entry_t *aux;

	//bin = ht_hash( hashtable, key );
	bin = ht_hash1( hashtable, key );	
	
	//search the position 
	pair = hashtable->table[ bin ];
	aux = pair;
	while( pair != NULL ) {
		if(pair->key == key){
			aux->next = pair->next;
			free(pair->value);
			free(pair);
		}
		aux = pair;
		pair = pair->next;
	}	
}
