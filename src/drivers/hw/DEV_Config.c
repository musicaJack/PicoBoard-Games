/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   
* | Function    :   Hardware underlying interface
******************************************************************************/
#include "DEV_Config.h"

#define SPI_PORT spi1
#define I2C_PORT spi1

uint slice_num;

void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_put(Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    return gpio_get(Pin);
}

void DEV_SPI_WriteByte(uint8_t Value)
{
    spi_write_blocking(SPI_PORT, &Value, 1);
}

void DEV_SPI_Write_nByte(uint8_t pData[], uint32_t Len)
{
    spi_write_blocking(SPI_PORT, pData, Len);
}

void DEV_I2C_Write(uint8_t addr, uint8_t reg, uint8_t Value)
{
    uint8_t data[2] = {reg, Value};
    i2c_write_blocking(i2c1, addr, data, 2, false);
}

void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len)
{
    i2c_write_blocking(i2c1, addr, pData, Len, false);
}

uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg)
{
    uint8_t buf;
    i2c_write_blocking(i2c1,addr,&reg,1,true);
    i2c_read_blocking(i2c1,addr,&buf,1,false);
    return buf;
}

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_init(Pin);
    if(Mode == 0 || Mode == GPIO_IN) {
        gpio_set_dir(Pin, GPIO_IN);
    } else {
        gpio_set_dir(Pin, GPIO_OUT);
    }
}

void DEV_KEY_Config(UWORD Pin)
{
    gpio_init(Pin);
    gpio_pull_up(Pin);
    gpio_set_dir(Pin, GPIO_IN);
}

void DEV_Delay_ms(UDOUBLE xms)
{
    sleep_ms(xms);
}

void DEV_Delay_us(UDOUBLE xus)
{
    sleep_us(xus);
}

void DEV_GPIO_Init(void)
{
    DEV_GPIO_Mode(LCD_RST_PIN, 1);
    DEV_GPIO_Mode(LCD_DC_PIN, 1);
    DEV_GPIO_Mode(LCD_CS_PIN, 1);
    DEV_GPIO_Mode(LCD_BL_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 1);
    DEV_Digital_Write(LCD_DC_PIN, 0);
    DEV_Digital_Write(LCD_BL_PIN, 1);
}

UBYTE DEV_Module_Init(void)
{
    stdio_init_all();
    spi_init(SPI_PORT, 10000 * 1000);
    gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);
    DEV_GPIO_Init();
    gpio_set_function(LCD_BL_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(LCD_BL_PIN);
    pwm_set_wrap(slice_num, 100);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
    pwm_set_clkdiv(slice_num,50);
    pwm_set_enabled(slice_num, true);
    i2c_init(i2c1,300*1000);
    gpio_set_function(LCD_SDA_PIN,GPIO_FUNC_I2C);
    gpio_set_function(LCD_SCL_PIN,GPIO_FUNC_I2C);
    gpio_pull_up(LCD_SDA_PIN);
    gpio_pull_up(LCD_SCL_PIN);
    printf("DEV_Module_Init OK \r\n");
    return 0;
}

void DEV_SET_PWM(uint8_t Value){
    if(Value<0 || Value >100){
        printf("DEV_SET_PWM Error \r\n");
    }else {
        pwm_set_chan_level(slice_num, PWM_CHAN_B, Value);
    }
}

void DEV_Module_Exit(void)
{
}
