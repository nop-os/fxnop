#include <fxnop/type.h>
#include <string.h>

void *memcpy(void *dest, const void *src, size_t size) {
  void *temp = dest;
  
  while (size--) {
    *((uint8_t *)(dest++)) = *((uint8_t *)(src++));
  }
  
  return temp;
}

void *memset(void *ptr, int val, size_t size) {
  void *temp = ptr;
  
  while (size--) {
    *((uint8_t *)(ptr++)) = val;
  }
  
  return temp;
}
