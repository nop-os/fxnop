#ifndef __DEVS_VIDEO_H__
#define __DEVS_VIDEO_H__

#include <fxnop/type.h>

enum {
  video_black,
  video_white,
  video_invert
};

extern uint32_t *video_ram;

extern volatile uint8_t *video_select;
extern volatile uint8_t *video_data;

uint8_t video_status(void);
void    video_set(uint8_t offset, uint8_t value);

void video_init(void);
void video_send(void);

void video_plot(int x, int y, int color);
void video_rect(int x, int y, int width, int height, int color);

#endif
