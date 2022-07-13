#include <fxnop/type.h>
#include <devs/video.h>
#include <string.h>

uint32_t *video_ram = NULL;
char video_title[32];

int video_dirty = 0;

volatile uint8_t *video_select = (void *)(0xB4000000);
volatile uint8_t *video_data = (void *)(0xB4010000);

const uint8_t video_font[] = {
  0x04, 0xaa, 0x79, 0x74, 0x24, 0xa0, 0x00, 0x01, 
  0x04, 0xaf, 0xc2, 0xa4, 0x42, 0x44, 0x00, 0x02, 
  0x00, 0x0a, 0x74, 0xe0, 0x42, 0xae, 0x2e, 0x04, 
  0x04, 0x0f, 0xe9, 0x50, 0x24, 0x04, 0x40, 0x48, 
  0x62, 0x6e, 0x9f, 0x7f, 0x66, 0x44, 0x00, 0x06, 
  0xb6, 0x93, 0x98, 0xc1, 0xfd, 0x00, 0x2e, 0x4b, 
  0xd2, 0x21, 0x77, 0xb2, 0x93, 0x02, 0x40, 0x20, 
  0x62, 0xfe, 0x1e, 0x62, 0x6e, 0x44, 0x2e, 0x42, 
  0x66, 0xe6, 0xef, 0xf6, 0x97, 0xfb, 0x8d, 0x96, 
  0xa9, 0xb9, 0x98, 0x88, 0x92, 0x1c, 0x8f, 0xd9, 
  0xdf, 0xd8, 0x9e, 0xeb, 0xf2, 0x9a, 0x89, 0xb9, 
  0x69, 0xe6, 0xef, 0x86, 0x97, 0x69, 0xf9, 0x96, 
  0xe6, 0xe7, 0x79, 0x99, 0x95, 0xf6, 0x86, 0x40, 
  0x99, 0x98, 0x29, 0x99, 0x65, 0x24, 0x42, 0xa0, 
  0xea, 0xe7, 0x29, 0xaf, 0x62, 0x44, 0x22, 0x00, 
  0x85, 0x9e, 0x26, 0x4d, 0x92, 0xf6, 0x16, 0x0f
};

uint8_t video_status(void) {
  return *video_select;
}

void video_set(uint8_t offset, uint8_t value) {
  *video_select = offset;
  *video_data = value;
}

void video_init(void) {
  video_ram = (void *)(0x88080000 - 1024);
  memset(video_ram, 0, 1024);
  
  video_dirty = 1;
}

int video_update(void) {
  if (!video_dirty) return 0;
  video_dirty = 0;
  
  for (int i = 0; i < 64; i++) {
    if (i < 8 || i == 63) {
      memset(video_ram + (i << 2), 0xFF, 16);
    } else {
      video_ram[0 + (i << 2)] |= 0x80000000;
      video_ram[3 + (i << 2)] |= 0x00000001;
    }
  }
  
  video_ram[0] = 0x7FFFFFFF;
  video_ram[3] = 0xFFFFFFFE;
  
  const char *str = video_title;
  int x = 2;
  
  while (*str) {
    uint8_t chr = *str - 32;
    if (chr >= 64) chr -= 32;
    
    int id_low = (chr >> 0) & 1;
    int id_mid = (chr >> 1) & 7;
    int id_high = (chr >> 4) & 3;
    
    for (int j = 0; j < 4; j++) {
      uint8_t mask = video_font[id_mid | ((j | (id_high << 2)) << 3)];
      
      for (int k = 0; k < 4; k++) {
        if (mask & (1 << (7 - (k | (id_low << 2))))) video_ram[((x + k) >> 5) + ((2 + j) << 2)] &= ~(1 << (31 - (((x + k) & 31))));
      }
    }
    
    x += 5;
    str++;
  }
  
  return 1;
}

void video_send(void) {
  uint8_t *ptr = (void *)(video_ram);
  
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
  if ((unsigned int)(x) >= 126 || (unsigned int)(y) >= 55) return;
  
  x += 1;
  y += 8;
  
  if (color == video_black) video_ram[(x >> 5) + (y << 2)] |= (1 << (31 - (x & 31)));
  else if (color == video_white) video_ram[(x >> 5) + (y << 2)] &= ~(1 << (31 - (x & 31)));
  else if (color == video_invert) video_ram[(x >> 5) + (y << 2)] ^= (1 << (31 - (x & 31)));
  
  video_dirty = 1;
}

void video_rect(int x, int y, int width, int height, int color) {
  // TODO: this might be optimizable...
  
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      video_plot(x + j, y + i, color);
    }
  }
}

void video_text(const char *str, int x, int y, int color) {
  // TODO: this might be optimizable too...
  
  while (*str) {
    uint8_t chr = *str - 32;
    if (chr >= 64) chr -= 32;
    
    int id_low = (chr >> 0) & 1;
    int id_mid = (chr >> 1) & 7;
    int id_high = (chr >> 4) & 3;
    
    for (int i = 0; i < 4; i++) {
      uint8_t mask = video_font[id_mid | ((i | (id_high << 2)) << 3)];
      
      for (int j = 0; j < 4; j++) {
        if (mask & (1 << (7 - (j | (id_low << 2))))) video_plot(x + j, y + i, color);
      }
    }
    
    x += 5;
    str++;
  }
}

void video_scroll(int height, int color) {
  memmove(video_ram + (8 << 2), video_ram + ((8 + height) << 2), (55 - height) << 4);
  video_rect(0, 55 - height, 126, height, color);
}
