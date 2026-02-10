#ifndef __LCD_1IN3_H
#define __LCD_1IN3_H

#include "DEV_Config.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define LCD_1IN3_HEIGHT 240
#define LCD_1IN3_WIDTH 240
#define HORIZONTAL 0
#define VERTICAL   1
#define LCD_1IN3_SetBacklight(Value) ;

typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN3_ATTRIBUTES;
extern LCD_1IN3_ATTRIBUTES LCD_1IN3;

void LCD_1IN3_Init(UBYTE Scan_dir);
void LCD_1IN3_Clear(UWORD Color);
void LCD_1IN3_Display(UWORD *Image);
void LCD_1IN3_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN3_DisplayPoint(UWORD X, UWORD Y, UWORD Color);
void Handler_1IN3_LCD(int signo);
#endif
