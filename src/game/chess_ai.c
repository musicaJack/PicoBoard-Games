/**
 * @file chess_ai.c
 * @brief AI 选步入口：按难度调用 Easy / Medium
 */

#include "chess_state.h"
#include "chess_move.h"
#include "chess_ai.h"

extern int chess_ai_pick_move_easy(const ChessBoardState *state, ChessMove *out);
extern int chess_ai_pick_move_medium(const ChessBoardState *state, ChessMove *out);

int chess_ai_pick_move(const ChessBoardState *state, ChessAiDifficulty difficulty, ChessMove *out) {
    if (difficulty == CHESS_AI_EASY)
        return chess_ai_pick_move_easy(state, out);
    return chess_ai_pick_move_medium(state, out);
}
