/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：touch.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：lcd触摸驱动。 
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "touch.h"

#define TOUCH_MISO_GPIO                   GPIOB              
#define TOUCH_MISO_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define TOUCH_MISO_GPIO_PIN               GPIO_Pin_14
#define TOUCH_MISO_GPIO_SOURCE            GPIO_PinSource14

#define TOUCH_MOSI_GPIO                   GPIOB
#define TOUCH_MOSI_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define TOUCH_MOSI_GPIO_PIN               GPIO_Pin_15
#define TOUCH_MOSI_GPIO_SOURCE            GPIO_PinSource15

#define TOUCH_SCK_GPIO                    GPIOD
#define TOUCH_SCK_GPIO_CLK                RCC_AHB1Periph_GPIOD
#define TOUCH_SCK_GPIO_PIN                GPIO_Pin_3
#define TOUCH_SCK_GPIO_SOURCE             GPIO_PinSource3

#define TOUCH_CS_GPIO                     GPIOC
#define TOUCH_CS_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define TOUCH_CS_GPIO_PIN                 GPIO_Pin_13
#define TOUCH_CS_GPIO_SOURCE              GPIO_PinSource13

#define TOUCH_BUSY_GPIO                   GPIOI
#define TOUCH_BUSY_GPIO_CLK               RCC_AHB1Periph_GPIOI
#define TOUCH_BUSY_GPIO_PIN               GPIO_Pin_11


#define TOUCH_GPIO_AF                     GPIO_AF_SPI2
#define TOUCH_RCC_CLK                     RCC_APB1Periph_SPI2

#define TOUCH_SPI                         SPI2

#define  TOUCH_CMD_X                      0xD0
#define  TOUCH_CMD_Y                      0x90


#define EN_SELECT()                       GPIO_ResetBits(TOUCH_CS_GPIO, TOUCH_CS_GPIO_PIN) 
#define DS_SELECT()                       GPIO_SetBits(TOUCH_CS_GPIO, TOUCH_CS_GPIO_PIN)

static void Delay(void)
{
  uint32_t i;
  
  for (i = 0; i < 0x200; i++);
}

static void Touch_Gpio_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(TOUCH_MISO_GPIO_CLK | TOUCH_MOSI_GPIO_CLK | TOUCH_SCK_GPIO_CLK
                         | TOUCH_CS_GPIO_CLK | TOUCH_BUSY_GPIO_CLK, ENABLE); 
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_PinAFConfig(TOUCH_MISO_GPIO, TOUCH_MISO_GPIO_SOURCE, TOUCH_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = TOUCH_MISO_GPIO_PIN;
  GPIO_Init(TOUCH_MISO_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(TOUCH_MOSI_GPIO, TOUCH_MOSI_GPIO_SOURCE, TOUCH_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = TOUCH_MOSI_GPIO_PIN;
  GPIO_Init(TOUCH_MOSI_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(TOUCH_SCK_GPIO, TOUCH_SCK_GPIO_SOURCE, TOUCH_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = TOUCH_SCK_GPIO_PIN;
  GPIO_Init(TOUCH_SCK_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = TOUCH_CS_GPIO_PIN;
  GPIO_Init(TOUCH_CS_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = TOUCH_BUSY_GPIO_PIN;
  GPIO_Init(TOUCH_BUSY_GPIO, &GPIO_InitStructure);
}

static void Touch_Spi_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  
  RCC_APB1PeriphClockCmd(TOUCH_RCC_CLK, ENABLE);
  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(TOUCH_SPI, &SPI_InitStructure); 
  SPI_Cmd(TOUCH_SPI, ENABLE);	
}

void Touch_Init(void)
{
  Touch_Gpio_Init();
  Touch_Spi_Init();
}

uint8_t Touch_Detected(void)
{
  return GPIO_ReadInputDataBit(TOUCH_BUSY_GPIO, TOUCH_BUSY_GPIO_PIN);
}

uint8_t Touch_Spi_ReadWrite(uint8_t data)
{
  uint8_t redata;

  while (!(TOUCH_SPI->SR & (1 << 1)));
  TOUCH_SPI->DR = data;
  while (!(TOUCH_SPI->SR & (1 << 0)));
  redata = TOUCH_SPI->DR;
  return redata;	
}

uint16_t Touch_ReadAD(uint8_t cmd)
{
  uint16_t redata;

  EN_SELECT();
  Touch_Spi_ReadWrite(cmd);
  
  Delay();

  redata = Touch_Spi_ReadWrite(0xFF);
  redata = redata << 8;
  redata |= Touch_Spi_ReadWrite(0xFF);
  redata = redata >> 3;
  redata &= 0xFFF;
  DS_SELECT();  
  return redata;	  
}

uint16_t Get_TouchADX(void)
{
  uint32_t sum = 0;
  uint32_t i;

  for (i = 0; i < 16; i++) 
  {
    sum += Touch_ReadAD(TOUCH_CMD_X);
    Delay();
  }
  sum >>= 4;
  
  return ((uint16_t)sum);
}

uint16_t Get_TouchADY(void)
{
  uint32_t sum;
  uint32_t i;

  for (i = 0; i < 16; i++) 
  {
    sum += Touch_ReadAD(TOUCH_CMD_Y);
    Delay();
  }
  sum >>= 4;
  return ((uint16_t)sum);
}
