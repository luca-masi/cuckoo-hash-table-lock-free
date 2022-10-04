#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    uint16_t 	src_port;
    uint16_t    dst_port;
    uint32_t 	src_addr;
    uint32_t 	dst_addr;
}packetID;

typedef struct{
    bool SYN;
    bool ACK;
    bool PSH;
    bool FIN;
    uint32_t nSEQ;
    uint32_t ACKn;
} connectionState;

typedef struct{
    int i;
    int j;
    int table;
    int value;
} indexTable;

typedef struct {			
	packetID key;		
	int value;		
} entryTable;

typedef struct{
    packetID p;
    int id;
    int ckh;
}packet_main;

typedef struct{
    time_t startT;
    time_t endT;
    double tTime;
}testing;

#endif