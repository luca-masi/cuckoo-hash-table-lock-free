#include "cuckooHash.h"
#include <atomic>


#define SEEDt1 0xFFFFFFFF
#define SEEDt2 0x46AF6449


entryTable** ckh_alloc_table(uint32_t sizeTable, int numslot)
{
	entryTable nop={.key={.src_port=0, .dst_port=0, .src_addr=0, .dst_addr=0}, .value=0};
	
	entryTable** T =(entryTable**) CALLOC(entryTable, sizeTable);
    for(int i = 0; i < sizeTable; i++){
        T[i] = (entryTable*) CALLOC(entryTable, numslot);
		for(int j=0; j<numslot; j++){
			T[i][j]=nop;
		}
    }

	return T;

}

void init_ckhTable(cuckooTable *Table, const int table_size, const int max_movement, const int slot, const int numtab, const int victims)
{
	uint32_t sizeTotTable=table_size/slot;
	entryTable nop={.key={.src_port=0, .dst_port=0, .src_addr=0, .dst_addr=0}, .value=0};
	
	Table->SIZE=(uint32_t)sizeTotTable;
	Table->num_entry= 0;
	Table->sizeSingleTable=(uint32_t) sizeTotTable/numtab;
	Table->max_move=max_movement;   
	Table->hash_T1=SEEDt1;
	Table->hash_T2=SEEDt2;
	Table->T1=NULL;
	Table->T2=NULL;
	Table->slot=slot;
	Table->TOTentry=table_size;
	Table->victim=nop;
	Table->num_victims=victims;

}

int ckh_dealloc(cuckooTable* D){

	for(int i=0; i<D->sizeSingleTable; i++){
		free(D->T1[i]);
		free(D->T2[i]);
	}

	free(D->T1);
	free(D->T2);

	return 0;
}

int ckh_atomic_insert(cuckooTable *D, packetID *key, int value)
{
	uint32_t h;
	int countMove=0;

	entryTable nop={.key={.src_port=0, .dst_port=0, .src_addr=0, .dst_addr=0}, .value=0};
	entryTable entry={.key={.src_port=key->src_port, .dst_port=key->dst_port, .src_addr=key->src_addr, .dst_addr=key->dst_addr}, .value=value};
	
	if(ckh_update(D, key, value)==0){
		//printf("chiave identica\n");
		return 0;
	}

	if(__atomic_compare_exchange (&D->victim, &nop, &entry, 0, 0, 0)==true){

		__atomic_add_fetch(&D->num_entry, 1, 0);
		//printf("chiave inserita\n");


		while(countMove < D->max_move)
		{
			h=hash(D->sizeSingleTable, &D->victim.key, D->hash_T1);
			for(int i=0; i<D->slot; i++)
			{
				if(__atomic_compare_exchange (&D->T1[h][i], &nop, &D->victim, 0, 0, 0)==true){
					D->victim=nop;
					return 0;
				}
			}		
			
			int j=rand()%D->slot;

		
			__atomic_exchange(&D->T1[h][j], &D->victim,  &D->victim, 0);

			h=hash(D->sizeSingleTable, &D->victim.key, D->hash_T2);
			for(int i=0; i<D->slot; i++)
			{
				if(__atomic_compare_exchange (&D->T2[h][i], &nop, &D->victim, 0, 0, 0)==true){
					D->victim=nop;
					return 0;
				}
			}

			__atomic_exchange(&D->T2[h][j], &D->victim,  &D->victim, 0);

			countMove++;
		}

		return 0;
	

	}

    
	return -1;

}


indexTable ckh_getVal(cuckooTable *D, packetID *key)
{
	uint32_t h;
	indexTable index={.i=-1, .j=-1, .table=-1, .value=-1};
	packetID entry={.src_port=key->src_port, .dst_port=key->dst_port, .src_addr=key->src_addr, .dst_addr=key->dst_addr};

	h=hash(D->sizeSingleTable, key, D->hash_T1);
    for(int i=0; i<D->slot; i++)
	{    	
		if (key_cmp(&D->T1[h][i].key, key) == 0 && key_empty(&D->T1[h][i].key)==0){
			index.i=h;
			index.j=i;
			index.table=0;
			index.value= D->T1[h][i].value;
			return index;
		}
    }

	h=hash(D->sizeSingleTable, key, D->hash_T2);
    for(int i=0; i<D->slot; i++)
	{
	    if (key_cmp(&D->T2[h][i].key, key) == 0  && key_empty(&D->T2[h][i].key)==0){
			index.i=h;
			index.j=i;
			index.table=1;
			index.value= D->T2[h][i].value;
			return index;
		}
    }

	if(key_cmp(&D->victim.key, key)==0  && key_empty(&D->victim.key)==0){
			index.table=2;
			index.value= D->victim.value;
			return index;
	}

	return index;
}



