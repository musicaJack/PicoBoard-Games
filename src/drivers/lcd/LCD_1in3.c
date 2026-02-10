/*****************************************************************************
* LCD_1in3 driver
******************************************************************************/
#include "LCD_1in3.h"

#include <stdlib.h>
#include <stdio.h>

LCD_1IN3_ATTRIBUTES LCD_1IN3;

static void LCD_1IN3_Reset(void)
{
    DEV_Digital_Write(LCD_RST_PIN, 1);
    DEV_Delay_ms(100);
    DEV_Digital_Write(LCD_RST_PIN, 0);
    DEV_Delay_ms(100);
    DEV_Digital_Write(LCD_RST_PIN, 1);
    DEV_Delay_ms(100);
}

static void LCD_1IN3_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(LCD_DC_PIN, 0);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

static void LCD_1IN3_SendData_8Bit(UBYTE Data)
{
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

static void LCD_1IN3_SendData_16Bit(UWORD Data)
{
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_SPI_WriteByte((Data >> 8) & 0xFF);
    DEV_SPI_WriteByte(Data & 0xFF);
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

static void LCD_1IN3_InitReg(void)
{
    LCD_1IN3_SendCommand(0x3A);
    LCD_1IN3_SendData_8Bit(0x05);
    LCD_1IN3_SendCommand(0xB2);
    LCD_1IN3_SendData_8Bit(0x0C);
    LCD_1IN3_SendData_8Bit(0x0C);
    LCD_1IN3_SendData_8Bit(0x00);
    LCD_1IN3_SendData_8Bit(0x33);
    LCD_1IN3_SendData_8Bit(0x33);
    LCD_1IN3_SendCommand(0xB7);
    LCD_1IN3_SendData_8Bit(0x35);
    LCD_1IN3_SendCommand(0xBB);
    LCD_1IN3_SendData_8Bit(0x19);
    LCD_1IN3_SendCommand(0xC0);
    LCD_1IN3_SendData_8Bit(0x2C);
    LCD_1IN3_SendCommand(0xC2);
    LCD_1IN3_SendData_8Bit(0x01);
    LCD_1IN3_SendCommand(0xC3);
    LCD_1IN3_SendData_8Bit(0x12);
    LCD_1IN3_SendCommand(0xC4);
    LCD_1IN3_SendData_8Bit(0x20);
    LCD_1IN3_SendCommand(0xC6);
    LCD_1IN3_SendData_8Bit(0x0F);
    LCD_1IN3_SendCommand(0xD0);
    LCD_1IN3_SendData_8Bit(0xA4);
    LCD_1IN3_SendData_8Bit(0xA1);
    LCD_1IN3_SendCommand(0xE0);
    LCD_1IN3_SendData_8Bit(0xD0);
    LCD_1IN3_SendData_8Bit(0x04);
    LCD_1IN3_SendData_8Bit(0x0D);
    LCD_1IN3_SendData_8Bit(0x11);
    LCD_1IN3_SendData_8Bit(0x13);
    LCD_1IN3_SendData_8Bit(0x2B);
    LCD_1IN3_SendData_8Bit(0x3F);
    LCD_1IN3_SendData_8Bit(0x54);
    LCD_1IN3_SendData_8Bit(0x4C);
    LCD_1IN3_SendData_8Bit(0x18);
    LCD_1IN3_SendData_8Bit(0x0D);
    LCD_1IN3_SendData_8Bit(0x0B);
    LCD_1IN3_SendData_8Bit(0x1F);
    LCD_1IN3_SendData_8Bit(0x23);
    LCD_1IN3_SendCommand(0xE1);
    LCD_1IN3_SendData_8Bit(0xD0);
    LCD_1IN3_SendData_8Bit(0x04);
    LCD_1IN3_SendData_8Bit(0x0C);
    LCD_1IN3_SendData_8Bit(0x11);
    LCD_1IN3_SendData_8Bit(0x13);
    LCD_1IN3_SendData_8Bit(0x2C);
    LCD_1IN3_SendData_8Bit(0x3F);
    LCD_1IN3_SendData_8Bit(0x44);
    LCD_1IN3_SendData_8Bit(0x51);
    LCD_1IN3_SendData_8Bit(0x2F);
    LCD_1IN3_SendData_8Bit(0x1F);
    LCD_1IN3_SendData_8Bit(0x1F);
    LCD_1IN3_SendData_8Bit(0x20);
    LCD_1IN3_SendData_8Bit(0x23);
    LCD_1IN3_SendCommand(0x21);
    LCD_1IN3_SendCommand(0x11);
    LCD_1IN3_SendCommand(0x29);
}

static void LCD_1IN3_SetAttributes(UBYTE Scan_dir)
{
    LCD_1IN3.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x00;
    if(Scan_dir == HORIZONTAL) {
        LCD_1IN3.HEIGHT = LCD_1IN3_WIDTH;
        LCD_1IN3.WIDTH   = LCD_1IN3_HEIGHT;
        MemoryAccessReg = 0X70;
    } else {
        LCD_1IN3.HEIGHT = LCD_1IN3_HEIGHT;
        LCD_1IN3.WIDTH   = LCD_1IN3_WIDTH;
        MemoryAccessReg = 0X00;
    }
    LCD_1IN3_SendCommand(0x36);
    LCD_1IN3_SendData_8Bit(MemoryAccessReg);
}

void LCD_1IN3_Init(UBYTE Scan_dir)
{
    DEV_SET_PWM(90);
    LCD_1IN3_Reset();
    LCD_1IN3_SetAttributes(Scan_dir);
    LCD_1IN3_InitReg();
}

void LCD_1IN3_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    LCD_1IN3_SendCommand(0x2A);
    LCD_1IN3_SendData_8Bit(0x00);
    LCD_1IN3_SendData_8Bit(Xstart);
    LCD_1IN3_SendData_8Bit(0x00);
    LCD_1IN3_SendData_8Bit(Xend-1);
    LCD_1IN3_SendCommand(0x2B);
    LCD_1IN3_SendData_8Bit(0x00);
    LCD_1IN3_SendData_8Bit(Ystart);
    LCD_1IN3_SendData_8Bit(0x00);
    LCD_1IN3_SendData_8Bit(Yend-1);
    LCD_1IN3_SendCommand(0X2C);
}

void LCD_1IN3_Clear(UWORD Color)
{
    UWORD j, i;
    UWORD line[LCD_1IN3_WIDTH];
    Color = (UWORD)(((Color << 8) & 0xff00) | (Color >> 8));
    for (i = 0; i < LCD_1IN3.WIDTH; i++) {
        line[i] = Color;
    }
    LCD_1IN3_SetWindows(0, 0, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT);
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    for (j = 0; j < LCD_1IN3.HEIGHT; j++) {
        DEV_SPI_Write_nByte((uint8_t *)line, LCD_1IN3.WIDTH * 2);
    }
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

void LCD_1IN3_Display(UWORD *Image)
{
    UWORD j;
    LCD_1IN3_SetWindows(0, 0, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT);
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    for (j = 0; j < LCD_1IN3.HEIGHT; j++) {
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN3.WIDTH], LCD_1IN3.WIDTH*2);
    }
    DEV_Digital_Write(LCD_CS_PIN, 1);
    LCD_1IN3_SendCommand(0x29);
}

void LCD_1IN3_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    UDOUBLE Addr = 0;
    UWORD j;
    LCD_1IN3_SetWindows(Xstart, Ystart, Xend , Yend);
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    for (j = Ystart; j < Yend - 1; j++) {
        Addr = Xstart + j * LCD_1IN3.WIDTH ;
        DEV_SPI_Write_nByte((uint8_t *)&Image[Addr], (Xend-Xstart)*2);
    }
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

void LCD_1IN3_DisplayPoint(UWORD X, UWORD Y, UWORD Color)
{
    LCD_1IN3_SetWindows(X,Y,X,Y);
    LCD_1IN3_SendData_16Bit(Color);
}

void Handler_1IN3_LCD(int signo)
{
    (void)signo;
    printf("\r\nHandler:Program stop\r\n");
    DEV_Module_Exit();
    exit(0);
}
