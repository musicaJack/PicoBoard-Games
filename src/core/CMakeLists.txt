add_library(core input.c render.c)
target_include_directories(core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../drivers/hw
)
target_link_libraries(core PUBLIC Config)
