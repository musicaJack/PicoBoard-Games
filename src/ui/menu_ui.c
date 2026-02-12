#include "menu_ui.h"
#include "core/input.h"
#include "core/render.h"
#include "game/chess_pieces_small.h"
#include "DEV_Config.h"
#include "LCD_1in3.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LCD_W 240
#define LCD_H 240
#define C_BLACK  0x0000
#define C_WHITE  0xFFFF
#define C_YELLOW 0xFFE0
#define C_DARK_GRAY 0x3186

static void draw_icon_chess(FrameBuffer *fb, int cx, int cy) {
  int sz = CHESS_PIECE_SMALL_W;
  int x = cx - sz / 2, y = cy - sz / 2;
  chess_draw_piece_fb(fb, x, y, chess_pieces_small[9], sz, sz, C_WHITE, C_BLACK);
}

static void draw_icon_ttt(FrameBuffer *fb, int cx, int cy) {
  int cell = 8;
  int lw = 2;
  int x0 = cx - cell * 3 / 2 - lw;
  int y0 = cy - cell * 3 / 2 - lw;
  fb_fill_rect(fb, x0 + cell - lw/2, y0, lw, cell*3 + lw, C_WHITE);
  fb_fill_rect(fb, x0 + cell*2 - lw/2, y0, lw, cell*3 + lw, C_WHITE);
  fb_fill_rect(fb, x0, y0 + cell - lw/2, cell*3 + lw, lw, C_WHITE);
  fb_fill_rect(fb, x0, y0 + cell*2 - lw/2, cell*3 + lw, lw, C_WHITE);
}

static void draw_icon_gomoku(FrameBuffer *fb, int cx, int cy) {
  int r = 5;
  for (int i = -2; i <= 2; i++) {
    int dx = i * 14;
    for (int yy = -r; yy <= r; yy++)
      for (int xx = -r; xx <= r; xx++)
        if (xx*xx + yy*yy <= r*r)
          fb_fill_rect(fb, cx + dx + xx, cy + yy, 1, 1, C_WHITE);
  }
}

static void draw_menu(FrameBuffer *fb, int selection) {
  fb_fill_rect(fb, 0, 0, LCD_W, LCD_H, C_BLACK);
  int box_h = 56;
  int y0 = 24;
  int y1 = y0 + box_h + 8;
  int y2 = y1 + box_h + 8;

  for (int i = 0; i < 3; i++) {
    int y = (i == 0) ? y0 : (i == 1) ? y1 : y2;
    uint16_t border = (i == selection) ? C_YELLOW : C_DARK_GRAY;
    int bw = 2;
    fb_fill_rect(fb, 24, y, LCD_W - 48, box_h, C_BLACK);
    fb_fill_rect(fb, 24, y, LCD_W - 48, bw, border);
    fb_fill_rect(fb, 24, y + box_h - bw, LCD_W - 48, bw, border);
    fb_fill_rect(fb, 24, y, bw, box_h, border);
    fb_fill_rect(fb, LCD_W - 24 - bw, y, bw, box_h, border);
    if (i == selection)
      fb_fill_rect(fb, 32, y + box_h/2 - 6, 12, 12, C_YELLOW);
    if (i == 0)
      draw_icon_ttt(fb, 120, y + box_h/2);
    else if (i == 1)
      draw_icon_gomoku(fb, 120, y + box_h/2);
    else
      draw_icon_chess(fb, 120, y + box_h/2);
  }
}

int menu_run(void) {
  uint16_t *buf = (uint16_t *)malloc((size_t)LCD_W * (size_t)LCD_H * sizeof(uint16_t));
  if (!buf) return 1;

  FrameBuffer fb;
  fb.buf = buf;
  fb.w = LCD_W;
  fb.h = LCD_H;

  InputButton btn_up, btn_down, btn_a, btn_ctrl;
  input_button_init(&btn_up, PIN_JOY_UP);
  input_button_init(&btn_down, PIN_JOY_DOWN);
  input_button_init(&btn_a, PIN_BTN_A);
  input_button_init(&btn_ctrl, PIN_JOY_CTRL);

  int selection = 0;
  draw_menu(&fb, selection);
  LCD_1IN3_Display((UWORD *)buf);

  while (1) {
    if (input_button_pressed(&btn_up, 150))   { if (selection > 0) selection--; draw_menu(&fb, selection); LCD_1IN3_Display((UWORD *)buf); }
    if (input_button_pressed(&btn_down, 150)) { if (selection < 2) selection++; draw_menu(&fb, selection); LCD_1IN3_Display((UWORD *)buf); }
    if (input_button_pressed(&btn_a, 150) || input_button_pressed(&btn_ctrl, 150)) {
      free(buf);
      return selection + 1;
    }
    DEV_Delay_ms(20);
  }
}
