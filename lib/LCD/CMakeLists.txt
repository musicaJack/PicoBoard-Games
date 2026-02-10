include_directories(../Config)

# 只保留 1.3inch LCD 驱动
add_library(LCD
    LCD_1in3.c
)
target_link_libraries(LCD PUBLIC Config)