#ifndef PICO_CODE_TICTACTOE_GAME_H
#define PICO_CODE_TICTACTOE_GAME_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t board[3][3];
  uint8_t cur_player;
  bool game_over;
  uint8_t winner;
  uint8_t win_a_r, win_a_c;
  uint8_t win_b_r, win_b_c;
  bool has_win_line;
} TttGame;

void ttt_game_init(TttGame *g);
bool ttt_game_place_human(TttGame *g, int row, int col);
bool ttt_game_ai_move(TttGame *g);
bool ttt_game_is_over(const TttGame *g);
int ttt_game_cell(const TttGame *g, int row, int col);

#endif
