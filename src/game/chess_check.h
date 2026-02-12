/**
 * @file chess_check.h
 * @brief 将军与格攻击判断：find_king、is_king_in_check、is_square_attacked
 */

#ifndef PICO_CODE_CHESS_CHECK_H
#define PICO_CODE_CHESS_CHECK_H

#include "chess_state.h"

/** 找到己方王的位置，返回 1 且 *out_r,*out_c 有效；若无则返回 0 */
int chess_find_king(const int8_t board[8][8], int side, int *out_r, int *out_c);

/** 格 (r,c) 是否被 by_side 方攻击 */
int chess_is_square_attacked(const ChessBoardState *b, int r, int c, int by_side);

/** 己方王是否被对方攻击 */
int chess_is_king_in_check(const ChessBoardState *b, int side);

#endif /* PICO_CODE_CHESS_CHECK_H */
