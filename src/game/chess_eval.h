/**
 * @file chess_eval.h
 * @brief 简单局面评估：子力价值（与 demo eval.hpp 一致，供 AI 选步）
 */

#ifndef PICO_CODE_CHESS_EVAL_H
#define PICO_CODE_CHESS_EVAL_H

#include "chess_state.h"
#include "chess_move.h"

/** 子力价值（后9 车5 象马3 兵1 王0，仅用于比较） */
int chess_piece_value(int8_t piece_index);

/** 局面评估：side 方的子力减去对方子力（越大对 side 越有利） */
int chess_eval_material(const ChessBoardState *b, int side);

/** 在副本上执行一步并返回执行后对 side 的评估（用于 AI 选步） */
int chess_eval_after_move(const ChessBoardState *b, const ChessMove *m, int side);

#endif /* PICO_CODE_CHESS_EVAL_H */
