/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：at24cxx.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：I2C eeprom 驱动
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "at24xx.h"


uint32_t I2CEeprom_WritePage(uint8_t *buf, uint32_t addr, uint32_t num)
{
  /* wait busy */
  while(I2C_GetFlagStatus(AT24XX_I2C, I2C_FLAG_BUSY));
  /* Send START condition */
  I2C_GenerateSTART(AT24XX_I2C, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_MODE_SELECT))  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(AT24XX_I2C, AT24XX_ADDR, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  /* state=I2C1->SR2; //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位 */
  /* Send the EEPROM's internal address to write */
#if (AT24XX_ADDLENGTH == 2)
  I2C_SendData(AT24XX_I2C, (uint8_t)(addr >> 8));
#endif
  I2C_SendData(AT24XX_I2C, (uint8_t)addr);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING));  
  
  while (num--) 
  {
    I2C_SendData(AT24XX_I2C, *buf);
	buf++;
	while(I2C_CheckEvent(AT24XX_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
  }
  I2C_GenerateSTOP(AT24XX_I2C,ENABLE);
  return 0;
}

uint32_t I2CEeprom_WaitStandby(void)    
{
  uint32_t tmpSR1;
  /* While the bus is busy */
  while(I2C_GetFlagStatus(AT24XX_I2C, I2C_FLAG_BUSY));
  
  AT24XX_I2C->SR1 &= 0x00;
  do
  {
    I2C_GenerateSTART(AT24XX_I2C, ENABLE);
  
    tmpSR1=AT24XX_I2C->SR1;
  
    I2C_Send7bitAddress(AT24XX_I2C, AT24XX_ADDR, I2C_Direction_Transmitter);
  }while((AT24XX_I2C->SR1 & 0x02)==0X00);

  I2C_ClearFlag(AT24XX_I2C, I2C_FLAG_AF);
  I2C_GenerateSTOP(AT24XX_I2C, ENABLE);
}

uint32_t I2CEeprom_PageRead(uint8_t *buf, uint32_t addr, uint32_t num)
{
  /* While the bus is busy */
  while(I2C_GetFlagStatus(AT24XX_I2C, I2C_FLAG_BUSY));
  /* Send START condition */
  I2C_GenerateSTART(AT24XX_I2C, ENABLE);  
  /* Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_MODE_SELECT)); 
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(AT24XX_I2C, AT24XX_ADDR, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
#if (AT24XX_ADDLENGTH == 2)
  I2C_SendData(AT24XX_I2C, (uint8_t)(addr >> 8));
#endif
  I2C_SendData(AT24XX_I2C, (uint8_t)addr); 

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING));  

  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(AT24XX_I2C, ENABLE);
  
  /* Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  while(!I2C_CheckEvent(AT24XX_I2C, I2C_EVENT_MASTER_MODE_SELECT));  
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(AT24XX_I2C, AT24XX_ADDR, I2C_Direction_Receiver); 
  
  while (num) 
  {
    if (num == 1) 
	{
	  I2C_AcknowledgeConfig(AT24XX_I2C,DISABLE);
	  I2C_GenerateSTOP(AT24XX_I2C,ENABLE);
	}
    if(I2C_CheckEvent(AT24XX_I2C,I2C_EVENT_MASTER_BYTE_RECEIVED)==SUCCESS)
    {
      /* Read a byte from the EEPROM */
      *buf = I2C_ReceiveData(AT24XX_I2C);  
      /* Point to the next location where the byte read will be saved */
      buf++;  
      /* Decrement the read bytes counter */
      num--;   
    }	
  }
  I2C_AcknowledgeConfig(AT24XX_I2C, ENABLE);
  return 0;
}

void at24xx_write(uint32_t addr, uint8_t *buf, uint16_t count)
{
  uint32_t len;

	
  while ((count)) {
    if (count > AT24XX_PAGESIZE) {
      len = AT24XX_PAGESIZE;
    } else {
      len = count;
    }
    count -= len;
    I2CEeprom_WritePage(buf, addr, len);
    addr += len;
    buf += len;
    I2CEeprom_WaitStandby();
  }
}

void at24xx_read(uint32_t addr, uint8_t *buf, uint16_t count)
{
  I2CEeprom_PageRead(buf, addr, count);
}

void at24xx_gpio_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
   

  
  /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_AHB1PeriphClockCmd(AT24XX_SCL_CLK | AT24XX_SDA_CLK, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  

  /* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(AT24XX_SCL_GPIO, AT24XX_SCL_PINS, AT24XX_GPIO_AF);

  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(AT24XX_SDA_GPIO, AT24XX_SDA_PINS, AT24XX_GPIO_AF);      
  /*!< GPIO configuration */  
  /*!< Configure sEE_I2C pins: SCL */   
  GPIO_InitStructure.GPIO_Pin = AT24XX_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(AT24XX_SCL_GPIO, &GPIO_InitStructure);

  /*!< Configure sEE_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = AT24XX_SDA_PIN;
  GPIO_Init(AT24XX_SDA_GPIO, &GPIO_InitStructure);


}


void at24xx_init(void)
{
  I2C_InitTypeDef  I2C_InitStructure;

  at24xx_gpio_init();
  
  /*!< AT24XX_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(AT24XX_RCC_CLK, ENABLE);  
  /* Reset AT24XX_I2C IP */
  RCC_APB1PeriphResetCmd(AT24XX_RCC_CLK, ENABLE);
  /* Release reset signal of AT24XX_I2C IP */
  RCC_APB1PeriphResetCmd(AT24XX_RCC_CLK, DISABLE);
  /*!< I2C configuration */
  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 10000;
  

  /* Apply AT24XX_I2C configuration after enabling it */
  I2C_Init(AT24XX_I2C, &I2C_InitStructure);
  I2C_AcknowledgeConfig(AT24XX_I2C,ENABLE);
  /* AT24XX_I2C Peripheral Enable */
  I2C_Cmd(AT24XX_I2C, ENABLE);
//  /* Enable the AT24XX_I2C peripheral DMA requests */
//  I2C_DMACmd(AT24XX_I2C, ENABLE);
}
