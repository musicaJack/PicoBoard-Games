/**
 * @file chess_ui.c
 * @brief 国际象棋 UI：棋盘/棋子/光标/高亮、Check!/终局文案、难度选择、主循环
 */

#include "chess_ui.h"
#include "core/input.h"
#include "core/render.h"
#include "game/chess_types.h"
#include "game/chess_state.h"
#include "game/chess_move.h"
#include "game/chess_legal.h"
#include "game/chess_result.h"
#include "game/chess_check.h"
#include "game/chess_ai.h"
#include "game/chess_pieces_small.h"
#include "DEV_Config.h"
#include "LCD_1in3.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define LCD_W 240
#define LCD_H 240
#define CELL_SIZE 28
#define BOARD_PX (8 * CELL_SIZE)
#define BOARD_OFF_X ((LCD_W - BOARD_PX) / 2)
#define BOARD_OFF_Y 0
#define STATUS_Y (BOARD_PX + 1)
#define STATUS_H (LCD_H - STATUS_Y)
#define PIECE_SIZE 28

#define C_BLACK   0x0000
#define C_WHITE   0xFFFF
#define C_RED     0xF800
#define C_GREEN   0x07E0
#define C_YELLOW  0xFFE0
#define C_GRAY    0x8410
#define C_DARK    0x3186
#define C_LIGHT   0xC618

