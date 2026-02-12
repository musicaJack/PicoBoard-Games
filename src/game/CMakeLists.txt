add_library(game
  tictactoe_game.c
  gomoku_game.c
  chess_types.c
  chess_state.c
  chess_move.c
  chess_pseudo.c
  chess_check.c
  chess_legal.c
  chess_result.c
  chess_eval.c
  chess_ai.c
  chess_ai_easy.c
  chess_ai_medium.c
)
target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
