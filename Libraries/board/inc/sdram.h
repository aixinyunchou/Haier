/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：sdram.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：sdram(w9825c6)驱动头文件。 
 ******************************************************************************
 */
#ifndef __SDRAM_H__
#define __SDRAM_H__

#include "stm32f4xx.h"


#define SDRAM_BANK_ADDR                          ((uint32_t)0xD0000000)
#define SDRAM_SIZE                               ((uint32_t)0x2000000)
#define SDRAM_SWAP_BANK_ADDR                     ((uint32_t)0x80000000)
#define SDRAM_REMAP_BANK_ADDR                    ((uint32_t)0x00000000)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)  

void SDRAM_Init(void);

#endif /* __SDRAM_H__ */