indexTable ckh_multiRead_nblck(cuckooTable *D, packetID *key)
{
	
	indexTable start={.i=0, .j=0, .table=0, .value=0};
	indexTable end={.i=1, .j=1, .table=1, .value=1};
	int j=0;


	while(start.i!= end.i && start.j!= end.j && start.table!= end.table && start.value!=end.value){
		start=ckh_getVal(D, key);
		end=ckh_getVal(D, key);
		
		//if(start.value!=end.value || start.i!= end.i)
		//	printf("val start: %d, val end: %d\n", start.value, end.value);
	}
	return start;
}



int ckh_delete(cuckooTable *D, packetID *key)
{
	bool compare=false;
	entryTable nop={.key={.src_port=0, .dst_port=0, .src_addr=0, .dst_addr=0}, .value=0};
	
	while(compare==false){
		indexTable val_index=ckh_multiRead_nblck(D, key);
		entryTable tmp={.key={.src_port=key->src_port, .dst_port=key->dst_port, .src_addr=key->src_addr, .dst_addr=key->dst_addr}, .value=val_index.value};
		if(val_index.table==0){
			compare=__atomic_compare_exchange (&D->T1[val_index.i][val_index.j], &tmp, &nop, 0, 0, 0);
		}
		else if(val_index.table==1){
			compare=__atomic_compare_exchange (&D->T2[val_index.i][val_index.j], &tmp, &nop, 0, 0, 0);
		}
		else if(val_index.table==2){
			compare=__atomic_compare_exchange (&D->victim, &tmp, &nop, 0, 0, 0);
		}
		else 
			return -1;

	}
	__atomic_sub_fetch(&D->num_entry, 1, 0);

	return 0;
}


int ckh_update(cuckooTable *D, packetID* key, int value){

	bool compare=false;
	entryTable newVal={.key={.src_port=key->src_port, .dst_port=key->dst_port, .src_addr=key->src_addr, .dst_addr=key->dst_addr}, .value=value};
	
	while(compare==false){

		indexTable val_index=ckh_multiRead_nblck(D, key);
		entryTable tmp={.key={.src_port=key->src_port, .dst_port=key->dst_port, .src_addr=key->src_addr, .dst_addr=key->dst_addr}, .value=val_index.value};
		
		if(val_index.table==0)
			compare=__atomic_compare_exchange (&D->T1[val_index.i][val_index.j], &tmp, &newVal, 0, 0, 0);
		else if(val_index.table==1)
			compare=__atomic_compare_exchange (&D->T2[val_index.i][val_index.j], &tmp, &newVal, 0, 0, 0);
		else if(val_index.table==2)
			compare=__atomic_compare_exchange (&D->victim, &tmp, &newVal, 0, 0, 0);
		else 
			return -1;

	} 

	return 0;
}



int ckh_insert(cuckooTable *D, packetID *key, int value)
{
	uint32_t h;
	entryTable tmp;
	entryTable x;
	
	int countMove=0;

	if(ckh_update(D, key, value)==0)
		return 0;

	if(key_empty(&D->victim.key)==0){
		
		D->num_entry++;

		x.key = *key;
		x.value=value;

		
		while(countMove < D->max_move)
		{
			h=hash(D->sizeSingleTable, &x.key, D->hash_T1);
			for(int i=0; i<D->slot; i++)
			{
				if (key_empty(&D->T1[h][i].key)==0){
					D->T1[h][i] = x;
					return 0;
				}

			}

			int j=rand()%D->slot;
			
			tmp=D->T1[h][j];
			D->T1[h][j]=x;
			x=tmp;


			h=hash(D->sizeSingleTable, &x.key, D->hash_T2);
			for(int i=0; i<D->slot; i++)
			{
				if (key_empty(&D->T2[h][i].key)==0){
					D->T2[h][i]= x;
					return 0;
				}
			}

			tmp=D->T2[h][j];
			D->T2[h][j]=x;
			x=tmp;

			countMove++;
		}


		D->victim=x;
		return 0;
	}

	return -1;

}


