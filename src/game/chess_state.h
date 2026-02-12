/**
 * @file chess_state.h
 * @brief 棋盘状态：board、side_to_move、易位资格、吃过路兵列
 */

#ifndef PICO_CODE_CHESS_STATE_H
#define PICO_CODE_CHESS_STATE_H

#include <stdbool.h>
#include <stdint.h>

/** 棋盘状态（含易位资格与吃过路兵列） */
typedef struct {
    int8_t board[8][8];
    int side_to_move;       /* 0=黑 1=白 */
    bool castling[2][2];    /* [color][0=queenside, 1=kingside] 是否仍可易位 */
    int ep_col;             /* 吃过路兵目标列 0..7，无则 -1 */
} ChessBoardState;

void chess_state_init_from_initial(ChessBoardState *b);
int8_t chess_state_at(const ChessBoardState *b, int r, int c);
int chess_state_is_empty(const ChessBoardState *b, int r, int c);
int chess_state_in_bounds(int r, int c);

#endif /* PICO_CODE_CHESS_STATE_H */
