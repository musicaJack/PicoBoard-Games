/**
 * @file chess_result.h
 * @brief 终局判断：has_any_legal_move、get_game_result、all_legal_moves
 */

#ifndef PICO_CODE_CHESS_RESULT_H
#define PICO_CODE_CHESS_RESULT_H

#include "chess_state.h"
#include "chess_move.h"

/** 当前行棋方是否至少有一个合法走法 */
int chess_has_any_legal_move(const ChessBoardState *b);

/** 终局结果：0=进行中，1=白胜（黑被将死），2=黑胜（白被将死），3=逼和 */
int chess_get_game_result(const ChessBoardState *b);

/** 当前方所有合法走法（用于 AI），填入 out */
void chess_all_legal_moves(const ChessBoardState *b, ChessAllMovesList *out);

#endif /* PICO_CODE_CHESS_RESULT_H */
