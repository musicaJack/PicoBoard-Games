add_library(game tictactoe_game.c gomoku_game.c)
target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
