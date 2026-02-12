/**
 * @file chess_eval.c
 */

#include "chess_types.h"
#include "chess_state.h"
#include "chess_move.h"
#include "chess_legal.h"
#include "chess_eval.h"

int chess_piece_value(int8_t piece_index) {
    if (piece_index < 0) return 0;
    ChessPieceType t = chess_piece_index_to_type(piece_index);
    switch (t) {
        case CHESS_PIECE_QUEEN:  return 9;
        case CHESS_PIECE_ROOK:   return 5;
        case CHESS_PIECE_BISHOP:
        case CHESS_PIECE_KNIGHT: return 3;
        case CHESS_PIECE_PAWN:   return 1;
        case CHESS_PIECE_KING:   return 0;
    }
    return 0;
}

int chess_eval_material(const ChessBoardState *b, int side) {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int8_t p = b->board[r][c];
            if (p == CHESS_EMPTY) continue;
            int v = chess_piece_value(p);
            if (chess_is_own_piece(p, side))
                score += v;
            else
                score -= v;
        }
    }
    return score;
}

int chess_eval_after_move(const ChessBoardState *b, const ChessMove *m, int side) {
    ChessBoardState tmp;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            tmp.board[r][c] = b->board[r][c];
    tmp.side_to_move = b->side_to_move;
    tmp.castling[0][0] = b->castling[0][0];
    tmp.castling[0][1] = b->castling[0][1];
    tmp.castling[1][0] = b->castling[1][0];
    tmp.castling[1][1] = b->castling[1][1];
    tmp.ep_col = b->ep_col;
    chess_do_move(&tmp, m);
    return chess_eval_material(&tmp, side);
}
