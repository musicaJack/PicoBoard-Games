/**
 * @file chess_check.c
 */

#include "chess_types.h"
#include "chess_state.h"
#include "chess_move.h"
#include "chess_pseudo.h"
#include "chess_check.h"

int chess_find_king(const int8_t board[8][8], int side, int *out_r, int *out_c) {
    int8_t king_idx = (side == 0) ? 1 : 7;
    *out_r = *out_c = -1;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (board[r][c] == king_idx) {
                *out_r = r;
                *out_c = c;
                return 1;
            }
        }
    }
    return 0;
}

int chess_is_square_attacked(const ChessBoardState *b, int r, int c, int by_side) {
    ChessBoardState tmp;
    for (int ri = 0; ri < 8; ri++)
        for (int ci = 0; ci < 8; ci++)
            tmp.board[ri][ci] = b->board[ri][ci];
    tmp.side_to_move = by_side;
    tmp.castling[0][0] = tmp.castling[0][1] = tmp.castling[1][0] = tmp.castling[1][1] = 0;
    tmp.ep_col = -1;

    ChessMoveList list;
    for (int ri = 0; ri < 8; ri++) {
        for (int ci = 0; ci < 8; ci++) {
            if (tmp.board[ri][ci] == CHESS_EMPTY || !chess_is_own_piece(tmp.board[ri][ci], by_side))
                continue;
            chess_pseudo_moves_from(&tmp, ri, ci, &list);
            for (int i = 0; i < list.count; i++) {
                if (list.moves[i].to_r == r && list.moves[i].to_c == c)
                    return 1;
            }
        }
    }
    return 0;
}

int chess_is_king_in_check(const ChessBoardState *b, int side) {
    int kr, kc;
    if (!chess_find_king(b->board, side, &kr, &kc)) return 0;
    return chess_is_square_attacked(b, kr, kc, 1 - side);
}
