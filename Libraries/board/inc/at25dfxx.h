/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：at25dfxx.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：spi flash(at25dfxx)驱动头文件。 
 ******************************************************************************
 */
#ifndef __AT25DFXX_H__
#define __AT25DFXX_H__

#include "stm32f4xx.h"

#define AT25DFXX_SPI                      SPI2

#define AT25DFXX_GPIO_AF                  GPIO_AF_SPI2
#define AT25DFXX_RCC_CLK                  RCC_APB1Periph_SPI2

#define AT25DFXX_MISO_GPIO                GPIOB
#define AT25DFXX_MISO_PIN                 GPIO_Pin_14
#define AT25DFXX_MISO_PINS                GPIO_PinSource14
#define AT25DFXX_MISO_CLK                 RCC_AHB1Periph_GPIOB

#define AT25DFXX_MOSI_GPIO                GPIOB
#define AT25DFXX_MOSI_PIN                 GPIO_Pin_15
#define AT25DFXX_MOSI_PINS                GPIO_PinSource15
#define AT25DFXX_MOSI_CLK                 RCC_AHB1Periph_GPIOB

#define AT25DFXX_SCK_GPIO                 GPIOD
#define AT25DFXX_SCK_PIN                  GPIO_Pin_3
#define AT25DFXX_SCK_PINS                 GPIO_PinSource3
#define AT25DFXX_SCK_CLK                  RCC_AHB1Periph_GPIOD

#define AT25DFXX_CS_GPIO                  GPIOE
#define AT25DFXX_CS_PIN                   GPIO_Pin_2
#define AT25DFXX_CS_PINS                  GPIO_PinSource2
#define AT25DFXX_CS_CLK                   RCC_AHB1Periph_GPIOE


#define AT25DFXX_CMD_WRITEDISABLE         0x04
#define AT25DFXX_CMD_READSTATUS           0x05
#define AT25DFXX_CMD_WRITEENABLE          0x06
#define AT25DFXX_CMD_READ                 0x0B
#define AT25DFXX_CMD_PAGEPROGRAM          0x02
#define AT25DFXX_CMD_READID               0x9F
#define AT25DFXX_CMD_BLOCKERASE4K         0x20
#define AT25DFXX_CMD_RREADPROTECT         0x3C
#define AT25DFXX_CMD_PROTECT              0x36
#define AT25DFXX_CMD_UNPROTECT            0x39


#define AT25DFXX_STATUS_WEL               ((uint32_t)1 << 9)
#define AT25DFXX_STATUS_RB                ((uint32_t)1 << 0)


#define AT25DFXX_PAGE_SIZE                256
#define AT25DFXX_BLOCK_SIZE               4096


extern void at25dfxx_init(void);
extern void at25dfxx_write(uint32_t addr, uint8_t *buf, uint32_t len);
extern void at25dfxx_read(uint32_t addr, uint8_t *buf, uint32_t len);
#endif /* __AT25DFXX_H__ */
