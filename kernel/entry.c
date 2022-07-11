#include <fxnop/type.h>
#include <devs/video.h>
#include <string.h>

extern uint32_t ld_data, ld_data_load, ld_data_size;
extern uint32_t ld_bss, ld_bss_size;

__attribute__((__section__(".entry"), __used__))
int start(int is_app, int opt_num) {
  memcpy(&ld_data, &ld_data_load, (size_t)(&ld_data_size));
  memset(&ld_bss, 0, (size_t)(&ld_bss_size));
  
  // TODO: relocate and take control of the entire memory
  
  video_init();
  video_rect(0, 0, 128, 64, video_white);
  
  for (;;) {
    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 128; j++) {
        uint32_t index = j + i * 128;
        
        index ^= index >> 3;
        index *= 137;
        index ^= index << 4;
        index *= 79;
        index ^= index >> 5;
        
        index &= (128 * 64) - 1;
        
        int x = index & 127;
        int y = index >> 7;
        
        video_plot(x, y, video_invert);
        video_send();
      }
    }
  }
  
  /*
  void (*reset)(void) = (void *)(0xA0000000);
  reset();
  */
}
