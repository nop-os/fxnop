#include <fxnop/type.h>
#include <devs/video.h>

uint32_t *video_ram = NULL;

volatile uint8_t *video_select = (void *)(0xB4000000);
volatile uint8_t *video_data = (void *)(0xB4010000);

uint8_t video_status(void) {
  return *video_select;
}

void video_set(uint8_t offset, uint8_t value) {
  *video_select = offset;
  *video_data = value;
}

void video_init(void) {
  video_ram = (void *)(0x88040000);
}

void video_send(void) {
  uint8_t *ptr = (void *)(video_ram);
  // TODO: uhm this feels slow
  
  for (int i = 0; i < 64; i++) {
    video_set(0x04, i | 0xC0);
    video_set(0x01, 0x01);
    video_set(0x04, 0x00);
    
    *video_select = 0x07;
    
    for (int j = 0; j < 16; j++) {
      *video_data = *(ptr++);
    }
  }
}

void video_plot(int x, int y, int color) {
  if ((unsigned int)(x) >= 128 || (unsigned int)(y) >= 64) return;
  
  if (color == video_black) video_ram[(x >> 5) + (y << 2)] |= (1 << (x & 31));
  else if (color == video_white) video_ram[(x >> 5) + (y << 2)] &= ~(1 << (x & 31));
  else if (color == video_invert) video_ram[(x >> 5) + (y << 2)] ^= (1 << (x & 31));
}

void video_rect(int x, int y, int width, int height, int color) {
  // TODO: this might be optimizable...
  
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      video_plot(x + j, y + i, color);
    }
  }
}
