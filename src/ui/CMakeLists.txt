add_library(ui menu_ui.c tictactoe_ui.c gomoku_ui.c chess_ui.c)
target_include_directories(ui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../drivers/hw
  ${CMAKE_CURRENT_SOURCE_DIR}/../drivers/lcd
)
target_link_libraries(ui PUBLIC game core LCD Config)
