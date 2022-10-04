#include "utility.h"

int key_cmp(packetID* a, packetID* b)
{
	if(a->src_addr== b->src_addr && a->src_port== b->src_port && a->dst_addr==b->dst_addr && a->dst_port==b->dst_port)
    return 0;
	else 
		return -1;
}

int key_empty (packetID* a){
  
  if(a->src_addr!=0 && a->src_port!=0 &&  a->dst_addr!=0 && a->dst_port!=0)
		return -1;
  else 
    return 0;

}

int string_to_int(const char* c){
    int len = 0;
    if (1 == sscanf(c, "%i", &len)){
        return len;
    }
    return -1;
}

std:: vector <int> displacemnt_thread(int numEntry, int numThread){
	std::vector <int> index;
	
	if(numEntry%numThread==0){
        for(int i=0; i<numThread; i++)
            index.push_back(numEntry / numThread);
    }
    else{
        for(int i=0; i<numThread-1; i++)
            index.push_back(numEntry / numThread);
        index.push_back(numEntry-(index[0]*(numThread-1)));
    }
	return index;
}



uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}


uint32_t getblock32 ( const uint32_t * p, int i )
{
  return p[i];
}

uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}
