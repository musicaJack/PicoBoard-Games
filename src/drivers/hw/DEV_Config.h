/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   
* | Function    :   Hardware underlying interface
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "stdio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#include "BoardPins.h"

void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_KEY_Config(UWORD Pin);
void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);
void DEV_Delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);
void DEV_I2C_Write(uint8_t addr, uint8_t reg, uint8_t Value);
void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len);
uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg);
void DEV_SET_PWM(uint8_t Value);
UBYTE DEV_Module_Init(void);
void DEV_Module_Exit(void);

#endif
