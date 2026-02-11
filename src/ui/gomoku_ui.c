#include "gomoku_ui.h"
#include "core/input.h"
#include "core/render.h"
#include "game/gomoku_game.h"
#include "DEV_Config.h"
#include "LCD_1in3.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LCD_W 240
#define LCD_H 240
#define BOARD_PX (15 * GOMOKU_SIZE)
#define CELL_PX  (BOARD_PX / GOMOKU_SIZE)
#define BOARD_X ((LCD_W - BOARD_PX) / 2)
#define BOARD_Y 0
/* 状态栏从棋盘下边界下方 1px 开始，避免遮住棋盘最底横线 */
#define STATUS_Y (BOARD_PX + 1)
#define STATUS_H (LCD_H - STATUS_Y)

#define C_BLACK      0x0000
#define C_WHITE      0xFFFF
#define C_RED        0xF800
#define C_GREEN      0x07E0
#define C_BLUE       0x001F
#define C_YELLOW     0xFFE0
#define C_GRAY       0x8410
#define C_DARK_GRAY  0x3186

static void gmk_draw_grid(FrameBuffer *fb) {
  fb_fill_rect(fb, 0, 0, LCD_W, LCD_H, C_BLACK);
  int lw = 1;
  for (int i = 0; i <= GOMOKU_SIZE; i++) {
    fb_fill_rect(fb, BOARD_X + i * CELL_PX - lw/2, BOARD_Y, lw, BOARD_PX, C_DARK_GRAY);
    fb_fill_rect(fb, BOARD_X, BOARD_Y + i * CELL_PX - lw/2, BOARD_PX, lw, C_DARK_GRAY);
  }
}

static void gmk_draw_piece_x(FrameBuffer *fb, int x0, int y0, int x1, int y1) {
  int pad = 3, thick = 2;
  fb_draw_line(fb, x0 + pad, y0 + pad, x1 - pad, y1 - pad, thick, C_BLUE);
  fb_draw_line(fb, x0 + pad, y1 - pad, x1 - pad, y0 + pad, thick, C_BLUE);
}

static void gmk_draw_piece_o(FrameBuffer *fb, int x0, int y0, int x1, int y1) {
  int cx = (x0 + x1) / 2, cy = (y0 + y1) / 2;
  int r = (x1 - x0) / 2 - 3;
  fb_draw_circle(fb, cx, cy, r, 2, C_RED);
}

static void gmk_draw_cursor(FrameBuffer *fb, int row, int col) {
  int x0 = BOARD_X + col * CELL_PX, y0 = BOARD_Y + row * CELL_PX;
  int b = 2;
  fb_fill_rect(fb, x0 + 1, y0 + 1, CELL_PX - 2, b, C_YELLOW);
  fb_fill_rect(fb, x0 + 1, y0 + CELL_PX - 1 - b, CELL_PX - 2, b, C_YELLOW);
  fb_fill_rect(fb, x0 + 1, y0 + 1, b, CELL_PX - 2, C_YELLOW);
  fb_fill_rect(fb, x0 + CELL_PX - 1 - b, y0 + 1, b, CELL_PX - 2, C_YELLOW);
}

static void gmk_draw_win_line(FrameBuffer *fb, int r0, int c0, int r1, int c1) {
  int x0 = BOARD_X + c0 * CELL_PX + CELL_PX/2, y0 = BOARD_Y + r0 * CELL_PX + CELL_PX/2;
  int x1 = BOARD_X + c1 * CELL_PX + CELL_PX/2, y1 = BOARD_Y + r1 * CELL_PX + CELL_PX/2;
  fb_draw_line(fb, x0, y0, x1, y1, 4, C_GREEN);
}

/* skip_r, skip_c: if both >= 0, do not draw the piece at (skip_r, skip_c) — for blink effect */
static void gmk_render_ex(const GmkGameState *g, int cursor_r, int cursor_c, int skip_r, int skip_c, FrameBuffer *fb) {
  gmk_draw_grid(fb);
  for (int r = 0; r < GOMOKU_SIZE; r++)
    for (int c = 0; c < GOMOKU_SIZE; c++) {
      if (r == skip_r && c == skip_c) continue;
      int x0 = BOARD_X + c * CELL_PX, y0 = BOARD_Y + r * CELL_PX;
      int x1 = x0 + CELL_PX, y1 = y0 + CELL_PX;
      uint8_t cell = gmk_game_cell(g, r, c);
      if (cell == 1) gmk_draw_piece_x(fb, x0, y0, x1, y1);
      else if (cell == 2) gmk_draw_piece_o(fb, x0, y0, x1, y1);
    }
  if (gmk_game_is_over(g)) {
    if (g->has_win_line) gmk_draw_win_line(fb, g->win_r0, g->win_c0, g->win_r1, g->win_c1);
  }
  gmk_draw_cursor(fb, cursor_r, cursor_c);
}

static void gmk_render(const GmkGameState *g, int cursor_r, int cursor_c, FrameBuffer *fb) {
  gmk_render_ex(g, cursor_r, cursor_c, -1, -1, fb);
}

