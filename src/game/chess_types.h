/**
 * @file chess_types.h
 * @brief 棋子类型、颜色、常量与索引映射（与 demo types.hpp 一致）
 */

#ifndef PICO_CODE_CHESS_TYPES_H
#define PICO_CODE_CHESS_TYPES_H

#include <stdint.h>

/* 棋子类型：用于走法生成 */
typedef enum {
    CHESS_PIECE_KING = 0,
    CHESS_PIECE_QUEEN,
    CHESS_PIECE_ROOK,
    CHESS_PIECE_BISHOP,
    CHESS_PIECE_KNIGHT,
    CHESS_PIECE_PAWN
} ChessPieceType;

/* 颜色：0=黑 1=白 */
typedef enum {
    CHESS_COLOR_BLACK = 0,
    CHESS_COLOR_WHITE = 1
} ChessPieceColor;

/* 棋盘格：棋子索引 0..11（与 chess_pieces 一致），-1 为空 */
#define CHESS_EMPTY ((int8_t)(-1))

/* 开局棋盘（行 0=黑方底线，行 7=白方底线；索引同 chess_pieces.h） */
extern const int8_t CHESS_INITIAL_BOARD[8][8];

/* 棋子索引 → 类型（0→Bishop, 1→King, 2→Knight, 3→Pawn, 4→Queen, 5→Rook；6..11 同序） */
ChessPieceType chess_piece_index_to_type(int8_t index);

/* 棋子索引 → 颜色（0..5 黑，6..11 白） */
ChessPieceColor chess_piece_index_to_color(int8_t index);

/* 是否为己方子（index 与 side 同色） */
int chess_is_own_piece(int8_t index, int side);

/* 是否为对方子 */
int chess_is_opponent_piece(int8_t index, int side);

#endif /* PICO_CODE_CHESS_TYPES_H */
