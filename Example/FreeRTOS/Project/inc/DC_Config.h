#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"

#define GPIO_CHANGE 0

typedef enum {LOW = 0, HIGH = !LOW} ElecLevel;  /*	电平高低状态	*/
typedef enum {OFF = 0x00, ON = !OFF} SWITCH_STATUS;

#if GPIO_CHANGE == 1
	#define DC_OUT_PIN 	 GPIO_Pin_3
	#define DC_OUT_PORT  GPIOI
#else
	#define DC_OUT_PIN 	 GPIO_Pin_2
	#define DC_OUT_PORT  GPIOC
#endif

#define DC_IN_PIN    GPIO_Pin_1
#define DC_IN_PORT	 GPIOC

#define DC_SWITCH_PIN  GPIO_Pin_0
#define DC_SWITCH_PORT GPIOB

#define DC_TEMPERATURE_PIN  GPIO_Pin_1
#define DC_TEMPERATURE_PORT GPIOB

void DC_GPIO_Conf(void);
void DC_EXTI_Conf(void);

void DC_USART_Conf(void);
void DC_USART_SendBuf(u8 *buf,u8 length);
void DC_USART_PutChar(u8 ch);

void setElecLevel(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin_x,ElecLevel DPFlag);
ElecLevel getElecLevel(GPIO_TypeDef * GPIOx ,uint16_t GPIO_Pin_x);
SWITCH_STATUS GetSwitchStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x);					/*	获取拨码状态	*/

#endif
