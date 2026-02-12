/**
 * @file chess_pseudo.h
 * @brief 伪合法走法生成：王/后/车/象/马/兵（含易位、吃过路兵、升变仅后）
 */

#ifndef PICO_CODE_CHESS_PSEUDO_H
#define PICO_CODE_CHESS_PSEUDO_H

#include "chess_state.h"
#include "chess_move.h"

void chess_pseudo_king(const ChessBoardState *b, int r, int c, ChessMoveList *out);
void chess_pseudo_queen(const ChessBoardState *b, int r, int c, ChessMoveList *out);
void chess_pseudo_rook(const ChessBoardState *b, int r, int c, ChessMoveList *out);
void chess_pseudo_bishop(const ChessBoardState *b, int r, int c, ChessMoveList *out);
void chess_pseudo_knight(const ChessBoardState *b, int r, int c, ChessMoveList *out);
void chess_pseudo_pawn(const ChessBoardState *b, int r, int c, ChessMoveList *out);

/** 某格棋子的所有伪合法目标格（升变只生成升后） */
void chess_pseudo_moves_from(const ChessBoardState *b, int r, int c, ChessMoveList *out);

#endif /* PICO_CODE_CHESS_PSEUDO_H */
