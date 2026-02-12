#include "ui/menu_ui.h"
#include "ui/tictactoe_ui.h"
#include "ui/gomoku_ui.h"
#include "ui/chess_ui.h"
#include "DEV_Config.h"
#include "LCD_1in3.h"

int main(void)
{
    DEV_Delay_ms(100);
    if (DEV_Module_Init() != 0)
        while (1) DEV_Delay_ms(1000);

    DEV_SET_PWM(50);
    LCD_1IN3_Init(HORIZONTAL);
    LCD_1IN3_Clear(0x0000);

    while (1) {
        int choice = menu_run();
        if (choice == 1)
            tictactoe_run();
        else if (choice == 2)
            gomoku_run();
        else if (choice == 3)
            chess_run();
    }
}
