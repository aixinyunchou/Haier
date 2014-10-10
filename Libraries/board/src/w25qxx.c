/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：w25qxx.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：spi flash(w25qxx) 驱动。 
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "w25qxx.h"

#define EN_SELECT()              GPIO_ResetBits(W25QXX_CS_GPIO, W25QXX_CS_PIN) 
#define DS_SELECT()              GPIO_SetBits(W25QXX_CS_GPIO, W25QXX_CS_PIN)

volatile uint32_t SpiFLashID;


static void w25qxx_gpio_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(W25QXX_MISO_CLK | W25QXX_MOSI_CLK | W25QXX_SCK_CLK
                         | W25QXX_CS_CLK, ENABLE);
						 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_PinAFConfig(W25QXX_MISO_GPIO, W25QXX_MISO_PINS, W25QXX_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = W25QXX_MISO_PIN;
  GPIO_Init(W25QXX_MISO_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(W25QXX_MOSI_GPIO, W25QXX_MOSI_PINS, W25QXX_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = W25QXX_MOSI_PIN;
  GPIO_Init(W25QXX_MOSI_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(W25QXX_SCK_GPIO, W25QXX_SCK_PINS, W25QXX_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = W25QXX_SCK_PIN;
  GPIO_Init(W25QXX_SCK_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = W25QXX_CS_PIN;
  GPIO_Init(W25QXX_CS_GPIO, &GPIO_InitStructure);  
}

static void w25qxx_spi_init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  
  RCC_APB1PeriphClockCmd(W25QXX_RCC_CLK, ENABLE);
  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(W25QXX_SPI, &SPI_InitStructure); 
  SPI_Cmd(W25QXX_SPI, ENABLE);		
}

static uint8_t w25qxx_read_write(uint8_t data)
{
  uint8_t redata;
  
  while (!(W25QXX_SPI->SR & (1 << 1)));
  W25QXX_SPI->DR = data;
  while (!(W25QXX_SPI->SR & (1 << 0)));
  redata = W25QXX_SPI->DR;
  return redata;
}

static uint8_t w25qxx_read_status(void)
{
  u8 redata;
	
  EN_SELECT();									
  w25qxx_read_write(0x05);							
  redata = w25qxx_read_write(0xFF);
  DS_SELECT();				
  return redata;	
}

static void w25qxx_wait_busy(void)
{
    while( w25qxx_read_status() & (0x01));
}

static uint32_t w25qxx_readid(void)
{
  uint32_t id = 0;
  uint8_t i;
  
  EN_SELECT();
  w25qxx_read_write(W25QXX_CMD_JEDEC_ID);
  for (i = 0; i < 3; i++)
  {
    id <<= 8;
	id |= w25qxx_read_write(0xFF);
  }  
  DS_SELECT();
  return id;
}

static void w25qxx_write_enable(void)
{
  EN_SELECT();
  w25qxx_read_write(W25QXX_CMD_WREN);
  DS_SELECT();
  while (!(w25qxx_read_status() & 0x02));
}

static void w25qxx_write_disable(void)
{
  EN_SELECT();
  w25qxx_read_write(W25QXX_CMD_WRDI);
  DS_SELECT();
  while (w25qxx_read_status() & 0x02);
}

static void w25qxx_sector_erase(uint32_t add)
{
//  add = add / W25QXX_SECTOR_SIZE;
  w25qxx_write_enable();								
  EN_SELECT();						
  w25qxx_read_write(W25QXX_CMD_ERASE_4K);						
  w25qxx_read_write((uint8_t)(add >> 16));					
  w25qxx_read_write((uint8_t)(add >> 8));	
  w25qxx_read_write(0x00);						
  DS_SELECT();
  w25qxx_wait_busy();						
  w25qxx_write_disable();
			
}

void w25qxx_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
  if (len == 0)return;
   
  EN_SELECT();
  w25qxx_read_write(W25QXX_CMD_READ);						
  w25qxx_read_write((uint8_t)(addr >> 16));					
  w25qxx_read_write((uint8_t)(addr >> 8));	
  w25qxx_read_write((uint8_t)(addr & 0xFF));	
  while (len)
  {
    len--;
    *buf++ = w25qxx_read_write(0x00);	
  }  
  DS_SELECT();  
}

void w25qxx_write(uint32_t addr, uint8_t *buf, uint32_t len)
{
  uint32_t num;
  if (len == 0)return;
  while(len)
  {
    num = W25QXX_PAGE_SIZE;
    if ((addr % W25QXX_SECTOR_SIZE) == 0)
    {
      w25qxx_sector_erase(addr);
    }
    w25qxx_write_enable();
    if (addr & 0xFF) num = addr % W25QXX_PAGE_SIZE;
    if (num > len) num = len;
    len -= num;
    EN_SELECT();
    w25qxx_read_write(W25QXX_CMD_PP);						
    w25qxx_read_write((uint8_t)(addr >> 16));					
    w25qxx_read_write((uint8_t)(addr >> 8));	
    w25qxx_read_write((uint8_t)(addr & 0xFF));
    addr += num;
    while (num--)
    {
	  w25qxx_read_write(*buf++);
    }	
    DS_SELECT();
    w25qxx_wait_busy();
    w25qxx_write_disable();
  }
}

void w25qxx_init(void)
{
  w25qxx_gpio_init();
  w25qxx_spi_init();
  
  w25qxx_readid();
  SpiFLashID = w25qxx_readid();
}