static const uint8_t font5x7_gmk[][7] = {
  {0,0,0,0,0,0,0},
  {0x04,0x0A,0x11,0x11,0x1F,0x11,0x11}, /* A */
  {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}, /* G */
  {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, /* H */
  {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, /* I */
  {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, /* K */
  {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, /* N */
  {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, /* T */
  {0x00,0x00,0x00,0x00,0x00,0x04,0x04}, /* . */
  {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, /* Y */
  {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* O */
  {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, /* U */
  {0x11,0x11,0x11,0x15,0x15,0x15,0x0A}, /* W */
  {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, /* L */
  {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E}, /* S */
  {0x04,0x04,0x04,0x04,0x00,0x04,0x04}, /* ! */
};
static int gmk_font_idx(char ch) {
  switch (ch) {
    case ' ': return 0;
    case 'A': return 1;
    case 'G': return 2;
    case 'H': return 3;
    case 'I': return 4;
    case 'K': return 5;
    case 'N': return 6;
    case 'T': return 7;
    case '.': return 8;
    case 'Y': return 9;
    case 'O': return 10;
    case 'U': return 11;
    case 'W': return 12;
    case 'L': return 13;
    case 'S': return 14;
    case '!': return 15;
    default: return 0;
  }
}
static void gmk_draw_char5x7(FrameBuffer *fb, int x, int y, char ch, uint16_t color) {
  int idx = gmk_font_idx(ch);
  render_draw_glyph_5x7(fb, x, y, font5x7_gmk[idx], color, 1);
}
static void gmk_draw_text5x7(FrameBuffer *fb, int x, int y, const char *s, uint16_t color) {
  for (int i = 0; s[i]; i++)
    gmk_draw_char5x7(fb, x + i * 6, y, s[i], color);
}
static void gmk_draw_status_bar(FrameBuffer *fb, bool thinking) {
  fb_fill_rect(fb, 0, STATUS_Y, LCD_W, STATUS_H, C_BLACK);
  if (thinking) {
    gmk_draw_text5x7(fb, 28, STATUS_Y + 4, "AI THINKING...", C_GRAY);
  }
}

/* human_won: true = show "YOU WIN!", false = show "YOU LOST!" */
static void gmk_draw_game_over_message(FrameBuffer *fb, bool human_won) {
  const char *msg = human_won ? "YOU WIN!" : "YOU LOST!";
  int len = human_won ? 8 : 9;
  int text_w = len * 6;
  int x = (LCD_W - text_w) / 2;
  uint16_t color = human_won ? C_GREEN : C_RED;
  fb_fill_rect(fb, 0, STATUS_Y, LCD_W, STATUS_H, C_BLACK);
  gmk_draw_text5x7(fb, x, STATUS_Y + 4, msg, color);
}

void gomoku_run(void) {
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
  GmkGameState game;
  gmk_game_init(&game);
  int cursor_r = GOMOKU_SIZE / 2, cursor_c = GOMOKU_SIZE / 2;
  gmk_render(&game, cursor_r, cursor_c, &fb);
  gmk_draw_status_bar(&fb, false);
  LCD_1IN3_Display((UWORD *)fb.buf);
  while (1) {
    bool dirty = false;
    if (input_button_pressed(&btn_x, 250)) {
      free(fb.buf);
      return;
    }
    if (input_button_pressed(&btn_up, 120))   { if (cursor_r > 0) { cursor_r--; dirty = true; } }
    if (input_button_pressed(&btn_down, 120)) { if (cursor_r < GOMOKU_SIZE - 1) { cursor_r++; dirty = true; } }
    if (input_button_pressed(&btn_left, 120)) { if (cursor_c > 0) { cursor_c--; dirty = true; } }
    if (input_button_pressed(&btn_right, 120)){ if (cursor_c < GOMOKU_SIZE - 1) { cursor_c++; dirty = true; } }
    if (input_button_pressed(&btn_a, 80) || input_button_pressed(&btn_y, 80) || input_button_pressed(&btn_ctrl, 80)) {
      if (gmk_game_place_human(&game, cursor_r, cursor_c)) {
        gmk_render(&game, cursor_r, cursor_c, &fb);
        gmk_draw_status_bar(&fb, false);
        if (gmk_game_is_over(&game) && game.has_win_line) gmk_draw_game_over_message(&fb, true);
        LCD_1IN3_Display((UWORD *)fb.buf);
        if (!gmk_game_is_over(&game) && game.cur_player == 2) {
          gmk_draw_status_bar(&fb, true);
          LCD_1IN3_Display((UWORD *)fb.buf);
          int ai_r = 0, ai_c = 0;
          gmk_game_ai_move(&game, &ai_r, &ai_c);
          for (int i = 0; i < 5; i++) {
            gmk_render_ex(&game, cursor_r, cursor_c, -1, -1, &fb);
            gmk_draw_status_bar(&fb, false);
            LCD_1IN3_Display((UWORD *)fb.buf);
            DEV_Delay_ms(250);
            gmk_render_ex(&game, cursor_r, cursor_c, ai_r, ai_c, &fb);
            gmk_draw_status_bar(&fb, false);
            LCD_1IN3_Display((UWORD *)fb.buf);
            DEV_Delay_ms(250);
          }
          gmk_render(&game, cursor_r, cursor_c, &fb);
          gmk_draw_status_bar(&fb, false);
          if (gmk_game_is_over(&game) && game.has_win_line) gmk_draw_game_over_message(&fb, false);
          LCD_1IN3_Display((UWORD *)fb.buf);
        }
        dirty = true;
      }
    }
    if (input_button_pressed(&btn_b, 200)) {
      gmk_game_init(&game);
      cursor_r = cursor_c = GOMOKU_SIZE / 2;
      dirty = true;
    }
    if (dirty) {
      gmk_render(&game, cursor_r, cursor_c, &fb);
      gmk_draw_status_bar(&fb, false);
      if (gmk_game_is_over(&game) && game.has_win_line) {
        bool human_won = (game.cur_player == 2);
        gmk_draw_game_over_message(&fb, human_won);
      }
      LCD_1IN3_Display((UWORD *)fb.buf);
    }
    DEV_Delay_ms(20);
  }
}
