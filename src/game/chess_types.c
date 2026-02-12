/**
 * @file chess_types.c
 */

#include "chess_types.h"

const int8_t CHESS_INITIAL_BOARD[8][8] = {
    { 5,  2,  0,  4,  1,  0,  2,  5 },  /* 黑车马象后王象马车 */
    { 3,  3,  3,  3,  3,  3,  3,  3 },  /* 黑兵 */
    { -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9,  9,  9,  9,  9,  9,  9,  9 },  /* 白兵 */
    { 11, 8,  6,  10, 7,  6,  8,  11 }, /* 白车马象后王象马车 */
};

static const ChessPieceType s_index_to_type[] = {
    CHESS_PIECE_BISHOP, CHESS_PIECE_KING, CHESS_PIECE_KNIGHT, CHESS_PIECE_PAWN,
    CHESS_PIECE_QUEEN, CHESS_PIECE_ROOK,
    CHESS_PIECE_BISHOP, CHESS_PIECE_KING, CHESS_PIECE_KNIGHT, CHESS_PIECE_PAWN,
    CHESS_PIECE_QUEEN, CHESS_PIECE_ROOK
};

ChessPieceType chess_piece_index_to_type(int8_t index) {
    if (index < 0 || index > 11) return CHESS_PIECE_PAWN;
    return s_index_to_type[index];
}

ChessPieceColor chess_piece_index_to_color(int8_t index) {
    if (index < 0 || index > 11) return CHESS_COLOR_BLACK;
    return (ChessPieceColor)(index < 6 ? 0 : 1);
}

int chess_is_own_piece(int8_t index, int side) {
    if (index < 0) return 0;
    return (side == 0 && index < 6) || (side == 1 && index >= 6);
}

int chess_is_opponent_piece(int8_t index, int side) {
    if (index < 0) return 0;
    return (side == 0 && index >= 6) || (side == 1 && index < 6);
}
