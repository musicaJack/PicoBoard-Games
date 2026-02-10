/*
 * 五子棋引擎：Minimax + Alpha-Beta 剪枝 + 棋型启发式评估
 * 设计要点：专业棋型权重、候选步邻域裁剪、必杀/必防预处理、移动排序
 */
#include "game/gomoku_game.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HU_PLAYER  1
#define AI_PLAYER  2

/* 棋型权重（与文献一致，单位：分） */
#define SCORE_FIVE    100000
#define SCORE_LIVE4   10000
#define SCORE_BLOCK4  5000
#define SCORE_LIVE3   2000
#define SCORE_BLOCK3  500
#define SCORE_LIVE2   100
#define SCORE_BLOCK2  30

#define SCORE_WIN   SCORE_FIVE
#define SCORE_LOSS  (-SCORE_FIVE)

/* 搜索深度：3 层（4 层在 Pico 上较慢） */
#define AI_DEPTH       3
#define CANDIDATE_RADIUS 2
#define MAX_CANDIDATES  64

static const int DR[4] = { 0, 1, 1,  1 };
static const int DC[4] = { 1, 0, 1, -1 };

/* ---------- 胜负与和棋检测 ---------- */
static bool check_win_at(GmkGameState *g, int r, int c, uint8_t p) {
  for (int d = 0; d < 4; d++) {
    int cnt = 1;
    int r0 = r, c0 = c, r1 = r, c1 = c;
    int rr = r + DR[d], cc = c + DC[d];
    while (rr >= 0 && rr < GOMOKU_SIZE && cc >= 0 && cc < GOMOKU_SIZE && g->board[rr][cc] == p) {
      cnt++; r1 = rr; c1 = cc;
      rr += DR[d]; cc += DC[d];
    }
    rr = r - DR[d]; cc = c - DC[d];
    while (rr >= 0 && rr < GOMOKU_SIZE && cc >= 0 && cc < GOMOKU_SIZE && g->board[rr][cc] == p) {
      cnt++; r0 = rr; c0 = cc;
      rr -= DR[d]; cc -= DC[d];
    }
    if (cnt >= 5) {
      g->has_win_line = true;
      g->win_r0 = r0; g->win_c0 = c0;
      g->win_r1 = r1; g->win_c1 = c1;
      return true;
    }
  }
  return false;
}

static bool is_draw(const GmkGameState *g) {
  for (int r = 0; r < GOMOKU_SIZE; r++)
    for (int c = 0; c < GOMOKU_SIZE; c++)
      if (g->board[r][c] == 0) return false;
  return true;
}

/* ---------- 棋型：单路（沿方向 dr,dc）的连续子数及两端是否为空 ---------- */
static int get_run_info(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], int r, int c, int dr, int dc,
                        uint8_t player, int *left_empty, int *right_empty) {
  int L = 0;
  int r0 = r, c0 = c;
  while (r0 >= 0 && r0 < GOMOKU_SIZE && c0 >= 0 && c0 < GOMOKU_SIZE && b[r0][c0] == player) {
    L++;
    r0 -= dr; c0 -= dc;
  }
  *left_empty = (r0 >= 0 && r0 < GOMOKU_SIZE && c0 >= 0 && c0 < GOMOKU_SIZE && b[r0][c0] == 0) ? 1 : 0;
  r0 = r + dr; c0 = c + dc;
  while (r0 >= 0 && r0 < GOMOKU_SIZE && c0 >= 0 && c0 < GOMOKU_SIZE && b[r0][c0] == player) {
    L++;
    r0 += dr; c0 += dc;
  }
  *right_empty = (r0 >= 0 && r0 < GOMOKU_SIZE && c0 >= 0 && c0 < GOMOKU_SIZE && b[r0][c0] == 0) ? 1 : 0;
  return L;
}

