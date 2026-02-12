/**
 * @file chess_state.c
 */

#include "chess_types.h"
#include "chess_state.h"

void chess_state_init_from_initial(ChessBoardState *b) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            b->board[r][c] = CHESS_INITIAL_BOARD[r][c];
        }
    }
    b->side_to_move = 1;  /* 白先 */
    b->castling[0][0] = b->castling[0][1] = 1;
    b->castling[1][0] = b->castling[1][1] = 1;
    b->ep_col = -1;
}

int8_t chess_state_at(const ChessBoardState *b, int r, int c) {
    return b->board[r][c];
}

int chess_state_is_empty(const ChessBoardState *b, int r, int c) {
    return b->board[r][c] == CHESS_EMPTY;
}

int chess_state_in_bounds(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}
