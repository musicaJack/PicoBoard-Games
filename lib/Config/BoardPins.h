/*****************************************************************************
 * BoardPins.h - Waveshare Pico-LCD-1.3 引脚统一配置
 * 以原厂「引脚排列介绍」为准
 ****************************************************************************/
#ifndef _BOARD_PINS_H_
#define _BOARD_PINS_H_

/* ---------- LCD（绿色） ---------- */
#define LCD_RST_PIN   12   /* GP12 LCD_RST 复位 */
#define LCD_DC_PIN    8    /* GP8  LCD_DC  数据/命令 */
#define LCD_CS_PIN    9    /* GP9  LCD_CS  片选 */
#define LCD_CLK_PIN   10   /* GP10 LCD_CLK SPI 时钟 */
#define LCD_MOSI_PIN  11   /* GP11 LCD_DIN SPI 数据 */
#define LCD_BL_PIN    13   /* GP13 LCD_BL  背光 */

/* ---------- I2C（若外设使用） ---------- */
#define LCD_SCL_PIN   7    /* GP7 */
#define LCD_SDA_PIN   6    /* GP6 */

/* ---------- 摇杆（蓝色） ---------- */
#define PIN_JOY_UP    2    /* GP2  摇杆向上 */
#define PIN_JOY_DOWN  18   /* GP18 摇杆向下 */
#define PIN_JOY_LEFT  16   /* GP16 摇杆向左 */
#define PIN_JOY_RIGHT 20   /* GP20 摇杆向右 */
#define PIN_JOY_CTRL  3    /* GP3  摇杆按下/中间 */

/* ---------- 用户按键（蓝色） ---------- */
#define PIN_BTN_A     15   /* GP15 用户按键 A */
#define PIN_BTN_B     17   /* GP17 用户按键 B */
#define PIN_BTN_X     19   /* GP19 用户按键 X */
#define PIN_BTN_Y     21   /* GP21 用户按键 Y */

#endif
