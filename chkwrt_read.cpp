#include "chkwrt_read.h"


int generate_packet(packet_main *pack, int numEntry)
{
	int num_packgnrt=0;

	for(int i=0; i<numEntry; i++){
		pack[i].ckh=0;
		pack[i].id=i+1;
		pack[i].p.src_addr=rand()%66+pack[i].id*100+1;
		pack[i].p.dst_addr=rand()%66+pack[i].id*100+1;
		pack[i].p.src_port=rand()%66+pack[i].id*100+1;
		pack[i].p.src_port=rand()%66+pack[i].id*100+1;
		num_packgnrt++;
	}


	return num_packgnrt;
	
};

void write_in_cuckoo(packet_main* pack, int index, cuckooTable *T)
{
	int insrtFaild=0;

	insrtFaild=ckh_atomic_insert(T, &pack[index].p, pack[index].id);
	if(insrtFaild==0){
		//printf("inserito pacchetto %d\n", pack[index].id);
		pack[index].ckh=1;
	}
	else{
		//printf("pacchetto %d NON inserito\n", pack[index].id);
		// __atomic_add_fetch(count, 1, 0);
	}

};


void read_to_cuckoo(packet_main* pack, int index, cuckooTable *T)
{ 
	indexTable val=ckh_multiRead_nblck(T, &pack[index].p);
	int j=0;
	if(val.value!=-1 && pack[index].ckh==1 && pack[index].id==val.value){
		pack[index].ckh=0;
		//printf(pacchetto trovato"\n);
		// __atomic_add_fetch(count, 1, 0);
	}

};


void delete_in_cuckoo(packetID* key, cuckooTable *T){
	int succes=ckh_delete(T, key);
	//if(succes==0)
		//printf("pacchetto eliminato\n");
}

void update_in_cuckoo(packetID* key, cuckooTable *T, int value){
	int succes=ckh_update(T, key, value);
//	if(succes==0)
//		printf("pacchetto aggiornato\n");
}



void write_all_incuckoo(packet_main* pack, cuckooTable *T, int numEntry, int numPack, int *packInsert)
{
	printf("inserimento in cuckoo...\n");
	int insrtFaild=0;
	int j=0;
	testing writeT;
	writeT.startT=time(NULL);

	for(int i=0; i< numEntry; i++){
		insrtFaild=ckh_atomic_insert(T, &pack[j].p, pack[j].id);
		if(insrtFaild==0){
			//pack[i].ckh=1;
			__atomic_add_fetch(packInsert, 1, 0);
		}
		else {
			//printf("paccheto %d, NON INSERITO\n", pack[i].p.dst_addr);
		}
		if(j<numPack)
			j++;
		else
			j=0;
			
	}

	writeT.endT=time(NULL);
	writeT.tTime=difftime(writeT.endT, writeT.startT);
	printf("SCRITTURA: %dkey/sec\n", *packInsert/(int)writeT.tTime);
	printf("load factor: %d, entry inserite/aggiornate: %d, entry presenti: %d\n", T->num_entry*100/T->TOTentry, *packInsert, T->num_entry);


};


void read_all_tocuckoo(packet_main* pack, cuckooTable *T, int numPack, int numberQuery, int *packRead)
{
	printf("lettura...\n");
	indexTable val={.value=0};
	int j=0;
	testing readT;
	readT.startT=time(NULL);

	for(int k=0; k<numberQuery; k++){
		val=ckh_multiRead_nblck(T, &pack[j].p);
		if(val.value!=-1 && val.value==pack[j].id){
			__atomic_add_fetch(packRead, 1, 0);
		}
		else{
			//printf("paccheto %d NON TROVATO\n", pack[j].p.dst_addr);
		}
		if(j<numPack)
			j++;
		else
			j=0;

	}
	readT.endT=time(NULL);
	readT.tTime=difftime(readT.endT, readT.startT);
	printf("LETTURA: %dkey/sec\n", numberQuery/(int)readT.tTime);

};

void delete_all_incuckoo(packet_main* pack, cuckooTable *T, int numPack, int numberQuery, int * packDel){
	printf("delete to cuckoo...\n");
	int delFaild=0;
	testing delT;
	delT.startT=time(NULL);
	int j=0;
	for(int i=0; i< numberQuery; i++){
		delFaild=ckh_delete(T, &pack[rand()%numPack].p);
		if(delFaild==0){
			//pack[i].ckh=1;
			__atomic_add_fetch(packDel, 1, 0);
		}
		else {
			//printf("paccheto %d, NON INSERITO\n", pack[i].p.dst_addr);
		}	
	}

	delT.endT=time(NULL);
	delT.tTime=difftime(delT.endT, delT.startT);
	printf("DELETE: %dkey/sec\n", *packDel/(int)delT.tTime);
	printf("load factor: %d, entry inserite: %d, tot entry delete: %d\n", T->num_entry*100/T->TOTentry, *packDel, T->num_entry);
}

void update_all_incuckoo(packet_main* pack, cuckooTable *T, int numPack, int numberQuery, int * packUpdt){
	printf("update in cuckoo...\n");
	int uptFaild=0;
	testing updtT;
	updtT.startT=time(NULL);
	int j=0;
	for(int i=0; i< numberQuery; i++){
		uptFaild=ckh_update(T, &pack[rand()%numPack].p, rand()%numPack+numberQuery);
		if(uptFaild==0){
			//pack[i].ckh=1;
			__atomic_add_fetch(packUpdt, 1, 0);
		}
		else {
			//printf("paccheto %d, NON INSERITO\n", pack[i].p.dst_addr);
		}
	}

	updtT.endT=time(NULL);
	updtT.tTime=difftime(updtT.endT, updtT.startT);
	printf("DELETE: %dkey/sec\n", *packUpdt/(int)updtT.tTime);
	printf("load factor: %d, entry inserite: %d, tot entry update: %d\n", T->num_entry*100/T->TOTentry, *packUpdt, T->num_entry);
}



void print_cuckoo(cuckooTable *T)
{

	for(int i=0; i< T->sizeSingleTable; i++){
			for(int j=0; j< T->slot; j++){
				// printf("%d\n", T->T1[i][j].key.src_addr);
				// printf("%d\n", T->T2[i][j].key.src_addr);
				// 	printf("%d\n", T->T1[i][j].key.src_port);
				// printf("%d\n", T->T2[i][j].key.src_port);
				// 	printf("%d\n", T->T1[i][j].key.dst_addr);
				// printf("%d\n", T->T2[i][j].key.dst_addr);
				// 	printf("%d\n", T->T1[i][j].key.dst_port);
				// printf("%d\n", T->T2[i][j].key.dst_port);
				printf("%d\n", T->T1[i][j].value);
				printf("%d\n", T->T2[i][j].value);
			}
	}
};

void countentry(cuckooTable *T, packet_main *p, int numEntry)
{
int count=0;
for(int k=0; k<numEntry; k++)
	for(int i=0; i< T->sizeSingleTable; i++)
			for(int j=0; j< T->slot; j++)
				if(T->T1[i][j].value==p[k].id || T->T2[i][j].value==p[k].id || T->victim.value==p[k].id){
					count++;
				}

printf("count:%d\n", count);
};

void print_pack(packet_main *p, int numEntry)
{

	for(int i=0; i< numEntry; i++){
		printf("%d\n", p[i].id);
	}

};




