/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：lcd.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：lcd驱动头文件。 
 ******************************************************************************
 */
#ifndef __LCD_H__
#define __LCD_H__
#include "stm32f4xx.h"
#include "sdram.h"


#define LCD_FRAME_BUFFER       SDRAM_BANK_ADDR
#define BUFFER_OFFSET          ((uint32_t)0x130000)


void     LCD_Init                   (void);
void     LCD_DisplayOn              (void);
void     LCD_DisplayOff             (void);

#endif /* __LCD_H__ */
