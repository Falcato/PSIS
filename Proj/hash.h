#ifndef hashHeader
#define hashHeader


struct entry_s {
	int key;
	char *value;
	struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s {
	int size;
	struct entry_s **table;	
};

typedef struct hashtable_s hashtable_t;


hashtable_t *ht_create( int size );
int ht_hash( hashtable_t *hashtable, char *key );
int ht_hash1(hashtable_t *hashtable, int a);
entry_t *ht_newpair( int key, char *value );
int ht_set( hashtable_t *hashtable, int key, char *value, int flag );
char *ht_get( hashtable_t *hashtable, int key );
char *ht_print( hashtable_t *hashtable, int key );
int ht_remove(hashtable_t *hashtable, int key);

#endif
