CC=g++
CEXTERNALFLAGS= -latomic
CFLAGS=-std=c++0x -mcx16 -pthread -msse4.2 -latomic
COUT=-o
CDEBUGFLAG=-g

main: main.cpp
	$(CC) $(CFLAGS) $(COUT) main.out main.cpp utility.cpp hashFunct.cpp cuckooHash.cpp chkwrt_read.cpp $(CEXTERNALFLAGS)

main_debug: main.c
	$(CC) $(CDEBUGFLAG) $(CFLAGS) $(COUT) out/debug/main.db main.c

.PHONY: clean
clean:
	rm -rf *.out