/* 仅当 (r,c) 为该方向上的“起点”时计分，避免重复计同一段 */
static int run_score(int L, int left_ok, int right_ok) {
  if (L >= 5) return SCORE_FIVE;
  if (L == 4) {
    if (left_ok && right_ok) return SCORE_LIVE4;
    if (left_ok || right_ok) return SCORE_BLOCK4;
    return 0;
  }
  if (L == 3) {
    if (left_ok && right_ok) return SCORE_LIVE3;
    if (left_ok || right_ok) return SCORE_BLOCK3;
    return 0;
  }
  if (L == 2) {
    if (left_ok && right_ok) return SCORE_LIVE2;
    if (left_ok || right_ok) return SCORE_BLOCK2;
    return 0;
  }
  return 0;
}

/* 对某一方，在 (r,c) 沿 (dr,dc) 方向的棋型分（仅当 (r-dr,c-dc) 不是同色时计入） */
static int pattern_at(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], int r, int c, int dr, int dc, uint8_t player) {
  int pr = r - dr, pc = c - dc;
  if (pr >= 0 && pr < GOMOKU_SIZE && pc >= 0 && pc < GOMOKU_SIZE && b[pr][pc] == player)
    return 0;
  int le, re;
  int L = get_run_info(b, r, c, dr, dc, player, &le, &re);
  return run_score(L, le, re);
}

/* 全盘对 player 的棋型总分（四方向，每段只从起点计一次） */
static int side_pattern_score(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], uint8_t player) {
  int total = 0;
  for (int r = 0; r < GOMOKU_SIZE; r++) {
    for (int c = 0; c < GOMOKU_SIZE; c++) {
      if (b[r][c] != player) continue;
      for (int d = 0; d < 4; d++)
        total += pattern_at(b, r, c, DR[d], DC[d], player);
    }
  }
  return total;
}

/* 局面评估：正数对 AI 有利。若已有五连则返回胜负分 */
static int evaluate(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE]) {
  int ai_s = side_pattern_score(b, AI_PLAYER);
  int hu_s = side_pattern_score(b, HU_PLAYER);
  if (ai_s >= SCORE_FIVE) return SCORE_WIN;
  if (hu_s >= SCORE_FIVE) return SCORE_LOSS;
  return ai_s - hu_s;
}

/* ---------- 候选步：仅考虑已有子周围 CANDIDATE_RADIUS 格内的空位 ---------- */
typedef struct { int r; int c; int score; } Candidate;

static bool in_neighborhood(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], int r, int c) {
  for (int rr = r - CANDIDATE_RADIUS; rr <= r + CANDIDATE_RADIUS; rr++)
    for (int cc = c - CANDIDATE_RADIUS; cc <= c + CANDIDATE_RADIUS; cc++) {
      if (rr < 0 || rr >= GOMOKU_SIZE || cc < 0 || cc >= GOMOKU_SIZE) continue;
      if (b[rr][cc] != 0) return true;
    }
  return false;
}

static bool has_any_piece(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE]) {
  for (int r = 0; r < GOMOKU_SIZE; r++)
    for (int c = 0; c < GOMOKU_SIZE; c++)
      if (b[r][c] != 0) return true;
  return false;
}

static int collect_candidates(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE],
                              Candidate *out, int max_out, bool for_ai) {
  int n = 0;
  uint8_t copy[GOMOKU_SIZE][GOMOKU_SIZE];
  memcpy(copy, b, sizeof(copy));
  bool use_neighborhood = has_any_piece(b);
  /* 开局无子时只考虑中腹，减少首步分支 */
  int r0 = 0, r1 = GOMOKU_SIZE, c0 = 0, c1 = GOMOKU_SIZE;
  if (!use_neighborhood) {
    int center = GOMOKU_SIZE / 2;
    int margin = 3;
    r0 = center - margin; r1 = center + margin + 1;
    c0 = center - margin; c1 = center + margin + 1;
    if (r0 < 0) r0 = 0; if (c0 < 0) c0 = 0;
    if (r1 > GOMOKU_SIZE) r1 = GOMOKU_SIZE; if (c1 > GOMOKU_SIZE) c1 = GOMOKU_SIZE;
  }
  for (int r = r0; r < r1 && n < max_out; r++) {
    for (int c = c0; c < c1 && n < max_out; c++) {
      if (copy[r][c] != 0) continue;
      if (use_neighborhood && !in_neighborhood(copy, r, c)) continue;
      out[n].r = r; out[n].c = c;
      copy[r][c] = for_ai ? AI_PLAYER : HU_PLAYER;
      out[n].score = evaluate(copy);
      copy[r][c] = 0;
      n++;
    }
  }
  return n;
}

