/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：nand.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：nand(k9f1g08)驱动头文件。 
 ******************************************************************************
 */
#ifndef __NAND_H__
#define __NAND_H__

#include "stm32f4xx.h"

#define NAND_BANK                   0x70000000
#define CMD_AREA                    (uint32_t)(0x010000)
#define ADDR_AREA                   (uint32_t)(0x020000)
#define DATA_AREA                   (uint32_t)(0x000000)

#define K9FXX_ID                    0xECF10000
#define NAND_PAGES_PER_BLOCK        64
#define NAND_PAGE_DATA_SIZE         2048
#define NAND_PAGE_OOB_SIZE          64
#define NAND_BLOCK_NUM              1024
#define NAND_PAGE_NUM               (NAND_BLOCK_NUM * NAND_PAGES_PER_BLOCK)

/* FSMC NAND memory command */
#define	NAND_CMD_READ_1             ((uint8_t)0x00)
#define	NAND_CMD_READ_TRUE          ((uint8_t)0x30)

#define	NAND_CMD_RDCOPYBACK         ((uint8_t)0x00)
#define	NAND_CMD_RDCOPYBACK_TRUE    ((uint8_t)0x35)

#define NAND_CMD_PAGEPROGRAM        ((uint8_t)0x80)
#define NAND_CMD_PAGEPROGRAM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_COPYBACKPGM        ((uint8_t)0x85)
#define NAND_CMD_COPYBACKPGM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_ERASE0             ((uint8_t)0x60)
#define NAND_CMD_ERASE1             ((uint8_t)0xD0)

#define NAND_CMD_READID             ((uint8_t)0x90)
#define NAND_CMD_STATUS             ((uint8_t)0x70)
#define NAND_CMD_RESET              ((uint8_t)0xFF)

#define NAND_CMD_CACHEPGM           ((uint8_t)0x80)
#define NAND_CMD_CACHEPGM_TRUE      ((uint8_t)0x15)

#define NAND_CMD_RANDOMIN           ((uint8_t)0x85)
#define NAND_CMD_RANDOMOUT          ((uint8_t)0x05)
#define NAND_CMD_RANDOMOUT_TRUE     ((uint8_t)0xE0)

#define NAND_CMD_CACHERD_START      ((uint8_t)0x00)
#define NAND_CMD_CACHERD_START2     ((uint8_t)0x31)
#define NAND_CMD_CACHERD_EXIT       ((uint8_t)0x34)


void Nand_Init(void);
uint32_t Nand_ReadID(void);
void Nand_Reset(void);
uint32_t Nand_BlockErase(uint32_t Block);
uint32_t Nand_PageRead(uint32_t page, uint32_t offset, uint8_t* buf, uint32_t len);
uint32_t Nand_PageWrite(uint32_t page, uint32_t offset, uint8_t *buf, uint32_t len);
#endif
