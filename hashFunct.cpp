#include "hashFunct.h"


uint32_t crc32(const void* key, uint32_t bytes, const uint32_t initval)
{
    const uint32_t* key32 = (const uint32_t*)key;
    uint32_t R = initval;
    for (uint32_t i = 0; i < bytes >> 2; ++i)
    {
        R ^= key32[i];
        R = g_tbl[0 * 256 + (uint8_t)(R >> 24)] ^
            g_tbl[1 * 256 + (uint8_t)(R >> 16)] ^
            g_tbl[2 * 256 + (uint8_t)(R >>  8)] ^
            g_tbl[3 * 256 + (uint8_t)(R >>  0)];
    }
    return R;
}

uint32_t crc32_hardware(const void* key, uint32_t bytes, const uint32_t initval){
  uint32_t tot=initval;
  uint32_t *data32;
  data32 = (uint32_t *) key;

  uint32_t block=sizeof(*data32);

    for (int i = 0; i < bytes / block; i++)
      tot = _mm_crc32_u32 (tot, *data32++);
    
  return tot;
}




uint32_t hash(int table_size, packetID* key, const uint32_t initval){
  uint32_t h=0;
  h=crc32_hardware((void*)key, (uint32_t)sizeof(key), initval)%table_size;
	return h;
}