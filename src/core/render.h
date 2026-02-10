#ifndef PICO_CODE_CORE_RENDER_H
#define PICO_CODE_CORE_RENDER_H

#include <stdint.h>

typedef struct {
  uint16_t *buf;
  int w;
  int h;
} FrameBuffer;

void fb_put(FrameBuffer *fb, int x, int y, uint16_t color565);
void fb_fill_rect(FrameBuffer *fb, int x, int y, int w, int h, uint16_t color565);
void fb_clear(FrameBuffer *fb, uint16_t color565);
void fb_draw_line(FrameBuffer *fb, int x0, int y0, int x1, int y1, int thickness, uint16_t color565);
void fb_draw_circle(FrameBuffer *fb, int cx, int cy, int radius, int thickness, uint16_t color565);
void render_draw_glyph_5x7(FrameBuffer *fb, int x, int y, const uint8_t glyph[7], uint16_t color, int scale);

#endif
