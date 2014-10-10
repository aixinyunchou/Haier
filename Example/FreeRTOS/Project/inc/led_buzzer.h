
#ifndef __LED_BUZZER_H_
#define __LED_BUZZER_H_

#include "includes.h"

#define NONE_BREATHE	0
#define BLUE_BREATHE	1
#define YEL_BREATHE		2

/*timer12 ch1*/
#define LED_B_GPIO          GPIOH
#define LED_B_PIN           GPIO_Pin_6
#define LED_B_PINS          GPIO_PinSource6
#define LED_B_CLK           RCC_AHB1Periph_GPIOH

/*timer3 ch4*/
#define LED_Y_GPIO          GPIOB
#define LED_Y_PIN           GPIO_Pin_1
#define LED_Y_PINS          GPIO_PinSource1
#define LED_Y_CLK           RCC_AHB1Periph_GPIOB


/*蜂鸣器*/
/*应该用pwm驱动蜂鸣器，现用管脚非timer输出通道，暂用io模拟方波驱动蜂鸣器*/
//#define BUZZER_GPIO			GPIOI
//#define BUZZER_PIN			GPIO_Pin_11
//#define BUZZER_RCC_CLK  RCC_AHB1Periph_GPIOI

//#define BUZZER_ON()			GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);
//#define BUZZER_OFF()		GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);

#define BUZZER_GPIO			GPIOB
#define BUZZER_PIN			GPIO_Pin_0
#define BUZZER_PINS     GPIO_PinSource0
#define BUZZER_RCC_CLK  RCC_AHB1Periph_GPIOB

//#define BUZZER_ON()			GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);
//#define BUZZER_OFF()		GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);


extern uint8_t Led_NO;
extern uint8_t Led_Breath_Flag;
extern uint8_t Breath_Colour;

void cfg_LED(void);
void test_LED(void);
void Led_Show(void);

void Buzzer_GPIO(void);
void Buzzer_Test(void);
void Buzzer_Delay(uint32_t time);
void Buzzer_On(uint32_t BUzzerTIme);

void Pwm_GPIO_Config(void);
//void Led_Pwm(void);
void Led_Breath(void);
void Yel_Led_Pwm(void);
void Blu_Led_Pwm(void);
void Led_Colour_Change(uint8_t COlour);

void Led_Timer_Config(void);
void Led_Timer_NVIC_Config(void);
void Led_Timer(void);

void Buzzer_Pwm_Init(void);
#endif //__LED_BUZZER_H_
