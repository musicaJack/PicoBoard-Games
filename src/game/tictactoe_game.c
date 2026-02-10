#include "game/tictactoe_game.h"
#include <stdbool.h>
#include <stdint.h>

#define AI_PLAYER  2
#define HU_PLAYER  1
#define SCORE_AI_WIN   1
#define SCORE_HU_WIN  -1
#define SCORE_DRAW    0
#define SCORE_NONE   -2

static bool board_win(const uint8_t b[3][3], uint8_t p) {
  for (int r = 0; r < 3; r++)
    if (b[r][0] == p && b[r][1] == p && b[r][2] == p) return true;
  for (int c = 0; c < 3; c++)
    if (b[0][c] == p && b[1][c] == p && b[2][c] == p) return true;
  if (b[0][0] == p && b[1][1] == p && b[2][2] == p) return true;
  if (b[0][2] == p && b[1][1] == p && b[2][0] == p) return true;
  return false;
}

static bool board_draw(const uint8_t b[3][3]) {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      if (b[r][c] == 0) return false;
  return true;
}

static bool check_win(TttGame *g, uint8_t p) {
  for (int r = 0; r < 3; r++) {
    if (g->board[r][0] == p && g->board[r][1] == p && g->board[r][2] == p) {
      g->has_win_line = true;
      g->win_a_r = (uint8_t)r; g->win_a_c = 0;
      g->win_b_r = (uint8_t)r; g->win_b_c = 2;
      return true;
    }
  }
  for (int c = 0; c < 3; c++) {
    if (g->board[0][c] == p && g->board[1][c] == p && g->board[2][c] == p) {
      g->has_win_line = true;
      g->win_a_r = 0; g->win_a_c = (uint8_t)c;
      g->win_b_r = 2; g->win_b_c = (uint8_t)c;
      return true;
    }
  }
  if (g->board[0][0] == p && g->board[1][1] == p && g->board[2][2] == p) {
    g->has_win_line = true;
    g->win_a_r = 0; g->win_a_c = 0;
    g->win_b_r = 2; g->win_b_c = 2;
    return true;
  }
  if (g->board[0][2] == p && g->board[1][1] == p && g->board[2][0] == p) {
    g->has_win_line = true;
    g->win_a_r = 0; g->win_a_c = 2;
    g->win_b_r = 2; g->win_b_c = 0;
    return true;
  }
  return false;
}

static int ai_evaluate(const uint8_t b[3][3]) {
  if (board_win(b, AI_PLAYER))  return SCORE_AI_WIN;
  if (board_win(b, HU_PLAYER))  return SCORE_HU_WIN;
  if (board_draw(b))            return SCORE_DRAW;
  return SCORE_NONE;
}

static int ai_alphabeta(uint8_t b[3][3], int alpha, int beta, bool maximizing) {
  int ev = ai_evaluate(b);
  if (ev != SCORE_NONE) return ev;
  if (maximizing) {
    int value = -10;
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        if (b[r][c] != 0) continue;
        b[r][c] = AI_PLAYER;
        int s = ai_alphabeta(b, alpha, beta, false);
        b[r][c] = 0;
        if (s > value) value = s;
        if (value > alpha) alpha = value;
        if (beta <= alpha) return value;
      }
    }
    return value;
  } else {
    int value = 10;
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        if (b[r][c] != 0) continue;
        b[r][c] = HU_PLAYER;
        int s = ai_alphabeta(b, alpha, beta, true);
        b[r][c] = 0;
        if (s < value) value = s;
        if (value < beta) beta = value;
        if (beta <= alpha) return value;
      }
    }
    return value;
  }
}

void ttt_game_init(TttGame *g) {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      g->board[r][c] = 0;
  g->cur_player = 1;
  g->game_over = false;
  g->winner = 0;
  g->has_win_line = false;
}

bool ttt_game_place_human(TttGame *g, int row, int col) {
  if (g->game_over) return false;
  if (g->cur_player != HU_PLAYER) return false;
  if (row < 0 || row > 2 || col < 0 || col > 2) return false;
  if (g->board[row][col] != 0) return false;
  g->board[row][col] = HU_PLAYER;
  if (check_win(g, HU_PLAYER)) {
    g->game_over = true;
    g->winner = 1;
  } else {
    bool is_draw = true;
    for (int r = 0; r < 3 && is_draw; r++)
      for (int c = 0; c < 3; c++)
        if (g->board[r][c] == 0) { is_draw = false; break; }
    if (is_draw) {
      g->game_over = true;
      g->has_win_line = false;
      g->winner = 0;
    } else {
      g->cur_player = AI_PLAYER;
    }
  }
  return true;
}

bool ttt_game_ai_move(TttGame *g) {
  if (g->game_over || g->cur_player != AI_PLAYER) return false;
  int best_r = 0, best_c = 0;
  int best_score = -10;
  uint8_t b[3][3];
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      b[r][c] = g->board[r][c];
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      if (b[r][c] != 0) continue;
      b[r][c] = AI_PLAYER;
      int s = ai_alphabeta(b, -10, 10, false);
      b[r][c] = 0;
      if (s > best_score) {
        best_score = s;
        best_r = r;
        best_c = c;
      }
    }
  }
  g->board[best_r][best_c] = AI_PLAYER;
  if (check_win(g, AI_PLAYER)) {
    g->game_over = true;
    g->winner = 2;
  } else {
    bool is_draw = true;
    for (int r = 0; r < 3 && is_draw; r++)
      for (int c = 0; c < 3; c++)
        if (g->board[r][c] == 0) { is_draw = false; break; }
    if (is_draw) {
      g->game_over = true;
      g->has_win_line = false;
      g->winner = 0;
    } else {
      g->cur_player = HU_PLAYER;
    }
  }
  return true;
}

bool ttt_game_is_over(const TttGame *g) {
  return g->game_over;
}

int ttt_game_cell(const TttGame *g, int row, int col) {
  if (row < 0 || row > 2 || col < 0 || col > 2) return 0;
  return g->board[row][col];
}
