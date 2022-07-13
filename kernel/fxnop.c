#include <fxnop/term.h>
#include <fxnop/type.h>
#include <devs/video.h>
#include <string.h>

extern uint32_t ld_text, ld_text_size;
extern uint32_t ld_data, ld_data_load, ld_data_size;
extern uint32_t ld_bss, ld_bss_size;

void fxnop_entry(void) {
  void (*reset)(void) = (void *)(0xA0000000);
  reset();
  
  // 6. delete entries mapping 0x003xxxxx
  
  for (int i = 0; i < 64; i++) {
    uint32_t *data_1 = (uint32_t *)(0xF6000000 + (i << 8));
    uint32_t *data_2 = (uint32_t *)(0xF7000000 + (i << 8));
    
    if (!(*data_1 & 0x00000100) || !(*data_2 & 0x00000100)) continue;
    
    uint32_t virt = *data_1 & 0xFFFFFC00;
    uint32_t phys = *data_2 & 0xFFFFFC00;
    
    if (!virt && !phys) {
      *data_1 &= ~0x00000100;
      *data_2 &= ~0x00000100;
    }
  }
  
  video_init();
  term_init();
  
  strcpy(video_title, "debug terminal");
  
  for (int i = 0; i < 64; i++) {
    uint32_t data_1 = *((uint32_t *)(0xF6000000 + (i << 8)));
    uint32_t data_2 = *((uint32_t *)(0xF7000000 + (i << 8)));
    
    if (!(data_1 & 0x00000100) || !(data_2 & 0x00000100)) continue;
    
    uint32_t virt = data_1 & 0xFFFFFC00;
    uint32_t phys = data_2 & 0xFFFFFC00;
    
    if (virt >= 0x08000000) continue;
    
    uint32_t size_id = ((data_2 >> 6) & 2) | ((data_2 >> 4) & 1);
    
    const char *sizes[] = {"1K", "4K", "64K", "1M"};
    
    term_printf("%02X:%08X>%08X,%s\n", i, virt, phys, sizes[size_id]);
  }
  
  for (;;) {
    if (video_update()) video_send();
  }
  
  /*
  void (*reset)(void) = (void *)(0xA0000000);
  reset();
  */
}
