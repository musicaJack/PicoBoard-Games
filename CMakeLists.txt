cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(Pico_ePaper_Code)
pico_sdk_init()

add_subdirectory(src/drivers)
add_subdirectory(src/core)
add_subdirectory(src/game)
add_subdirectory(src/ui)

add_executable(main src/app/app_main.c)
target_include_directories(main PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/drivers/hw
  ${CMAKE_SOURCE_DIR}/src/drivers/lcd
)
target_link_libraries(main ui game core LCD Config pico_stdlib)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)
pico_add_extra_outputs(main)