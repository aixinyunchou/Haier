/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：touch.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：lcd touch驱动头文件。 
 ******************************************************************************
 */
#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "stm32f4xx.h"

extern void Touch_Init(void);
extern uint8_t Touch_Detected(void);
extern uint16_t Get_TouchADX(void);
extern uint16_t Get_TouchADY(void);

#endif /* __TOUCH_H__ */
