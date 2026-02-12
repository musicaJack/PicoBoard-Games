/**
 * @file chess_result.c
 */

#include "chess_types.h"
#include "chess_state.h"
#include "chess_move.h"
#include "chess_check.h"
#include "chess_legal.h"
#include "chess_result.h"

int chess_has_any_legal_move(const ChessBoardState *b) {
    ChessMoveList list;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (b->board[r][c] == CHESS_EMPTY || !chess_is_own_piece(b->board[r][c], b->side_to_move))
                continue;
            chess_legal_moves_from(b, r, c, &list);
            if (list.count > 0) return 1;
        }
    }
    return 0;
}

int chess_get_game_result(const ChessBoardState *b) {
    if (chess_has_any_legal_move(b)) return 0;
    if (chess_is_king_in_check(b, b->side_to_move))
        return (b->side_to_move == 0 ? 1 : 2);
    return 3;
}

void chess_all_legal_moves(const ChessBoardState *b, ChessAllMovesList *out) {
    chess_all_moves_clear(out);
    ChessMoveList piece_list;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (b->board[r][c] == CHESS_EMPTY || !chess_is_own_piece(b->board[r][c], b->side_to_move))
                continue;
            chess_legal_moves_from(b, r, c, &piece_list);
            for (int i = 0; i < piece_list.count; i++)
                chess_all_moves_add(out, &piece_list.moves[i]);
        }
    }
}
