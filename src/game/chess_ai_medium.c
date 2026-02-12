/**
 * @file chess_ai_medium.c
 * @brief Medium AI：Negamax + Alpha-Beta，3 层搜索，叶子用 eval_material（demo 为 2 层，此处加深以增强棋力）
 */

#include <stdlib.h>
#include "chess_state.h"
#include "chess_move.h"
#include "chess_result.h"
#include "chess_eval.h"
#include "chess_legal.h"
#include "chess_ai.h"

#if defined(PICO_ON_DEVICE) && defined(LIB_PICO_STDLIB)
#include "pico/stdlib.h"
#include "pico/time.h"
#endif

#define CHESS_MATE_SCORE 10000
#define CHESS_MEDIUM_SEARCH_DEPTH 3   /* 3 层：己方-对方-己方 再评估，比 2 层强不少；再高在 Pico 上会变慢 */

/** Negamax + Alpha-Beta：返回当前行棋方的得分，越大越有利 */
static int search(const ChessBoardState *state, int depth, int alpha, int beta) {
    ChessAllMovesList list;
    chess_all_legal_moves(state, &list);

    if (list.count == 0) {
        int result = chess_get_game_result(state);
        if (result == 3) return 0;
        int current_side_won = (result == 1 && state->side_to_move == 1) || (result == 2 && state->side_to_move == 0);
        return current_side_won ? CHESS_MATE_SCORE : -CHESS_MATE_SCORE;
    }

    if (depth == 0)
        return chess_eval_material(state, state->side_to_move);

    int best = -CHESS_MATE_SCORE - 1;
    for (int i = 0; i < list.count; i++) {
        ChessBoardState next = *state;
        chess_do_move(&next, &list.moves[i]);
        int score = -search(&next, depth - 1, -beta, -alpha);
        if (score > best) best = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }
    return best;
}

int chess_ai_pick_move_medium(const ChessBoardState *state, ChessMove *out) {
    ChessAllMovesList list;
    chess_all_legal_moves(state, &list);
    if (list.count == 0) return 0;

    int best_score = -CHESS_MATE_SCORE - 1;
    int best_count = 0;
    int best_indices[CHESS_ALL_MOVES_MAX];
    const int alpha0 = -CHESS_MATE_SCORE - 1;
    int beta = CHESS_MATE_SCORE + 1;

    for (int i = 0; i < list.count; i++) {
        ChessBoardState next = *state;
        chess_do_move(&next, &list.moves[i]);
        int score = -search(&next, CHESS_MEDIUM_SEARCH_DEPTH - 1, -beta, -alpha0);
        if (score > best_score) {
            best_score = score;
            best_count = 0;
            best_indices[best_count++] = i;
        } else if (score == best_score) {
            best_indices[best_count++] = i;
        }
    }

    int idx = (best_count > 0) ? best_indices[0] : 0;
    if (best_count > 1) {
#if defined(PICO_ON_DEVICE) && defined(LIB_PICO_STDLIB)
        static int seeded = 0;
        if (!seeded) {
            srand((unsigned)(to_us_since_boot(get_absolute_time()) & 0x7FFF));
            seeded = 1;
        }
#endif
        idx = best_indices[rand() % best_count];
    }
    *out = list.moves[idx];
    return 1;
}