/* 5x7 字形：空格 + Check! YOU WIN LOST DRAW 等 */
static const uint8_t font_chess[][7] = {
  {0,0,0,0,0,0,0},                     /* space */
  {0x0E,0x11,0x10,0x10,0x11,0x11,0x0E}, /* C */
  {0x10,0x10,0x1E,0x11,0x11,0x11,0x11}, /* h：左竖+右弧顶+右竖到底，底行 10001 非 11110 */
  {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E}, /* e */
  {0x12,0x14,0x18,0x14,0x12,0x11,0x11}, /* k */
  {0x04,0x04,0x04,0x04,0x00,0x04,0x04}, /* ! */
  {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, /* Y */
  {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* O */
  {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, /* U */
  {0x1F,0x08,0x08,0x08,0x08,0x08,0x08}, /* T */
  {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, /* N */
  {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, /* R */
  {0x04,0x0A,0x11,0x11,0x1F,0x11,0x11}, /* A */
  {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E}, /* D */
  {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, /* I */
  {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* (O again) */
  {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E}, /* S */
  {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, /* L */
  {0x11,0x11,0x11,0x15,0x15,0x15,0x0A}, /* W */
  {0x1F,0x10,0x1E,0x10,0x10,0x10,0x1F}, /* E */
  {0x00,0x0E,0x01,0x0F,0x11,0x11,0x0F}, /* a */
  {0x00,0x0E,0x10,0x0E,0x01,0x11,0x0E}, /* s */
  {0x00,0x11,0x11,0x11,0x0F,0x01,0x0E}, /* y */
  {0x11,0x1B,0x15,0x11,0x11,0x11,0x11}, /* M */
  {0x00,0x00,0x16,0x19,0x11,0x11,0x11}, /* m */
  {0x00,0x01,0x01,0x0F,0x11,0x11,0x0F}, /* d */
  {0x00,0x04,0x00,0x04,0x04,0x04,0x0E}, /* i */
  {0x00,0x00,0x11,0x11,0x11,0x11,0x0E}, /* u */
  {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}, /* G */
  {0x00,0x00,0x00,0x00,0x00,0x04,0x04}, /* . */
  {0x00,0x00,0x0E,0x11,0x11,0x11,0x11}, /* n：弧顶+右竖，标准 5x7 */
  {0x00,0x0F,0x11,0x11,0x0F,0x01,0x0E}, /* g */
  {0x00,0x00,0x0E,0x10,0x10,0x10,0x0E}, /* c：Check! 用 */
};

static int chess_font_idx(char ch) {
  switch (ch) {
    case ' ': return 0;
    case 'C': return 1;
    case 'h': return 2;
    case 'e': return 3;
    case 'k': return 4;
    case '!': return 5;
    case 'Y': return 6;
    case 'O': return 7;
    case 'U': return 8;
    case 'T': return 9;
    case 'N': return 10;
    case 'R': return 11;
    case 'A': return 12;
    case 'D': return 13;
    case 'I': return 14;
    case 'S': return 16;
    case 'L': return 17;
    case 'W': return 18;
    case 'E': return 19;
    case 'a': return 20;
    case 's': return 21;
    case 'y': return 22;
    case 'M': return 23;
    case 'm': return 24;
    case 'd': return 25;
    case 'i': return 26;
    case 'u': return 27;
    case 'G': return 28;
    case '.': return 29;
    case 'n': return 30;
    case 'g': return 31;
    case 'c': return 32;
    default:  return 0;
  }
}

static void chess_draw_char(FrameBuffer *fb, int x, int y, char ch, uint16_t color) {
  int idx = chess_font_idx(ch);
  render_draw_glyph_5x7(fb, x, y, (const uint8_t*)font_chess[idx], color, 1);
}

static void chess_draw_text(FrameBuffer *fb, int x, int y, const char *s, uint16_t color) {
  for (int i = 0; s[i]; i++)
    chess_draw_char(fb, x + i * 6, y, s[i], color);
}

static void draw_board(FrameBuffer *fb) {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      uint16_t color = ((r + c) & 1) ? C_LIGHT : C_DARK;
      int x = BOARD_OFF_X + c * CELL_SIZE;
      int y = BOARD_OFF_Y + r * CELL_SIZE;
      fb_fill_rect(fb, x, y, CELL_SIZE, CELL_SIZE, color);
    }
  }
}

static void draw_pieces(FrameBuffer *fb, const ChessBoardState *state) {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      int8_t piece = state->board[r][c];
      if (piece == CHESS_EMPTY) continue;
      int px = BOARD_OFF_X + c * CELL_SIZE + (CELL_SIZE - PIECE_SIZE) / 2;
      int py = BOARD_OFF_Y + r * CELL_SIZE + (CELL_SIZE - PIECE_SIZE) / 2;
      uint16_t fg = (piece >= 6) ? C_WHITE : C_BLACK;
      uint16_t bg = ((r + c) & 1) ? C_LIGHT : C_DARK;
      const uint8_t *bitmap = chess_pieces_small[(int)piece];
      chess_draw_piece_fb(fb, px, py, bitmap, CHESS_PIECE_SMALL_W, CHESS_PIECE_SMALL_H, fg, bg);
    }
  }
}

static void draw_cursor(FrameBuffer *fb, int cur_r, int cur_c) {
  int x0 = BOARD_OFF_X + cur_c * CELL_SIZE, y0 = BOARD_OFF_Y + cur_r * CELL_SIZE;
  int b = 2;
  fb_fill_rect(fb, x0 + 1, y0 + 1, CELL_SIZE - 2, b, C_YELLOW);
  fb_fill_rect(fb, x0 + 1, y0 + CELL_SIZE - 1 - b, CELL_SIZE - 2, b, C_YELLOW);
  fb_fill_rect(fb, x0 + 1, y0 + 1, b, CELL_SIZE - 2, C_YELLOW);
  fb_fill_rect(fb, x0 + CELL_SIZE - 1 - b, y0 + 1, b, CELL_SIZE - 2, C_YELLOW);
}

/* 不再绘制可走位置绿框（需求：选中后不显示绿色提示） */

static void draw_status(FrameBuffer *fb, int game_result, int white_in_check) {
  fb_fill_rect(fb, 0, STATUS_Y, LCD_W, STATUS_H, C_BLACK);
  if (game_result == 1) { chess_draw_text(fb, (LCD_W - 6*9) / 2, STATUS_Y + 4, "YOU WIN!", C_GREEN); return; }
  if (game_result == 2) { chess_draw_text(fb, (LCD_W - 6*10) / 2, STATUS_Y + 4, "YOU LOST!", C_RED); return; }
  if (game_result == 3) { chess_draw_text(fb, (LCD_W - 6*5) / 2, STATUS_Y + 4, "DRAW!", C_GRAY); return; }
  if (white_in_check)   { chess_draw_text(fb, (LCD_W - 6*6) / 2, STATUS_Y + 4, "Check!", C_YELLOW); return; }
}

static void draw_status_ai_thinking(FrameBuffer *fb) {
  fb_fill_rect(fb, 0, STATUS_Y, LCD_W, STATUS_H, C_BLACK);
  chess_draw_text(fb, (LCD_W - 6*14) / 2, STATUS_Y + 4, "AI Thinking...", C_GRAY);
}

static void draw_last_ai_highlight(FrameBuffer *fb, int to_r, int to_c) {
  if (to_r < 0 || to_c < 0) return;
  int x = BOARD_OFF_X + to_c * CELL_SIZE, y = BOARD_OFF_Y + to_r * CELL_SIZE;
  int bw = 2;
  fb_fill_rect(fb, x, y, CELL_SIZE, bw, C_RED);
  fb_fill_rect(fb, x, y + CELL_SIZE - bw, CELL_SIZE, bw, C_RED);
  fb_fill_rect(fb, x, y, bw, CELL_SIZE, C_RED);
  fb_fill_rect(fb, x + CELL_SIZE - bw, y, bw, CELL_SIZE, C_RED);
}

/** 人类选中棋子时用黄框标出；sel_r>=0 时绘制，再选同一格取消后不绘 */
static void draw_selected_highlight(FrameBuffer *fb, int sel_r, int sel_c) {
  if (sel_r < 0 || sel_c < 0) return;
  int x = BOARD_OFF_X + sel_c * CELL_SIZE, y = BOARD_OFF_Y + sel_r * CELL_SIZE;
  int bw = 2;
  fb_fill_rect(fb, x, y, CELL_SIZE, bw, C_YELLOW);
  fb_fill_rect(fb, x, y + CELL_SIZE - bw, CELL_SIZE, bw, C_YELLOW);
  fb_fill_rect(fb, x, y, bw, CELL_SIZE, C_YELLOW);
  fb_fill_rect(fb, x + CELL_SIZE - bw, y, bw, CELL_SIZE, C_YELLOW);
}

static void draw_difficulty_screen(FrameBuffer *fb, int selection) {
  fb_fill_rect(fb, 0, 0, LCD_W, LCD_H, C_BLACK);
  int box_h = 56;
  int y0 = 60, y1 = 140;
  uint16_t border = C_DARK;
  int bw = 2;
  for (int i = 0; i < 2; i++) {
    int y = (i == 0) ? y0 : y1;
    border = (i == selection) ? C_YELLOW : C_DARK;
    fb_fill_rect(fb, 40, y, LCD_W - 80, box_h, C_BLACK);
    fb_fill_rect(fb, 40, y, LCD_W - 80, bw, border);
    fb_fill_rect(fb, 40, y + box_h - bw, LCD_W - 80, bw, border);
    fb_fill_rect(fb, 40, y, bw, box_h, border);
    fb_fill_rect(fb, LCD_W - 40 - bw, y, bw, box_h, border);
    if (i == selection)
      fb_fill_rect(fb, 52, y + box_h/2 - 6, 12, 12, C_YELLOW);
    const char *label = (i == 0) ? "Easy" : "Medium";
    int len = (i == 0) ? 4 : 6;
    chess_draw_text(fb, (LCD_W - len * 6) / 2, y + box_h/2 - 4, label, C_WHITE);
  }
}

/* 难度选择：返回 CHESS_AI_EASY 或 CHESS_AI_MEDIUM；选 X 返回 -1 表示退出到菜单 */
static int run_difficulty_selection(FrameBuffer *fb) {
  int selection = 0;
  draw_difficulty_screen(fb, selection);
  LCD_1IN3_Display((UWORD *)fb->buf);

  InputButton btn_up, btn_down, btn_a, btn_x, btn_ctrl;
  input_button_init(&btn_up, PIN_JOY_UP);
  input_button_init(&btn_down, PIN_JOY_DOWN);
  input_button_init(&btn_a, PIN_BTN_A);
  input_button_init(&btn_x, PIN_BTN_X);
  input_button_init(&btn_ctrl, PIN_JOY_CTRL);

  while (1) {
    if (input_button_pressed(&btn_x, 200)) return -1;
    if (input_button_pressed(&btn_up, 150))   { selection = 0; draw_difficulty_screen(fb, selection); LCD_1IN3_Display((UWORD *)fb->buf); }
    if (input_button_pressed(&btn_down, 150)) { selection = 1; draw_difficulty_screen(fb, selection); LCD_1IN3_Display((UWORD *)fb->buf); }
    if (input_button_pressed(&btn_a, 150) || input_button_pressed(&btn_ctrl, 150))
      return selection;
    DEV_Delay_ms(20);
  }
}

static void full_redraw(FrameBuffer *fb, const ChessBoardState *state,
                       int cur_r, int cur_c, int sel_r, int sel_c,
                       int last_ai_r, int last_ai_c, int game_result) {
  draw_board(fb);
  draw_pieces(fb, state);
  draw_last_ai_highlight(fb, last_ai_r, last_ai_c);
  draw_selected_highlight(fb, sel_r, sel_c);
  draw_cursor(fb, cur_r, cur_c);
  int white_check = (state->side_to_move == 1 && chess_is_king_in_check(state, 1)) ? 1 : 0;
  draw_status(fb, game_result, white_check);
}

void chess_run(void) {
  LCD_1IN3_Clear(C_BLACK);
  FrameBuffer fb;
  fb.w = LCD_W;
  fb.h = LCD_H;
  fb.buf = (uint16_t *)malloc((size_t)fb.w * (size_t)fb.h * sizeof(uint16_t));
  if (!fb.buf) return;

  int difficulty = run_difficulty_selection(&fb);
  if (difficulty < 0) { free(fb.buf); return; }

  ChessAiDifficulty ai_diff = (difficulty == 0) ? CHESS_AI_EASY : CHESS_AI_MEDIUM;

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

  ChessBoardState state;
  chess_state_init_from_initial(&state);
  int cur_r = 4, cur_c = 4;
  int sel_r = -1, sel_c = -1;
  int last_ai_r = -1, last_ai_c = -1;
  ChessMoveList legal_list;
  chess_move_list_clear(&legal_list);

  full_redraw(&fb, &state, cur_r, cur_c, sel_r, sel_c, last_ai_r, last_ai_c, 0);
  LCD_1IN3_Display((UWORD *)fb.buf);

  while (1) {
    int game_result = chess_get_game_result(&state);
    bool dirty = false;

    if (input_button_pressed(&btn_x, 250)) { free(fb.buf); return; }
    if (input_button_pressed(&btn_b, 200)) {
      chess_state_init_from_initial(&state);
      cur_r = cur_c = 4;
      sel_r = sel_c = -1;
      last_ai_r = last_ai_c = -1;
      chess_move_list_clear(&legal_list);
      dirty = true;
    }

    if (game_result == 0) {
      if (input_button_pressed(&btn_up, 120))   { if (cur_r > 0) { cur_r--; dirty = true; } }
      if (input_button_pressed(&btn_down, 120)) { if (cur_r < 7) { cur_r++; dirty = true; } }
      if (input_button_pressed(&btn_left, 120))  { if (cur_c > 0) { cur_c--; dirty = true; } }
      if (input_button_pressed(&btn_right, 120)){ if (cur_c < 7) { cur_c++; dirty = true; } }

      if (input_button_pressed(&btn_a, 80) || input_button_pressed(&btn_y, 80) || input_button_pressed(&btn_ctrl, 80)) {
        if (state.side_to_move != 1) { /* 人类执白，只有白方时处理选中/走子 */ }
        else {
          if (sel_r >= 0 && cur_r == sel_r && cur_c == sel_c) {
            sel_r = sel_c = -1;
            chess_move_list_clear(&legal_list);
            dirty = true;
          } else if (sel_r >= 0) {
            int found = 0;
            ChessMove chosen;
            for (int i = 0; i < legal_list.count; i++)
              if (legal_list.moves[i].to_r == cur_r && legal_list.moves[i].to_c == cur_c) {
                chosen = legal_list.moves[i];
                found = 1;
                break;
              }
            if (found) {
              chess_do_move(&state, &chosen);
              sel_r = sel_c = -1;
              chess_move_list_clear(&legal_list);
              game_result = chess_get_game_result(&state);
              dirty = true;
              if (game_result == 0 && state.side_to_move == 0) {
                /* 先显示 "AI Thinking..." 再计算 */
                full_redraw(&fb, &state, cur_r, cur_c, sel_r, sel_c, last_ai_r, last_ai_c, 0);
                draw_status_ai_thinking(&fb);
                LCD_1IN3_Display((UWORD *)fb.buf);
                ChessMove ai_move;
                if (chess_ai_pick_move(&state, ai_diff, &ai_move)) {
                  chess_do_move(&state, &ai_move);
                  last_ai_r = ai_move.to_r;
                  last_ai_c = ai_move.to_c;
                  game_result = chess_get_game_result(&state);
                }
                dirty = true;
              }
            }
          } else {
            if (chess_is_own_piece(state.board[cur_r][cur_c], 1)) {
              chess_legal_moves_from(&state, cur_r, cur_c, &legal_list);
              if (legal_list.count > 0) {
                sel_r = cur_r;
                sel_c = cur_c;
                dirty = true;
              }
            }
          }
        }
      }
    }

    if (dirty) {
      full_redraw(&fb, &state, cur_r, cur_c, sel_r, sel_c, last_ai_r, last_ai_c, game_result);
      LCD_1IN3_Display((UWORD *)fb.buf);
    }
    DEV_Delay_ms(20);
  }
}
