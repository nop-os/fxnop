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

void *memmove(void *dest, const void *src, size_t size) {
  if (dest < src) return memcpy(dest, src, size);
  
  while (size--) {
    ((uint8_t *)(dest))[size] = ((uint8_t *)(src))[size];
  }
  
  return dest;
}

size_t strlen(const char *str) {
  size_t length = 0;
  while (str[length]) length++;
  
  return length;
}

char *strcpy(char *dest, const char *src) {
  char *temp = dest;
  
  while (*src) {
    *(dest++) = *(src++);
  }
  
  *dest = '\0';
  return temp;
}
