#include "chkwrt_read.h"
#include "utility.h"

#include <bits/stdc++.h>



#define maxmove 200
#define slot 4
#define numtab 2
#define victims 1


void test1(cuckooTable *T, packet_main* pack, int numEntry, int numQuery, int numpack, const int nummthread_wrtiter, const int numthread_reader);

void test2(cuckooTable *T, packet_main* pack, int numEntry, int numQuery, int numpack, const int nummthread_wrtiter, const int numthread_reader);

void test3(cuckooTable *T, packet_main* pack, int numEntry, int numQuery, int numpack, const int nummthread_wrtiter, const int numthread_reader);

void test4(cuckooTable *T, packet_main* pack, int numEntry, int numQuery, int numpack, const int nummthread_wrtiter, const int numthread_reader);

void test5(cuckooTable *T, packet_main* pack, int numEntry, int numQuery, int numpack, const int nummthread_wrtiter, const int numthread_reader);


int main(int argc, char *argv[])
{
	const int sizetable = string_to_int(argv[1]);
	const int pck_gen = string_to_int(argv[2]);
    const int n_entry = string_to_int(argv[3]);
    const int numberQuery = string_to_int(argv[4]);
    const int numthread_reader = string_to_int(argv[5]);
	const int numthread_writer = string_to_int(argv[6]);
	const int test = string_to_int(argv[7]);


	srand(time(NULL));
	
	//generate packet
	packet_main* pack= CALLOC(packet_main, pck_gen);
	int num_pack=generate_packet(pack, pck_gen);
	printf("generazione pacchetti %d/%d\n", num_pack, pck_gen);
		
	//create cuckoo
	cuckooTable *T= CALLOC (cuckooTable, 1);
	init_ckhTable(T, sizetable, maxmove, slot, numtab, victims);
	T->T1=ckh_alloc_table(T->sizeSingleTable, slot);
	T->T2=ckh_alloc_table(T->sizeSingleTable, slot);

	switch(test){
		case 1:
			test1(T, pack, n_entry, numberQuery, pck_gen, numthread_writer, numthread_reader);	//insert
			break;
		case 2:
			test2(T, pack, n_entry, numberQuery,pck_gen, numthread_writer, numthread_reader);		//insert in %
			break;
		case 3:
			test3(T, pack, n_entry, numberQuery,pck_gen, numthread_writer, numthread_reader);		//lookup
			break;
		case 4:
			test4(T, pack, n_entry, numberQuery,pck_gen, numthread_writer, numthread_reader);		//lookup + insert
			break;
		case 5:
			test5(T, pack, n_entry, numberQuery,pck_gen, numthread_writer, numthread_reader);		//lookup + delete and insert
			break;
	}
	
	//dealloc
	ckh_dealloc(T);
	free(pack);

return 0;
}


void test1(cuckooTable *T, packet_main* pack,  int numEntry, int numQuery, int numpack, const int numthread_writer, const int numthread_reader){
	printf("test insert\n");
	
	//write in cuckoo
	int pck_insert=0;
	std:: thread writeThread(write_all_incuckoo, pack, T, numEntry, numpack, &pck_insert);
	//join
	writeThread.join();
}

void test2(cuckooTable *T, packet_main* pack,  int numEntry, int numQuery, int numpack, const int numthread_writer, const int numthread_reader){
	printf("test insert a pieno carico\n");
	
	//write in cuckoo
	std:: vector <int> numInsrt=displacemnt_thread(numEntry, 4);
	int index=0;
	for(int k=0; k<4; k++){
		int pck_insert=0;
		if(k==0)
			index=k*numInsrt[k];
		else	
			index=k*numInsrt[k-1];
		std:: thread writeThread(write_all_incuckoo, &pack[index], T, numInsrt[k], numpack, &pck_insert);
		//join
		writeThread.join();
	}
}
	
void test3(cuckooTable *T, packet_main* pack,  int numEntry, int numQuery, int numpack, const int numthread_writer, const int numthread_reader){
	printf("test lettura\n");
	
	//write in cuckoo
	int pck_insert=0;
	std:: thread writeThread(write_all_incuckoo, pack, T, numEntry, numpack, &pck_insert);
	//join
	writeThread.join();	
	
	//read to cuckoo
	int pckfind=0;
	std:: vector <int> numQueryxThread=displacemnt_thread(numQuery, numthread_reader);
	std::vector <std::thread> list_reader;
	
	for(int i=0; i<numthread_reader; i++){
		std:: thread readThread(read_all_tocuckoo, pack, T, numpack, numQueryxThread[i], &pckfind);
		list_reader.push_back(std::move(readThread));
	}
	//join
	for(int i=0; i<numthread_reader; i++)
			if(list_reader[i].joinable())
				list_reader[i].join();
	
}

void test4(cuckooTable *T, packet_main* pack,  int numEntry, int numQuery, int numpack, const int numthread_writer, const int numthread_reader){
	printf("test lettura con insert\n");
	
	//write in cuckoo 50%
	int pck_insert=0;
	std:: thread writeThread(write_all_incuckoo, pack, T, numEntry/2, numpack, &pck_insert);
	//join
	writeThread.join();

	//read to cuckoo
	int pckfind=0;
	std:: vector <int> numQueryxThread=displacemnt_thread(numQuery, numthread_reader);
	std::vector <std::thread> list_reader;
	for(int i=0; i<numthread_reader; i++){
		std:: thread readThread(read_all_tocuckoo, pack, T, numpack, numQueryxThread[i], &pckfind);
		list_reader.push_back(std::move(readThread));
	}

	//write in cuckoo 90%
	pck_insert=0;	
	std:: thread writeThread2(write_all_incuckoo, pack, T, numQuery, numpack, &pck_insert);
	

	//join
	for(int i=0; i<numthread_reader; i++)
			if(list_reader[i].joinable())
				list_reader[i].join();
	writeThread2.join();
}

void test5(cuckooTable *T, packet_main* pack,  int numEntry, int numQuery, int numpack, const int numthread_writer, const int numthread_reader){
	printf("test lettura con insert e delete\n");
	
	//write in cuckoo
	int pck_insert=0;
	std:: thread writeThread(write_all_incuckoo, pack, T, numEntry/2, numpack, &pck_insert);
	//join
	writeThread.join();

	//read to cuckoo
	int pckfind=0;
	std:: vector <int> numQueryxThread=displacemnt_thread(numQuery, numthread_reader);
	std::vector <std::thread> list_reader;
	for(int i=0; i<numthread_reader; i++){
		std:: thread readThread(read_all_tocuckoo, pack, T, numpack, numQueryxThread[i], &pckfind);
		list_reader.push_back(std::move(readThread));
	}

	//delete and insert
	int pckdelete=0;
	pck_insert=0;
	std:: thread deleteThread(delete_all_incuckoo, pack, T, numpack, numQuery/2, &pckdelete);
	std:: thread writeThread2(write_all_incuckoo, pack, T, numQuery/3, numpack, &pck_insert);

	//join
	for(int i=0; i<numthread_reader; i++)
		if(list_reader[i].joinable())
			list_reader[i].join();
	deleteThread.join();
	writeThread2.join();
}


