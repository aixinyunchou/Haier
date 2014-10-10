/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：w25qxx.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：spi flash(w25qxx)驱动头文件。 
 ******************************************************************************
 */

#ifndef __W25QXX_H__
#define __W25QXX_H__

#include "stm32f4xx.h"

#define W25QXX_SPI              SPI2

#define W25QXX_GPIO_AF          GPIO_AF_SPI2
#define W25QXX_RCC_CLK          RCC_APB1Periph_SPI2

#define W25QXX_MISO_GPIO        GPIOB
#define W25QXX_MISO_PIN         GPIO_Pin_14
#define W25QXX_MISO_PINS        GPIO_PinSource14
#define W25QXX_MISO_CLK         RCC_AHB1Periph_GPIOB

#define W25QXX_MOSI_GPIO        GPIOB
#define W25QXX_MOSI_PIN         GPIO_Pin_15
#define W25QXX_MOSI_PINS        GPIO_PinSource15
#define W25QXX_MOSI_CLK         RCC_AHB1Periph_GPIOB

#define W25QXX_SCK_GPIO         GPIOD
#define W25QXX_SCK_PIN          GPIO_Pin_3
#define W25QXX_SCK_PINS         GPIO_PinSource3
#define W25QXX_SCK_CLK          RCC_AHB1Periph_GPIOD

#define W25QXX_CS_GPIO          GPIOE
#define W25QXX_CS_PIN           GPIO_Pin_2
#define W25QXX_CS_PINS          GPIO_PinSource2
#define W25QXX_CS_CLK           RCC_AHB1Periph_GPIOE

#define W25QXX_CMD_WRSR         (0x01)  /* Write Status Register */
#define W25QXX_CMD_PP           (0x02)  /* Page Program */
#define W25QXX_CMD_READ         (0x03)  /* Read Data */
#define W25QXX_CMD_WRDI         (0x04)  /* Write Disable */
#define W25QXX_CMD_RDSR1        (0x05)  /* Read Status Register-1 */
#define W25QXX_CMD_WREN         (0x06)  /* Write Enable */
#define W25QXX_CMD_FAST_REA     (0x0B)  /* Fast Read */
#define W25QXX_CMD_ERASE_4K     (0x20)  /* Sector Erase: */
#define W25QXX_CMD_RDSR2        (0x35)  /* Read Status Register-2 */
#define W25QXX_CMD_ERASE_32K    (0x52)  /* 32KB Block Erase */
#define W25QXX_CMD_JEDEC_ID     (0x9F)  /* Read JEDEC ID */
#define W25QXX_CMD_ERASE_full   (0xC7)  /* Chip Erase */
#define W25QXX_CMD_ERASE_64K    (0xD8)  /* 64KB Block Erase */

#define W25QXX_PAGE_SIZE        256
#define W25QXX_SECTOR_SIZE      4096


extern volatile uint32_t SpiFLashID;

extern void w25qxx_init(void);
extern void w25qxx_read(uint32_t addr, uint8_t *buf, uint32_t len);
extern void w25qxx_write(uint32_t addr, uint8_t *buf, uint32_t len);

#endif /* __W25QXX_H__ */
