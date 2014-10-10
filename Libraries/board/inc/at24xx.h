/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：at24xx.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：I2C eeprom(at24xx)驱动头文件。
 ******************************************************************************
 */
#ifndef __AT24XX_H__
#define __AT24XX_H__

#include "stm32f4xx.h"

#define AT24XX_I2C                      I2C1
#define AT24XX_GPIO_AF                  GPIO_AF_I2C1
#define AT24XX_RCC_CLK                  RCC_APB1Periph_I2C1

#define AT24XX_SCL_GPIO                 GPIOB
#define AT24XX_SCL_PIN                  GPIO_Pin_8
#define AT24XX_SCL_PINS                 GPIO_PinSource8
#define AT24XX_SCL_CLK                  RCC_AHB1Periph_GPIOB

#define AT24XX_SDA_GPIO                 GPIOB
#define AT24XX_SDA_PIN                  GPIO_Pin_9
#define AT24XX_SDA_PINS                 GPIO_PinSource9
#define AT24XX_SDA_CLK                  RCC_AHB1Periph_GPIOB

#define AT24XX_ADDR                     0xA0

#define AT24C02                         1
#define AT24C64                         2
#define AT24C256                        3
#define AT24C512                        4

#define AT24XX_CFG                      AT24C64


#if (AT24XX_CFG == AT24C02)
#define AT24XX_PAGESIZE                 8
#define AT24XX_ADDLENGTH                1
#elif (AT24XX_CFG == AT24C64)
#define AT24XX_PAGESIZE                 32
#define AT24XX_ADDLENGTH                2
#elif (AT24XX_CFG == AT24C256)
#define AT24XX_PAGESIZE                 64
#define AT24XX_ADDLENGTH 
#elif (I2CEEPROM_CFG == AT24C512)
#define AT24XX_PAGESIZE                 128
#define AT24XX_ADDLENGTH                2
#else
  #error "eeprom is not selected "
#endif


extern void at24xx_init(void);
extern void at24xx_write(uint32_t addr, uint8_t *buf, uint16_t count);
extern void at24xx_read(uint32_t addr, uint8_t *buf, uint16_t count);
#endif /* __AT24XX_H__ */
