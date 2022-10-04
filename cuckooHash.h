#ifndef CUCKOO_HASH
#define CUCKOO_HASH

#include "utility.h"
#include "hashFunct.h"

typedef struct{
	
	uint32_t SIZE;	/* Size of Hash table*/ 
	int num_entry;		/*  Current size */
	uint32_t sizeSingleTable;	/*  Size of tables */
	int max_move;		/*  Max iterations in insertion */
	entryTable **T1;		/*  Pointer to hash table 1 */
	entryTable **T2;		/*  Pointer to hash table 2 */
	uint32_t hash_T1;			/*  Hash function 1 */
	uint32_t hash_T2;			/*  Hash function 2 */
    int slot;
	int TOTentry;
	entryTable victim;
	int num_victims;
	
}cuckooTable;

entryTable** ckh_alloc_table(uint32_t sizeTable, int numslot);

void init_ckhTable(cuckooTable* Table, const int table_size, const int max_movement, const int numbucket, const int numtab, const int victims);

int ckh_dealloc(cuckooTable* D);

int ckh_insert(cuckooTable *D, packetID *key, int value);

int ckh_atomic_insert(cuckooTable *D, packetID *key, int value);

indexTable ckh_getVal(cuckooTable *D, packetID *key);

int ckh_update(cuckooTable *D, packetID *key, int value);

void ckh_updateVal(entryTable* T, int value);

indexTable ckh_multiRead_nblck(cuckooTable *D, packetID *key);

int ckh_delete(cuckooTable *D, packetID *key);
#endif

