/**
 * @file chess_ai_easy.c
 * @brief 简单 AI：贪心子力评估，等分时随机（完整移植 demo simple_ai.hpp）
 */

#include <stdlib.h>
#include "chess_state.h"
#include "chess_move.h"
#include "chess_result.h"
#include "chess_eval.h"
#include "chess_ai.h"

#if defined(PICO_ON_DEVICE) && defined(LIB_PICO_STDLIB)
#include "pico/stdlib.h"
#include "pico/time.h"
#endif

int chess_ai_pick_move_easy(const ChessBoardState *state, ChessMove *out) {
    ChessAllMovesList list;
    chess_all_legal_moves(state, &list);
    if (list.count == 0) return 0;

    int side = state->side_to_move;
    int best_score = -9999;
    int best_count = 0;
    int best_indices[CHESS_ALL_MOVES_MAX];

    for (int i = 0; i < list.count; i++) {
        int s = chess_eval_after_move(state, &list.moves[i], side);
        if (s > best_score) {
            best_score = s;
            best_count = 0;
            best_indices[best_count++] = i;
        } else if (s == best_score) {
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
