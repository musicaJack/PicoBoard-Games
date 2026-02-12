/**
 * @file chess_move.c
 */

#include "chess_move.h"

void chess_move_list_clear(ChessMoveList *list) {
    list->count = 0;
}

void chess_move_list_add(ChessMoveList *list, int8_t fr, int8_t fc, int8_t tr, int8_t tc,
                         int8_t prom, int ep, int castle) {
    if (list->count >= CHESS_MOVE_LIST_MAX) return;
    ChessMove *m = &list->moves[list->count++];
    m->from_r = fr;
    m->from_c = fc;
    m->to_r = tr;
    m->to_c = tc;
    m->promote_to = prom;
    m->is_ep = ep;
    m->is_castle = castle;
}

void chess_move_list_add_move(ChessMoveList *list, const ChessMove *m) {
    if (list->count >= CHESS_MOVE_LIST_MAX) return;
    list->moves[list->count++] = *m;
}

void chess_all_moves_clear(ChessAllMovesList *list) {
    list->count = 0;
}

void chess_all_moves_add(ChessAllMovesList *list, const ChessMove *m) {
    if (list->count >= CHESS_ALL_MOVES_MAX) return;
    list->moves[list->count++] = *m;
}
