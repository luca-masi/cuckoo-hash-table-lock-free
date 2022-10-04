#ifndef UTILITY__H
#define UTILITY_H

#include "structures.h"
#include <cstdlib>
#include <vector>
#define MALLOC(type, n) (type *)malloc((n) * sizeof(type))
#define CALLOC(type, n) (type *)calloc((n), sizeof(type))



int key_cmp(packetID* a, packetID* b);

int key_empty (packetID* a);

int string_to_int(const char* c);

std::vector <int> displacemnt_thread(int numEntry, int numThread);

#endif
