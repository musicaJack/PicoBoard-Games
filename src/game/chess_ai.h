/**
 * @file chess_ai.h
 * @brief 难度枚举与 AI 选步接口
 */

#ifndef PICO_CODE_CHESS_AI_H
#define PICO_CODE_CHESS_AI_H

#include "chess_state.h"
#include "chess_move.h"

typedef enum {
    CHESS_AI_EASY = 0,
    CHESS_AI_MEDIUM = 1
} ChessAiDifficulty;

/** 为当前行棋方选一步：有合法步则写入 *out 并返回 1，否则返回 0 */
int chess_ai_pick_move(const ChessBoardState *state, ChessAiDifficulty difficulty, ChessMove *out);

#endif /* PICO_CODE_CHESS_AI_H */
