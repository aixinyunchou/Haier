/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：at25dfxx.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：spi flash(at25dfxx) 驱动
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "at25dfxx.h"


#define EN_SELECT()              GPIO_ResetBits(AT25DFXX_CS_GPIO, AT25DFXX_CS_PIN) 
#define DS_SELECT()              GPIO_SetBits(AT25DFXX_CS_GPIO, AT25DFXX_CS_PIN)

volatile uint32_t SpiFLashID;  


static void at25dfxx_gpio_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(AT25DFXX_MISO_CLK | AT25DFXX_MOSI_CLK | AT25DFXX_SCK_CLK
                         | AT25DFXX_CS_CLK, ENABLE);
						 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_PinAFConfig(AT25DFXX_MISO_GPIO, AT25DFXX_MISO_PINS, AT25DFXX_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = AT25DFXX_MISO_PIN;
  GPIO_Init(AT25DFXX_MISO_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(AT25DFXX_MOSI_GPIO, AT25DFXX_MOSI_PINS, AT25DFXX_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = AT25DFXX_MOSI_PIN;
  GPIO_Init(AT25DFXX_MOSI_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(AT25DFXX_SCK_GPIO, AT25DFXX_SCK_PINS, AT25DFXX_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = AT25DFXX_SCK_PIN;
  GPIO_Init(AT25DFXX_SCK_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = AT25DFXX_CS_PIN;
  GPIO_Init(AT25DFXX_CS_GPIO, &GPIO_InitStructure);  
}

static void at25dfxx_spi_init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  
  RCC_APB1PeriphClockCmd(AT25DFXX_RCC_CLK, ENABLE);
  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(AT25DFXX_SPI, &SPI_InitStructure); 
  SPI_Cmd(AT25DFXX_SPI, ENABLE);		
}

static uint8_t at25dfxx_read_write(uint8_t data)
{
  uint8_t redata;
  
  while (!(AT25DFXX_SPI->SR & (1 << 1)));
  AT25DFXX_SPI->DR = data;
  while (!(AT25DFXX_SPI->SR & (1 << 0)));
  redata = AT25DFXX_SPI->DR;
  return redata;
}

static uint16_t at25dfxx_read_status(void)
{
  uint16_t tmp;
  EN_SELECT();
  at25dfxx_read_write(AT25DFXX_CMD_READSTATUS);
  tmp = at25dfxx_read_write(0x00);
  tmp <<= 8;
  tmp |= at25dfxx_read_write(0x00);
  DS_SELECT();
  return tmp;
}



static void at25dfxx_write_enable(void)
{
  EN_SELECT();
  at25dfxx_read_write(AT25DFXX_CMD_WRITEENABLE);
  DS_SELECT();
  while(!(at25dfxx_read_status() & AT25DFXX_STATUS_WEL));
}

static void at25dfxx_write_disable(void)
{
  EN_SELECT();
  at25dfxx_read_write(AT25DFXX_CMD_WRITEDISABLE);
  DS_SELECT(); 
}

static void at25dfxx_unprotect(uint32_t addr)
{
  at25dfxx_write_enable();
  EN_SELECT();
  at25dfxx_read_write(0x39);
  at25dfxx_read_write((addr >> 16) & 0xFF);
  at25dfxx_read_write((addr >> 8) & 0xFF);
  at25dfxx_read_write(addr & 0xFF);
  DS_SELECT();  
}

void at25dfxx_sector_erase(uint32_t addr)
{
  addr -= addr % AT25DFXX_BLOCK_SIZE;
  at25dfxx_unprotect(addr);
  at25dfxx_write_enable();
  EN_SELECT();			
  at25dfxx_read_write(AT25DFXX_CMD_BLOCKERASE4K);
  at25dfxx_read_write((addr >> 16) & 0xFF);
  at25dfxx_read_write((addr >> 8) & 0xFF);
  at25dfxx_read_write(addr & 0xFF);	

  DS_SELECT();
  while((at25dfxx_read_status() & AT25DFXX_STATUS_RB));
}

uint32_t at25dfxx_readid(void)
{
  uint32_t i, FlashID = 0;
  
  EN_SELECT();
  at25dfxx_read_write(AT25DFXX_CMD_READID);
  for (i = 0; i < 4; i++) {
    FlashID <<= 8;
    FlashID |= at25dfxx_read_write(0x00);
  }
  DS_SELECT();
  return FlashID;
}

void at25dfxx_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
  EN_SELECT();
  at25dfxx_read_write(AT25DFXX_CMD_READ);
  at25dfxx_read_write((addr >> 16) & 0xFF);
  at25dfxx_read_write((addr >> 8) & 0xFF);
  at25dfxx_read_write(addr & 0xFF);	
  at25dfxx_read_write(0x00);
  
  while (len--) {
    *buf++ = at25dfxx_read_write(0x00);
  }
  DS_SELECT();
}

void at25dfxx_write(uint32_t addr, uint8_t *buf, uint32_t len)
{
  uint32_t write_len;

  while (len) {
    write_len = AT25DFXX_PAGE_SIZE;
    if ((addr % AT25DFXX_BLOCK_SIZE) == 0) {
      at25dfxx_sector_erase(addr);
    }
    at25dfxx_write_enable();
    if (addr & 0xFF) write_len = addr % AT25DFXX_PAGE_SIZE;
    if (write_len > len) {
      write_len = len;
    }
    len -= write_len;
    EN_SELECT();
    at25dfxx_read_write(AT25DFXX_CMD_PAGEPROGRAM);
    at25dfxx_read_write((addr >> 16) & 0xFF);
    at25dfxx_read_write((addr >> 8) & 0xFF);	
    at25dfxx_read_write(addr & 0xFF);
    addr += write_len;
    while (write_len--) {
      at25dfxx_read_write(*buf++);
    }	
    DS_SELECT();
    while((at25dfxx_read_status() & AT25DFXX_STATUS_RB));    
  }
  at25dfxx_write_disable();
}

void at25dfxx_init(void)
{
  at25dfxx_gpio_init();
  at25dfxx_spi_init();
  
  at25dfxx_readid();
  SpiFLashID = at25dfxx_readid();

}