/* 简单插入排序：按 score 降序（AI 先试高分，Human 先试低分，有利于剪枝） */
static void sort_candidates_max(Candidate *c, int n) {
  for (int i = 1; i < n; i++) {
    Candidate t = c[i];
    int j = i;
    while (j > 0 && c[j - 1].score < t.score) { c[j] = c[j - 1]; j--; }
    c[j] = t;
  }
}
static void sort_candidates_min(Candidate *c, int n) {
  for (int i = 1; i < n; i++) {
    Candidate t = c[i];
    int j = i;
    while (j > 0 && c[j - 1].score > t.score) { c[j] = c[j - 1]; j--; }
    c[j] = t;
  }
}

/* ---------- 必杀：下一步成五；必防：对方活四/冲四的防点 ---------- */
static bool would_win(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], int r, int c, uint8_t player) {
  uint8_t tmp[GOMOKU_SIZE][GOMOKU_SIZE];
  memcpy(tmp, b, sizeof(tmp));
  tmp[r][c] = player;
  for (int d = 0; d < 4; d++) {
    int cnt = 1;
    int rr = r + DR[d], cc = c + DC[d];
    while (rr >= 0 && rr < GOMOKU_SIZE && cc >= 0 && cc < GOMOKU_SIZE && tmp[rr][cc] == player) {
      cnt++; rr += DR[d]; cc += DC[d];
    }
    rr = r - DR[d]; cc = c - DC[d];
    while (rr >= 0 && rr < GOMOKU_SIZE && cc >= 0 && cc < GOMOKU_SIZE && tmp[rr][cc] == player) {
      cnt++; rr -= DR[d]; cc -= DC[d];
    }
    if (cnt >= 5) return true;
  }
  return false;
}

/* 若对方在此落子会成五，则 (r,c) 为必防点；返回是否找到唯一防点并写入 *out_r,*out_c */
static bool must_block(const uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], int *out_r, int *out_c) {
  int block_r = -1, block_c = -1;
  int count = 0;
  for (int r = 0; r < GOMOKU_SIZE; r++) {
    for (int c = 0; c < GOMOKU_SIZE; c++) {
      if (b[r][c] != 0) continue;
      if (would_win(b, r, c, HU_PLAYER)) {
        if (count == 0) { block_r = r; block_c = c; }
        count++;
      }
    }
  }
  if (count == 0) return false;
  *out_r = block_r; *out_c = block_c;
  return true;
}

/* ---------- Alpha-Beta 搜索（只扩展候选步） ---------- */
static int alphabeta(uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE], int depth, int alpha, int beta, bool maximizing) {
  int ev = evaluate(b);
  if (ev >= SCORE_WIN - 1000 || ev <= SCORE_LOSS + 1000) return ev;
  if (depth <= 0) return ev;

  Candidate cand[MAX_CANDIDATES];
  int n = collect_candidates(b, cand, MAX_CANDIDATES, maximizing);
  if (n == 0) return ev;

  if (maximizing) {
    sort_candidates_max(cand, n);
    int value = SCORE_LOSS;
    for (int i = 0; i < n; i++) {
      int r = cand[i].r, c = cand[i].c;
      b[r][c] = AI_PLAYER;
      int s = alphabeta(b, depth - 1, alpha, beta, false);
      b[r][c] = 0;
      if (s > value) value = s;
      if (value > alpha) alpha = value;
      if (beta <= alpha) return value;
    }
    return value;
  } else {
    sort_candidates_min(cand, n);
    int value = SCORE_WIN;
    for (int i = 0; i < n; i++) {
      int r = cand[i].r, c = cand[i].c;
      b[r][c] = HU_PLAYER;
      int s = alphabeta(b, depth - 1, alpha, beta, true);
      b[r][c] = 0;
      if (s < value) value = s;
      if (value < beta) beta = value;
      if (beta <= alpha) return value;
    }
    return value;
  }
}

