#include <fxnop/type.h>

extern uint32_t ld_text, ld_text_size;
extern uint32_t ld_data, ld_data_load, ld_data_size;
extern uint32_t ld_bss, ld_bss_size;
extern uint32_t ld_size;

extern void fxnop_entry(void);

__attribute__((__section__(".entry"), __used__))
int start(int is_app, int opt_num) {
  // 0. force map the entirety of the kernel
  
  for (void *addr = (void *)(0x00300200); addr < (void *)(0x00300200) + (uint32_t)(&ld_size); addr += 1024) {
    volatile uint8_t temp = *((uint8_t *)(addr));
  }
  
  // 1. delete every entry not mapping to 0x003xxxxx
  
  for (int i = 0; i < 64; i++) {
    volatile uint32_t *data_1 = (uint32_t *)(0xF6000000 + (i << 8));
    volatile uint32_t *data_2 = (uint32_t *)(0xF7000000 + (i << 8));
    
    if (!(*data_1 & 0x00000100) || !(*data_2 & 0x00000100)) continue;
    
    uint32_t virt = *data_1 & 0xFFFFFC00;
    uint32_t phys = *data_2 & 0xFFFFFC00;
    
    if ((virt >> 20) != 0x0003) {
      *data_1 &= ~0x00000100;
      *data_2 &= ~0x00000100;
    }
  }
  
  // 2. for each entry mapping 0x003xxxxx, remap it with 0x000xxxxx
  
  for (int i = 0; i < 64; i++) {
    volatile uint32_t *data_1 = (uint32_t *)(0xF6000000 + (i << 8));
    volatile uint32_t *data_2 = (uint32_t *)(0xF7000000 + (i << 8));
    
    if (!(*data_1 & 0x00000100) || !(*data_2 & 0x00000100)) continue;
    
    uint32_t virt = *data_1 & 0xFFFFFC00;
    uint32_t phys = *data_2 & 0xFFFFFC00;
    
    if ((virt >> 20) == 0x0003) {
      for (int j = 0; j < 64; j++) {
        volatile uint32_t *new_data_1 = (uint32_t *)(0xF6000000 + (j << 8));
        volatile uint32_t *new_data_2 = (uint32_t *)(0xF7000000 + (j << 8));
        
        if (*new_data_1 & 0x00000100 || *new_data_2 & 0x00000100) continue;
        
        *new_data_1 = (virt & 0x000FFC00) | 0x00000100;
        *new_data_2 = phys | 0x00000110;
        
        break;
      }
    }
  }
  
  // 3. update all cache shit
  
  *((volatile uint32_t *)(0xFF000078)) = 0x00000000;
  
  // 4. fill up the .data and .bss sections with, uhm, data
  
  uint8_t *data = (uint8_t *)(&ld_data);
  uint8_t *data_load = (uint8_t *)(&ld_data_load);
  
  for (size_t i = 0; i < (size_t)(&ld_data_size); i++) {
    data[i] = data_load[i];
  }
  
  uint8_t *bss = (uint8_t *)(&ld_bss);
  
  for (size_t i = 0; i < (size_t)(&ld_bss_size); i++) {
    bss[i] = 0x00;
  }
  
  // 5. jump to kernel entry, but at new relocated address
  
  fxnop_entry();
}
