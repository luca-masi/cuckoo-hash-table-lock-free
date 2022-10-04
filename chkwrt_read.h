#ifndef WRT_READ
#define WRT_READ
#include "cuckooHash.h"
#include <unistd.h>
#include <thread>

int generate_packet(packet_main *pack, int numEntry);

void write_in_cuckoo(packet_main* pack, int index, cuckooTable *T);

void read_to_cuckoo(packet_main* pack, int index, cuckooTable *T);

void delete_in_cuckoo(packetID* key, cuckooTable *T);

void update_in_cuckoo(packetID* key, cuckooTable *T, int value);

void write_all_incuckoo(packet_main* pack, cuckooTable *T, int numEntry, int * packInsert);

void read_all_tocuckoo(packet_main* pack, cuckooTable *T, int numEntry, int numberQuery, int * packRead);

void update_all_incuckoo(packet_main* pack, cuckooTable *T, int numPack, int numberQuery, int * packUpdt);

void delete_all_incuckoo(packet_main* pack, cuckooTable *T, int numPack, int numberQuery, int * packDel);

void print_cuckoo(cuckooTable *T);

void print_pack(packet_main *p, int numEntry);

void countentry(cuckooTable *T, packet_main *p, int numEntry);



#endif
