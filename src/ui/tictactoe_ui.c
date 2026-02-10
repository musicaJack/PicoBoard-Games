#include "tictactoe_ui.h"
#include "core/input.h"
#include "core/render.h"
#include "game/tictactoe_game.h"
#include "DEV_Config.h"
#include "LCD_1in3.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LCD_W 240
#define LCD_H 240
#define C_BLACK      0x0000
#define C_WHITE      0xFFFF
#define C_RED        0xF800
#define C_GREEN      0x07E0
#define C_BLUE       0x001F
#define C_YELLOW     0xFFE0
#define C_GRAY       0x8410
#define C_DARK_GRAY  0x3186

static void draw_grid(FrameBuffer *fb) {
  fb_clear(fb, C_BLACK);
  const int cell = 80;
  const int lw = 4;
  fb_fill_rect(fb, cell - lw / 2, 0, lw, LCD_H, C_DARK_GRAY);
  fb_fill_rect(fb, 2 * cell - lw / 2, 0, lw, LCD_H, C_DARK_GRAY);
  fb_fill_rect(fb, 0, cell - lw / 2, LCD_W, lw, C_DARK_GRAY);
  fb_fill_rect(fb, 0, 2 * cell - lw / 2, LCD_W, lw, C_DARK_GRAY);
}

static void draw_piece_x(FrameBuffer *fb, int x0, int y0, int x1, int y1) {
  int pad = 14;
  int thick = 5;
  fb_draw_line(fb, x0 + pad, y0 + pad, x1 - pad, y1 - pad, thick, C_BLUE);
  fb_draw_line(fb, x0 + pad, y1 - pad, x1 - pad, y0 + pad, thick, C_BLUE);
}

static void draw_piece_o(FrameBuffer *fb, int x0, int y0, int x1, int y1) {
  int cx = (x0 + x1) / 2;
  int cy = (y0 + y1) / 2;
  int r = (x1 - x0) / 2 - 16;
  fb_draw_circle(fb, cx, cy, r, 4, C_RED);
}

static void draw_cursor(FrameBuffer *fb, int cr, int cc) {
  int cell = 80;
  int x0 = cc * cell;
  int y0 = cr * cell;
  int x1 = x0 + cell;
  int y1 = y0 + cell;
  int b = 4;
  fb_fill_rect(fb, x0 + 2, y0 + 2, cell - 4, b, C_YELLOW);
  fb_fill_rect(fb, x0 + 2, y1 - 2 - b, cell - 4, b, C_YELLOW);
  fb_fill_rect(fb, x0 + 2, y0 + 2, b, cell - 4, C_YELLOW);
  fb_fill_rect(fb, x1 - 2 - b, y0 + 2, b, cell - 4, C_YELLOW);
}

static void draw_win_line(FrameBuffer *fb, uint8_t ar, uint8_t ac, uint8_t br, uint8_t bc) {
  int cell = 80;
  int ax = ac * cell + cell / 2;
  int ay = ar * cell + cell / 2;
  int bx = bc * cell + cell / 2;
  int by = br * cell + cell / 2;
  fb_draw_line(fb, ax, ay, bx, by, 8, C_GREEN);
}

static const uint8_t font_5x7[14][7] = {
  { 0,0,0,0,0,0,0 },
  { 0x04,0x0A,0x11,0x11,0x1F,0x11,0x11 },
  { 0x1E,0x11,0x11,0x11,0x11,0x11,0x1E },
  { 0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F },
  { 0x0E,0x04,0x04,0x04,0x04,0x04,0x0E },
  { 0x10,0x10,0x10,0x10,0x10,0x10,0x1F },
  { 0x11,0x19,0x15,0x13,0x11,0x11,0x11 },
  { 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E },
  { 0x1E,0x11,0x11,0x1E,0x12,0x11,0x11 },
  { 0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E },
  { 0x1F,0x04,0x04,0x04,0x04,0x04,0x04 },
  { 0x11,0x11,0x11,0x11,0x11,0x11,0x0E },
  { 0x11,0x11,0x11,0x15,0x15,0x0A,0x0A },
  { 0x11,0x11,0x0A,0x04,0x04,0x04,0x04 },
};
#define IDX_SP 0
#define IDX_A 1
#define IDX_D 2
#define IDX_E 3
#define IDX_I 4
#define IDX_L 5
#define IDX_N 6
#define IDX_O 7
#define IDX_R 8
#define IDX_S 9
#define IDX_T 10
#define IDX_U 11
#define IDX_W 12
#define IDX_Y 13

static void draw_char_5x7(FrameBuffer *fb, int x, int y, int idx, uint16_t color) {
  if (idx < 0 || idx >= 14) return;
  render_draw_glyph_5x7(fb, x, y, font_5x7[idx], color, 2);
}

