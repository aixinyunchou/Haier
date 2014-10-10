#ifndef __PIR_H__
#define __PIR_H__

#include "includes.h"
#define GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define PIR_OUT_PIN              GPIO_Pin_3

extern float limitdata;
extern uint8_t passdata;

extern uint16_t pir_data[];

void Init_Pir_Port_Out(void);
void Init_Pir_Port_In(void);

uint16_t Get_Pir_Value(void);
uint16_t Get_Pir_Value_1794(void);
uint8_t Filter_Limit(void);
void Pir_Switch(void);
void Pir_Switch_1794(void);
void Printf_Pir_Data(uint16_t type);
static double filterloop(double next);
void Pir_TEST(uint16_t type);

#endif
