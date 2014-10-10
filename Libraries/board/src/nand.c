/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：nand.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：nand flash(k9f1g08) 驱动。 
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "nand.h"

static void Nand_GPIO_Init(void)
{
  /* 定义GPIO初始化结构体 */
  GPIO_InitTypeDef GPIO_InitStructure;
  /*
   ****************************************************************************
   * PD14 --> D0      | PD4  --> NAND_NOE
   * PD15 --> D1      | PD5  --> NAND_NWE
   * PD0  --> D2      | PD6  --> NAND_NWAIT
   * PD1  --> D3      | PD7  --> NAND_NCE2
   * PE7  --> D4      | PD11 --> NAND_CLE
   * PE8  --> D5      | PD12 --> NAND_ALE
   * PE9  --> D6      |
   * PE10 --> D7      |
   ****************************************************************************
   */
  /* 使能GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
  /* 设置GPIO mode、speed、otype、pupd */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  |GPIO_Pin_1  |
                                GPIO_Pin_14 |GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9 |GPIO_Pin_10;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7 |
                                GPIO_Pin_11 |GPIO_Pin_12 ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);    
}

void Nand_Init(void)
{
  FMC_NANDInitTypeDef FMC_NANDInitStruct;
  FMC_NAND_PCCARDTimingInitTypeDef FMC_SpaceTimingStruct;
	
  Nand_GPIO_Init();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);



  FMC_SpaceTimingStruct.FMC_SetupTime = 0x02;
  FMC_SpaceTimingStruct.FMC_WaitSetupTime = 0x08;
  FMC_SpaceTimingStruct.FMC_HiZSetupTime = 0x04;
  FMC_SpaceTimingStruct.FMC_HoldSetupTime = 0x04;
	
  FMC_NANDInitStruct.FMC_Bank = FMC_Bank2_NAND;
  FMC_NANDInitStruct.FMC_Waitfeature = FMC_Waitfeature_Disable;
  FMC_NANDInitStruct.FMC_MemoryDataWidth = FMC_NAND_MemoryDataWidth_8b;
  FMC_NANDInitStruct.FMC_ECC = FMC_ECC_Disable;
  FMC_NANDInitStruct.FMC_ECCPageSize = FMC_ECCPageSize_2048Bytes;
  FMC_NANDInitStruct.FMC_TCLRSetupTime = 0x00;
  FMC_NANDInitStruct.FMC_TARSetupTime = 0x00;
  FMC_NANDInitStruct.FMC_CommonSpaceTimingStruct = &FMC_SpaceTimingStruct;
  FMC_NANDInitStruct.FMC_AttributeSpaceTimingStruct = &FMC_SpaceTimingStruct;
  FMC_NANDInit(&FMC_NANDInitStruct); 
  FMC_NANDCmd(FMC_Bank2_NAND, ENABLE);
}


__inline static void nand_cmd(uint8_t cmd)
{
  *(volatile uint8_t*)(NAND_BANK | CMD_AREA) = cmd;
}
__inline static void nand_addr(uint8_t addr)
{
  *(volatile uint8_t*)(NAND_BANK | ADDR_AREA) = addr;
}

__inline static void nand_writebyte(uint8_t data)
{
  *(volatile uint8_t*)(NAND_BANK | DATA_AREA) = data;
}

__inline static uint8_t nand_readbyte(void)
{
  return (*(volatile uint8_t*)(NAND_BANK | DATA_AREA));
}

__inline static void nand_waitready(void)
{
    while (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0);
}

__inline static uint8_t nand_readstatus(void)
{
    nand_cmd(NAND_CMD_STATUS);
    return (nand_readbyte());
}

uint32_t Nand_ReadID(void)
{
  uint8_t a, b, c, d;

  nand_cmd(NAND_CMD_READID);
  nand_addr(0x00);

  a = nand_readbyte();
  b = nand_readbyte();
  d = nand_readbyte();
  c = nand_readbyte();
  return ((a << 24) | (b << 16) | (c << 8) | d);
}

void Nand_Reset(void)
{
  nand_cmd(NAND_CMD_RESET);
  nand_waitready();
}

uint32_t Nand_BlockErase(uint32_t Block)
{
  if (Block >= NAND_BLOCK_NUM)return 0;
  
  nand_cmd(NAND_CMD_ERASE0);
  nand_addr(Block & 0xff);
  nand_addr((Block >> 8) & 0xff);
  nand_cmd(NAND_CMD_ERASE1);	
  nand_waitready();
  
  while (!(nand_readstatus() & (1 << 6)));
  if (nand_readstatus() & 0x01) {
    return 0;
  }  
  return 1;
}

uint32_t Nand_PageRead(uint32_t page, uint32_t offset, uint8_t* buf, uint32_t len)
{
  uint32_t i, readnum;
 
  if (page >= NAND_PAGE_NUM)return 0; 
  if (offset >= NAND_PAGE_DATA_SIZE)return 0;
  if ((offset + len) > NAND_PAGE_DATA_SIZE) {
    readnum = NAND_PAGE_DATA_SIZE - offset;
  } else {
    readnum = len;
  }
  
  nand_cmd(NAND_CMD_READ_1);
  nand_addr(offset & 0xff);
  nand_addr((offset >> 8) & 0xff);
  nand_addr(page & 0xff);
  nand_addr((page >> 8) & 0xff);
  nand_cmd(NAND_CMD_READ_TRUE);
  nand_waitready();
  for (i = 0; i < readnum; i++) {
    buf[i] = nand_readbyte();
  }
  return readnum;
}

uint32_t Nand_PageWrite(uint32_t page, uint32_t offset, uint8_t *buf, uint32_t len)
{
  uint32_t i, writenum;

  if (page >= NAND_PAGE_NUM)return 0; 
  if (offset >= NAND_PAGE_DATA_SIZE)return 0;
  if ((offset + len) > NAND_PAGE_DATA_SIZE) {
    writenum = NAND_PAGE_DATA_SIZE - offset;
  } else {
    writenum = len;
  } 

  nand_cmd(NAND_CMD_PAGEPROGRAM);
  nand_addr(offset & 0xff);
  nand_addr((offset >> 8) & 0xff);
  nand_addr(page & 0xff);
  nand_addr((page >> 8) & 0xff);
  
  
  for (i = 0; i < writenum; i++) {
    nand_writebyte(buf[i]);
  }
  nand_cmd(NAND_CMD_PAGEPROGRAM_TRUE);
  nand_waitready();
  
  while (!(nand_readstatus() & (1 << 6)));
  if (nand_readstatus() & 0x01) {
    return 0;
  }
  return writenum;  
  
}