static void draw_popup(FrameBuffer *fb, uint8_t winner) {
  int pw = 140, ph = 52;
  int px = (LCD_W - pw) / 2, py = (LCD_H - ph) / 2;
  fb_fill_rect(fb, px, py, pw, ph, C_DARK_GRAY);
  fb_fill_rect(fb, px + 3, py + 3, pw - 6, ph - 6, C_BLACK);
  int scale = 2;
  int ch_w = 6 * scale;
  int tx = px + 8, ty = py + (ph - 7 * scale) / 2;
  if (winner == 1) {
    const int m[] = { IDX_Y, IDX_O, IDX_U, IDX_SP, IDX_W, IDX_I, IDX_N };
    for (int i = 0; i < 7; i++)
      draw_char_5x7(fb, tx + i * ch_w, ty, m[i], C_GREEN);
  } else if (winner == 2) {
    const int m[] = { IDX_Y, IDX_O, IDX_U, IDX_SP, IDX_L, IDX_O, IDX_S, IDX_T };
    for (int i = 0; i < 8; i++)
      draw_char_5x7(fb, tx + i * ch_w, ty, m[i], C_RED);
  } else {
    const int m[] = { IDX_D, IDX_R, IDX_A, IDX_W };
    for (int i = 0; i < 4; i++)
      draw_char_5x7(fb, tx + i * ch_w, ty, m[i], C_GRAY);
  }
}

static void render(const TttGame *g, int cursor_r, int cursor_c, FrameBuffer *fb) {
  draw_grid(fb);
  int cell = 80;
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      int x0 = c * cell;
      int y0 = r * cell;
      int x1 = x0 + cell;
      int y1 = y0 + cell;
      int cell_val = ttt_game_cell(g, r, c);
      if (cell_val == 1) draw_piece_x(fb, x0, y0, x1, y1);
      else if (cell_val == 2) draw_piece_o(fb, x0, y0, x1, y1);
    }
  }
  if (ttt_game_is_over(g)) {
    if (g->has_win_line)
      draw_win_line(fb, g->win_a_r, g->win_a_c, g->win_b_r, g->win_b_c);
    draw_popup(fb, g->winner);
  }
  draw_cursor(fb, cursor_r, cursor_c);
}

void tictactoe_run(void) {
  LCD_1IN3_Clear(C_BLACK);
  InputButton btn_a, btn_b, btn_x, btn_y, btn_up, btn_down, btn_left, btn_right, btn_ctrl;
  input_button_init(&btn_a, PIN_BTN_A);
  input_button_init(&btn_b, PIN_BTN_B);
  input_button_init(&btn_x, PIN_BTN_X);
  input_button_init(&btn_y, PIN_BTN_Y);
  input_button_init(&btn_up, PIN_JOY_UP);
  input_button_init(&btn_down, PIN_JOY_DOWN);
  input_button_init(&btn_left, PIN_JOY_LEFT);
  input_button_init(&btn_right, PIN_JOY_RIGHT);
  input_button_init(&btn_ctrl, PIN_JOY_CTRL);
  FrameBuffer fb;
  fb.w = LCD_W;
  fb.h = LCD_H;
  fb.buf = (uint16_t *)malloc((size_t)fb.w * (size_t)fb.h * sizeof(uint16_t));
  if (!fb.buf) return;
  TttGame game;
  ttt_game_init(&game);
  int cursor_r = 1, cursor_c = 1;
  render(&game, cursor_r, cursor_c, &fb);
  LCD_1IN3_Display((UWORD *)fb.buf);
  while (1) {
    bool dirty = false;
    if (input_button_pressed(&btn_x, 250)) {
      free(fb.buf);
      return;
    }
    if (input_button_pressed(&btn_up, 120))   { if (cursor_r > 0) { cursor_r--; dirty = true; } }
    if (input_button_pressed(&btn_down, 120)) { if (cursor_r < 2) { cursor_r++; dirty = true; } }
    if (input_button_pressed(&btn_left, 120)) { if (cursor_c > 0) { cursor_c--; dirty = true; } }
    if (input_button_pressed(&btn_right, 120)){ if (cursor_c < 2) { cursor_c++; dirty = true; } }
    if (input_button_pressed(&btn_a, 80) || input_button_pressed(&btn_y, 80) || input_button_pressed(&btn_ctrl, 80)) {
      if (ttt_game_place_human(&game, cursor_r, cursor_c)) {
        dirty = true;
        if (!ttt_game_is_over(&game))
          ttt_game_ai_move(&game);
      }
    }
    if (input_button_pressed(&btn_b, 200)) {
      ttt_game_init(&game);
      cursor_r = 1;
      cursor_c = 1;
      dirty = true;
    }
    if (dirty) {
      render(&game, cursor_r, cursor_c, &fb);
      LCD_1IN3_Display((UWORD *)fb.buf);
    }
    DEV_Delay_ms(20);
  }
}
