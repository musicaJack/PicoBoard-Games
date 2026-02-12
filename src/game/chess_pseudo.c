/**
 * @file chess_pseudo.c
 * @brief 伪合法走法实现；兵升变只生成升后（与 CHESS_TODO 约定一致）
 */

#include "chess_types.h"
#include "chess_state.h"
#include "chess_move.h"
#include "chess_pseudo.h"

static const int KING_DR[] = { -1, -1, -1,  0,  0,  1,  1,  1 };
static const int KING_DC[] = { -1,  0,  1, -1,  1, -1,  0,  1 };
static const int KNIGHT_DR[] = { -2, -2, -1, -1,  1,  1,  2,  2 };
static const int KNIGHT_DC[] = { -1,  1, -2,  2, -2,  2, -1,  1 };

/* 升变仅后：白后=10，黑后=4 */
static const int8_t PROMO_QUEEN_WHITE = 10;
static const int8_t PROMO_QUEEN_BLACK = 4;

static void ray_moves(const ChessBoardState *b, int r, int c, int dr, int dc, ChessMoveList *out) {
    int side = b->side_to_move;
    int nr = r + dr, nc = c + dc;
    while (chess_state_in_bounds(nr, nc)) {
        int8_t dst = b->board[nr][nc];
        if (dst != CHESS_EMPTY) {
            if (chess_is_opponent_piece(dst, side))
                chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)nc,
                                    CHESS_PROMOTE_NONE, 0, 0);
            break;
        }
        chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)nc,
                            CHESS_PROMOTE_NONE, 0, 0);
        nr += dr;
        nc += dc;
    }
}

void chess_pseudo_king(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    int side = b->side_to_move;
    for (int i = 0; i < 8; i++) {
        int nr = r + KING_DR[i], nc = c + KING_DC[i];
        if (!chess_state_in_bounds(nr, nc)) continue;
        int8_t dst = b->board[nr][nc];
        if (dst != CHESS_EMPTY && chess_is_own_piece(dst, side)) continue;
        chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)nc,
                            CHESS_PROMOTE_NONE, 0, 0);
    }
    if (c != 4) return;
    if (b->castling[side][1]) {
        if (b->board[r][5] == CHESS_EMPTY && b->board[r][6] == CHESS_EMPTY)
            chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)r, (int8_t)6,
                               CHESS_PROMOTE_NONE, 0, 1);
    }
    if (b->castling[side][0]) {
        if (b->board[r][1] == CHESS_EMPTY && b->board[r][2] == CHESS_EMPTY && b->board[r][3] == CHESS_EMPTY)
            chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)r, (int8_t)2,
                               CHESS_PROMOTE_NONE, 0, 1);
    }
}

void chess_pseudo_rook(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    ray_moves(b, r, c, -1,  0, out);
    ray_moves(b, r, c,  1,  0, out);
    ray_moves(b, r, c,  0, -1, out);
    ray_moves(b, r, c,  0,  1, out);
}

void chess_pseudo_bishop(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    ray_moves(b, r, c, -1, -1, out);
    ray_moves(b, r, c, -1,  1, out);
    ray_moves(b, r, c,  1, -1, out);
    ray_moves(b, r, c,  1,  1, out);
}

void chess_pseudo_queen(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    chess_pseudo_rook(b, r, c, out);
    chess_pseudo_bishop(b, r, c, out);
}

void chess_pseudo_knight(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    int side = b->side_to_move;
    for (int i = 0; i < 8; i++) {
        int nr = r + KNIGHT_DR[i], nc = c + KNIGHT_DC[i];
        if (!chess_state_in_bounds(nr, nc)) continue;
        int8_t dst = b->board[nr][nc];
        if (dst != CHESS_EMPTY && chess_is_own_piece(dst, side)) continue;
        chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)nc,
                            CHESS_PROMOTE_NONE, 0, 0);
    }
}

void chess_pseudo_pawn(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    int side = b->side_to_move;
    int step = (side == 1) ? -1 : 1;
    int start_row = (side == 1) ? 6 : 1;
    int nr = r + step;
    if (!chess_state_in_bounds(nr, c)) return;
    int promo = ((side == 1 && nr == 0) || (side == 0 && nr == 7)) ? 1 : 0;
    int8_t prom_piece = (side == 1) ? PROMO_QUEEN_WHITE : PROMO_QUEEN_BLACK;

    if (b->board[nr][c] == CHESS_EMPTY) {
        if (promo) {
            chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)c, prom_piece, 0, 0);
        } else {
            chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)c,
                               CHESS_PROMOTE_NONE, 0, 0);
        }
        if (r == start_row) {
            int nr2 = r + 2 * step;
            if (chess_state_in_bounds(nr2, c) && b->board[nr2][c] == CHESS_EMPTY)
                chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr2, (int8_t)c,
                                   CHESS_PROMOTE_NONE, 0, 0);
        }
    }
    for (int d = 0; d < 2; d++) {
        int dc = (d == 0) ? -1 : 1;
        int nc = c + dc;
        if (!chess_state_in_bounds(nr, nc)) continue;
        if (b->board[nr][nc] != CHESS_EMPTY && chess_is_opponent_piece(b->board[nr][nc], side)) {
            if (promo)
                chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)nc, prom_piece, 0, 0);
            else
                chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)nr, (int8_t)nc,
                                   CHESS_PROMOTE_NONE, 0, 0);
        }
    }
    /* 吃过路兵 */
    if (b->ep_col >= 0 && (side == 1 ? r == 3 : r == 4) && (c == b->ep_col - 1 || c == b->ep_col + 1)) {
        int to_c = b->ep_col;
        int to_r = (side == 1) ? 2 : 5;
        chess_move_list_add(out, (int8_t)r, (int8_t)c, (int8_t)to_r, (int8_t)to_c,
                            CHESS_PROMOTE_NONE, 1, 0);
    }
}

void chess_pseudo_moves_from(const ChessBoardState *b, int r, int c, ChessMoveList *out) {
    chess_move_list_clear(out);
    if (!chess_state_in_bounds(r, c) || b->board[r][c] == CHESS_EMPTY) return;
    ChessPieceType t = chess_piece_index_to_type(b->board[r][c]);
    switch (t) {
        case CHESS_PIECE_KING:   chess_pseudo_king(b, r, c, out); break;
        case CHESS_PIECE_QUEEN:  chess_pseudo_queen(b, r, c, out); break;
        case CHESS_PIECE_ROOK:   chess_pseudo_rook(b, r, c, out); break;
        case CHESS_PIECE_BISHOP: chess_pseudo_bishop(b, r, c, out); break;
        case CHESS_PIECE_KNIGHT: chess_pseudo_knight(b, r, c, out); break;
        case CHESS_PIECE_PAWN:   chess_pseudo_pawn(b, r, c, out); break;
    }
}
