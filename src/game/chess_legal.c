/**
 * @file chess_legal.c
 */

#include "chess_types.h"
#include "chess_state.h"
#include "chess_move.h"
#include "chess_pseudo.h"
#include "chess_check.h"
#include "chess_legal.h"

void chess_apply_move(int8_t board[8][8], const ChessMove *m) {
    int8_t piece = board[m->from_r][m->from_c];
    if (m->is_castle) {
        int r = m->from_r;
        board[m->from_r][m->from_c] = CHESS_EMPTY;
        board[m->to_r][m->to_c] = piece;
        if (m->to_c == 6) {
            board[r][5] = board[r][7];
            board[r][7] = CHESS_EMPTY;
        } else {
            board[r][3] = board[r][0];
            board[r][0] = CHESS_EMPTY;
        }
        return;
    }
    if (m->is_ep) {
        board[m->from_r][m->to_c] = CHESS_EMPTY;
    }
    board[m->to_r][m->to_c] = (m->promote_to >= 0) ? m->promote_to : piece;
    board[m->from_r][m->from_c] = CHESS_EMPTY;
}

void chess_legal_moves_from(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    chess_move_list_clear(out);
    if (!chess_state_in_bounds(r, c) || b->board[r][c] == CHESS_EMPTY) return;
    if (!chess_is_own_piece(b->board[r][c], b->side_to_move)) return;

    ChessMoveList pseudo;
    chess_pseudo_moves_from(b, r, c, &pseudo);
    for (int i = 0; i < pseudo.count; i++) {
        const ChessMove *pm = &pseudo.moves[i];
        if (pm->is_castle) {
            int mid_c = (pm->from_c + pm->to_c) / 2;
            if (chess_is_square_attacked(b, pm->from_r, mid_c, 1 - b->side_to_move))
                continue;
        }
        int8_t copy[8][8];
        for (int ri = 0; ri < 8; ri++)
            for (int ci = 0; ci < 8; ci++)
                copy[ri][ci] = b->board[ri][ci];
        chess_apply_move(copy, pm);
        ChessBoardState after;
        for (int ri = 0; ri < 8; ri++)
            for (int ci = 0; ci < 8; ci++)
                after.board[ri][ci] = copy[ri][ci];
        after.side_to_move = b->side_to_move;
        after.castling[0][0] = after.castling[0][1] = after.castling[1][0] = after.castling[1][1] = 0;
        after.ep_col = -1;
        if (!chess_is_king_in_check(&after, b->side_to_move))
            chess_move_list_add_move(out, pm);
    }
}

void chess_do_move(ChessBoardState *state, const ChessMove *m) {
    int side = state->side_to_move;
    int8_t piece = state->board[m->from_r][m->from_c];
    ChessPieceType pt = chess_piece_index_to_type(piece);

    if (pt == CHESS_PIECE_KING) {
        state->castling[side][0] = state->castling[side][1] = 0;
    } else if (pt == CHESS_PIECE_ROOK) {
        if (m->from_r == 0 && m->from_c == 0) state->castling[0][0] = 0;
        if (m->from_r == 0 && m->from_c == 7) state->castling[0][1] = 0;
        if (m->from_r == 7 && m->from_c == 0) state->castling[1][0] = 0;
        if (m->from_r == 7 && m->from_c == 7) state->castling[1][1] = 0;
    }
    if (state->board[m->to_r][m->to_c] != CHESS_EMPTY || m->is_ep) {
        if (m->to_r == 0 && m->to_c == 0) state->castling[0][0] = 0;
        if (m->to_r == 0 && m->to_c == 7) state->castling[0][1] = 0;
        if (m->to_r == 7 && m->to_c == 0) state->castling[1][0] = 0;
        if (m->to_r == 7 && m->to_c == 7) state->castling[1][1] = 0;
    }
    if (pt == CHESS_PIECE_PAWN && (m->from_r - m->to_r) * (m->from_r - m->to_r) == 4) {
        state->ep_col = (int)m->from_c;
    } else {
        state->ep_col = -1;
    }
    chess_apply_move(state->board, m);
    state->side_to_move = 1 - state->side_to_move;
}
