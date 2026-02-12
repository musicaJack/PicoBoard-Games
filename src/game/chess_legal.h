/**
 * @file chess_legal.h
 * @brief 合法走法与应用：apply_move、legal_moves_from、do_move
 */

#ifndef PICO_CODE_CHESS_LEGAL_H
#define PICO_CODE_CHESS_LEGAL_H

#include "chess_state.h"
#include "chess_move.h"

/** 在棋盘副本上执行一步（不改 b），含易位/吃过路兵/升变 */
void chess_apply_move(int8_t board[8][8], const ChessMove *m);

/** 某格棋子的所有合法走法（伪合法 + 执行后己方王不被将军） */
void chess_legal_moves_from(const ChessBoardState *b, int r, int c, ChessMoveList *out);

/** 执行走棋：更新 state 的 board、易位/ep 并切换 side_to_move */
void chess_do_move(ChessBoardState *state, const ChessMove *m);

#endif /* PICO_CODE_CHESS_LEGAL_H */
