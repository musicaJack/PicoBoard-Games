add_library(LCD LCD_1in3.c)
target_include_directories(LCD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../hw)
target_link_libraries(LCD PUBLIC Config)