void gmk_game_init(GmkGameState *g) {
  memset(g->board, 0, sizeof(g->board));
  g->cur_player = HU_PLAYER;
  g->game_over = false;
  g->has_win_line = false;
}

bool gmk_game_place_human(GmkGameState *g, int row, int col) {
  if (g->game_over || g->cur_player != HU_PLAYER) return false;
  if (row < 0 || row >= GOMOKU_SIZE || col < 0 || col >= GOMOKU_SIZE) return false;
  if (g->board[row][col] != 0) return false;
  g->board[row][col] = HU_PLAYER;
  if (check_win_at(g, row, col, HU_PLAYER)) g->game_over = true;
  else if (is_draw(g)) g->game_over = true;
  else g->cur_player = AI_PLAYER;
  return true;
}

bool gmk_game_ai_move(GmkGameState *g) {
  if (g->game_over || g->cur_player != AI_PLAYER) return false;

  uint8_t b[GOMOKU_SIZE][GOMOKU_SIZE];
  memcpy(b, g->board, sizeof(b));

  /* 1) 必杀：有一步成五则直接下 */
  for (int r = 0; r < GOMOKU_SIZE; r++) {
    for (int c = 0; c < GOMOKU_SIZE; c++) {
      if (b[r][c] != 0) continue;
      if (would_win(b, r, c, AI_PLAYER)) {
        g->board[r][c] = AI_PLAYER;
        if (check_win_at(g, r, c, AI_PLAYER)) g->game_over = true;
        else if (is_draw(g)) g->game_over = true;
        else g->cur_player = HU_PLAYER;
        return true;
      }
    }
  }

  /* 2) 必防：对方有活四/冲四则防 */
  int block_r, block_c;
  if (must_block(b, &block_r, &block_c) &&
      (unsigned)block_r < GOMOKU_SIZE && (unsigned)block_c < GOMOKU_SIZE) {
    g->board[block_r][block_c] = AI_PLAYER;
    if (check_win_at(g, block_r, block_c, AI_PLAYER)) g->game_over = true;
    else if (is_draw(g)) g->game_over = true;
    else g->cur_player = HU_PLAYER;
    return true;
  }

  /* 3) Alpha-Beta 搜索 */
  Candidate cand[MAX_CANDIDATES];
  int n = collect_candidates(b, cand, MAX_CANDIDATES, true);
  if (n == 0) return false;

  sort_candidates_max(cand, n);
  int best_r = cand[0].r, best_c = cand[0].c;
  int best_score = SCORE_LOSS;

  for (int i = 0; i < n; i++) {
    int r = cand[i].r, c = cand[i].c;
    b[r][c] = AI_PLAYER;
    int s = alphabeta(b, AI_DEPTH - 1, SCORE_LOSS, SCORE_WIN, false);
    b[r][c] = 0;
    if (s > best_score) {
      best_score = s;
      best_r = r;
      best_c = c;
    }
  }

  g->board[best_r][best_c] = AI_PLAYER;
  if (check_win_at(g, best_r, best_c, AI_PLAYER)) g->game_over = true;
  else if (is_draw(g)) g->game_over = true;
  else g->cur_player = HU_PLAYER;
  return true;
}

bool gmk_game_is_over(const GmkGameState *g) {
  return g->game_over;
}

uint8_t gmk_game_cell(const GmkGameState *g, int row, int col) {
  if (row < 0 || row >= GOMOKU_SIZE || col < 0 || col >= GOMOKU_SIZE) return 0;
  return g->board[row][col];
}
