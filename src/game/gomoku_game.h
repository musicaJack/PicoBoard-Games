#ifndef PICO_CODE_GOMOKU_GAME_H
#define PICO_CODE_GOMOKU_GAME_H

#include <stdbool.h>
#include <stdint.h>

#define GOMOKU_SIZE 15

typedef struct {
  uint8_t board[GOMOKU_SIZE][GOMOKU_SIZE];
  uint8_t cur_player;
  bool game_over;
  int win_r0, win_c0, win_r1, win_c1;
  bool has_win_line;
} GmkGameState;

void gmk_game_init(GmkGameState *g);
bool gmk_game_place_human(GmkGameState *g, int row, int col);
bool gmk_game_ai_move(GmkGameState *g);
bool gmk_game_is_over(const GmkGameState *g);
uint8_t gmk_game_cell(const GmkGameState *g, int row, int col);

#endif
