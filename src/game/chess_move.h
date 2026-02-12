/**
 * @file chess_move.h
 * @brief 着法表示：ChessMove、ChessMoveList、ChessAllMovesList
 */

#ifndef PICO_CODE_CHESS_MOVE_H
#define PICO_CODE_CHESS_MOVE_H

#include <stdint.h>

/* 升变目标：无=-1，否则为棋子索引（后/车/马/象）；第一版一律升后 */
#define CHESS_PROMOTE_NONE ((int8_t)(-1))

/** 走法：源格 + 目标格 + 特殊标记（易位/吃过路兵/升变） */
typedef struct {
    int8_t from_r, from_c, to_r, to_c;
    int8_t promote_to;  /* CHESS_PROMOTE_NONE 或 升变后棋子索引 */
    int is_ep;           /* 是否吃过路兵 */
    int is_castle;       /* 是否易位 */
} ChessMove;

#define CHESS_MOVE_LIST_MAX 32
/** 伪合法/合法走法列表：单子最多 32 个 */
typedef struct {
    ChessMove moves[CHESS_MOVE_LIST_MAX];
    int count;
} ChessMoveList;

#define CHESS_ALL_MOVES_MAX 128
/** 全局面合法走法列表（用于 AI） */
typedef struct {
    ChessMove moves[CHESS_ALL_MOVES_MAX];
    int count;
} ChessAllMovesList;

void chess_move_list_clear(ChessMoveList *list);
void chess_move_list_add(ChessMoveList *list, int8_t fr, int8_t fc, int8_t tr, int8_t tc,
                         int8_t prom, int ep, int castle);
void chess_move_list_add_move(ChessMoveList *list, const ChessMove *m);

void chess_all_moves_clear(ChessAllMovesList *list);
void chess_all_moves_add(ChessAllMovesList *list, const ChessMove *m);

#endif /* PICO_CODE_CHESS_MOVE_H */
