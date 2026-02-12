#include "core/render.h"
#include <stdlib.h>

static inline uint16_t swap16(uint16_t v) {
  return (uint16_t)((v << 8) | (v >> 8));
}

void fb_put(FrameBuffer *fb, int x, int y, uint16_t color565) {
  if ((unsigned)x >= (unsigned)fb->w || (unsigned)y >= (unsigned)fb->h) return;
  fb->buf[y * fb->w + x] = swap16(color565);
}

void fb_fill_rect(FrameBuffer *fb, int x, int y, int w, int h, uint16_t color565) {
  if (w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > fb->w) w = fb->w - x;
  if (y + h > fb->h) h = fb->h - y;
  if (w <= 0 || h <= 0) return;
  uint16_t c = swap16(color565);
  for (int yy = y; yy < y + h; yy++) {
    uint16_t *row = &fb->buf[yy * fb->w + x];
    for (int xx = 0; xx < w; xx++) row[xx] = c;
  }
}

void fb_clear(FrameBuffer *fb, uint16_t color565) {
  fb_fill_rect(fb, 0, 0, fb->w, fb->h, color565);
}

void fb_draw_line(FrameBuffer *fb, int x0, int y0, int x1, int y1, int thickness, uint16_t color565) {
  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;
  int r = thickness / 2;
  for (;;) {
    fb_fill_rect(fb, x0 - r, y0 - r, thickness, thickness, color565);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void fb_draw_circle(FrameBuffer *fb, int cx, int cy, int radius, int thickness, uint16_t color565) {
  if (radius <= 0) return;
  int t = thickness;
  if (t < 1) t = 1;
  for (int rr = radius; rr > radius - t && rr > 0; rr--) {
    int x = rr;
    int y = 0;
    int err = 0;
    while (x >= y) {
      fb_put(fb, cx + x, cy + y, color565);
      fb_put(fb, cx + y, cy + x, color565);
      fb_put(fb, cx - y, cy + x, color565);
      fb_put(fb, cx - x, cy + y, color565);
      fb_put(fb, cx - x, cy - y, color565);
      fb_put(fb, cx - y, cy - x, color565);
      fb_put(fb, cx + y, cy - x, color565);
      fb_put(fb, cx + x, cy - y, color565);
      y++;
      err += 1 + 2 * y;
      if (2 * (err - x) + 1 > 0) {
        x--;
        err += 1 - 2 * x;
      }
    }
  }
}

void render_draw_glyph_5x7(FrameBuffer *fb, int x, int y, const uint8_t glyph[7], uint16_t color, int scale) {
  for (int row = 0; row < 7; row++) {
    uint8_t bits = glyph[row];
    for (int col = 0; col < 5; col++) {
      if ((bits >> (4 - col)) & 1) {
        if (scale <= 1)
          fb_put(fb, x + col, y + row, color);
        else
          fb_fill_rect(fb, x + col * scale, y + row * scale, scale, scale, color);
      }
    }
  }
}

void chess_draw_piece_fb(FrameBuffer *fb, int x, int y, const uint8_t *bitmap, int w, int h, uint16_t fg, uint16_t bg) {
  if (w <= 0 || h <= 0) return;
  /* 位图按连续位流存放：像素 (r,c) 对应位 r*w+c，与 scale_pieces.py 一致 */
  for (int r = 0; r < h; r++) {
    int fy = y + r;
    if ((unsigned)fy >= (unsigned)fb->h) continue;
    for (int c = 0; c < w; c++) {
      int fx = x + c;
      if ((unsigned)fx >= (unsigned)fb->w) continue;
      int pos = r * w + c;
      int byte_idx = pos / 8;
      int bit = 7 - (pos % 8);
      int pixel = (bitmap[byte_idx] >> bit) & 1;
      fb_put(fb, fx, fy, pixel ? fg : bg);
    }
  }
}
