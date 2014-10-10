#include "led_buzzer.h"

uint8_t Led_NO = 1;//test，按键变换led实验用。1,2,3表示不同颜色
uint8_t led_up = 0;//led_up == 1：LED渐亮，反之渐灭。
uint8_t low_time = 0;//延长呼吸灯灭时长
uint16_t CCR4_Val = 87;//pwm初始值（任意）
uint8_t Breath_Colour = NONE_BREATHE;

void cfg_LED(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}


void test_LED(void)
{
	cfg_LED();
	GPIO_SetBits(GPIOH,GPIO_Pin_5);
	GPIO_SetBits(GPIOH,GPIO_Pin_6);
	while(1)
	{	
		//ms_Delay(2000);
		GPIO_ResetBits(GPIOH,GPIO_Pin_5);
		GPIO_SetBits(GPIOH,GPIO_Pin_6);

		//ms_Delay(2000);
		GPIO_SetBits(GPIOH,GPIO_Pin_5);
		GPIO_ResetBits(GPIOH,GPIO_Pin_6);
	}
}

void Led_Show(void)
{
	switch(Led_NO)
	{
		case 1://R
			GPIO_ResetBits(GPIOH, GPIO_Pin_6);//B
			GPIO_ResetBits(GPIOH, GPIO_Pin_5);//G
			GPIO_SetBits(GPIOI, GPIO_Pin_3);//R
			break;
		case 2://G
			GPIO_ResetBits(GPIOH, GPIO_Pin_6);//B
			GPIO_SetBits(GPIOH, GPIO_Pin_5);//G
			GPIO_ResetBits(GPIOI, GPIO_Pin_3);//R
			break;
		case 3://B
			GPIO_SetBits(GPIOH, GPIO_Pin_6);//B
			GPIO_ResetBits(GPIOH, GPIO_Pin_5);//G
			GPIO_ResetBits(GPIOI, GPIO_Pin_3);//R
			break;
		default:
			break;
	}
	
}

void Buzzer_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(BUZZER_RCC_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
	GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);
}


void Buzzer_Test(void)
{
	Buzzer_GPIO();
	while(1)
	{
		GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);
		Buzzer_Delay(1);
		GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);
		Buzzer_Delay(1);
	}
}

void Buzzer_On(uint32_t BUzzerTIme)
{
	//Buzzer_GPIO();
	uint32_t time = BUzzerTIme;
	Buzzer_GPIO();
	while(--time)
	{
		GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);
		Buzzer_Delay(1);
		GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);
		Buzzer_Delay(1);
	}
}

void Buzzer_Delay(uint32_t time)
{
	uint32_t i = 0, j = 0;
	for(i = 0; i < time; i++)
	{
		for(j = 0; j < 0x15E5; j++)
		{;}
	}
}

/*led blue的呼吸效果，用timer12的pwm输出和timer3定时器中断实现*/
void Blu_Pwm_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

  /* GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(LED_B_CLK, ENABLE);
  
  /* GPIOD Configuration: TIM4 CH1 (PD12), TIM4 CH2 (PD13), TIM4 CH3 (PD14) and TIM4 CH4 (PD15) */
  GPIO_InitStructure.GPIO_Pin = LED_B_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(LED_B_GPIO, &GPIO_InitStructure); 

  /* Connect TIM4 pins to AF2 */  
  GPIO_PinAFConfig(LED_B_GPIO, LED_B_PINS, GPIO_AF_TIM12);
}

/*led  yellow的呼吸效果，用timer12的pwm输出和timer3定时器中断实现*/
void Yel_Pwm_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(LED_Y_CLK, ENABLE);
  
  /* GPIOD Configuration: TIM4 CH1 (PD12), TIM4 CH2 (PD13), TIM4 CH3 (PD14) and TIM4 CH4 (PD15) */
  GPIO_InitStructure.GPIO_Pin = LED_Y_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(LED_Y_GPIO, &GPIO_InitStructure); 

  /* Connect TIM4 pins to AF2 */  
  GPIO_PinAFConfig(LED_Y_GPIO, LED_Y_PINS, GPIO_AF_TIM3);
}

void Blu_Led_Pwm(void)
{
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	Breath_Colour = BLUE_BREATHE;
	
	Blu_Pwm_GPIO_Config();
	
  RCC_TIMCLKPresConfig(RCC_TIMPrescActivated);

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 21000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 699;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM12, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
	
	
  TIM_ARRPreloadConfig(TIM12, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM12, ENABLE);
}

void Yel_Led_Pwm(void)
{
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	Breath_Colour = YEL_BREATHE;
	
	Yel_Pwm_GPIO_Config();
	
  RCC_TIMCLKPresConfig(RCC_TIMPrescActivated);

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 21000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 699;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

void Led_Breath(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;	
	if(CCR4_Val == 0)
	{
		//ms_Delay(1000);
		low_time++;
		if(low_time == 150)
		{
			low_time = 0;
			led_up = 1;
		}
		else
			led_up = 3;
	}
		
	if(CCR4_Val == 300)
		led_up = 0;
	
	if(led_up == 1)
		CCR4_Val++;
	else if(led_up == 0)
		CCR4_Val--;
	else
		CCR4_Val = 0;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	if(Breath_Colour == BLUE_BREATHE)
	{
		TIM_OC1Init(TIM12, &TIM_OCInitStructure);
	}
	else 
	{
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	}
}

void Led_Timer_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_TimeBaseInitTypeDef
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* ????*/
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;			   //???   
	TIM_TimeBaseStructure.TIM_Prescaler = 20;    	   //???,??+1??????
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//????
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void Led_Timer_NVIC_Config(void) 
{
	NVIC_InitTypeDef NVIC_InitStruct; 

	//NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 

	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStruct); 
}

void Led_Timer(void)
{
	Led_Timer_Config();
	Led_Timer_NVIC_Config();
}

void Led_Colour_Change(uint8_t COlour)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	if(COlour == BLUE_BREATHE)
	{
		Blu_Led_Pwm();
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	}
	else
	{
		Yel_Led_Pwm();
		TIM_OC1Init(TIM12, &TIM_OCInitStructure);
	}
}
