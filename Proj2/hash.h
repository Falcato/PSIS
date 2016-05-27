#ifndef hashHeader
#define hashHeader

struct entry_s {
	uint32_t key;
	uint32_t size;
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
uint32_t ht_hash1(hashtable_t *hashtable, uint32_t a);
entry_t *ht_newpair( uint32_t key, char *value, uint32_t size );

int ht_set( hashtable_t *hashtable, uint32_t key, char *value, uint32_t size, int flag);

char *ht_get( hashtable_t *hashtable, uint32_t key );
uint32_t ht_get_length( hashtable_t *hashtable, uint32_t key );

void ht_print( hashtable_t *hashtable);

void ht_save( hashtable_t *hashtable, FILE *f);

void ht_read( hashtable_t *hashtable, FILE *f);

int ht_remove(hashtable_t *hashtable, uint32_t key);


#endif
