#include "DC_Config.h"

/******************************************************************************************
 *	函数名 ：DC_GPIO_Conf
 *	参数   ：void
 *  返回值 ：void
 *  功能   ：GPIO端口的管脚配置
 *******************************************************************************************/
void DC_GPIO_Conf(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;		                    //定义GPIO初始化结构体
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);			//使能GPIOB的AHP1时钟
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);			//使能GPIOC的AHP1时钟
#if GPIO_CHANGE == 1
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);			//使能GPIOC的AHP1时钟
#endif
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);			//使能GPIOE的AHP1时钟
   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//设置初始化模式为输出模式
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						  //设置输出类型为推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;					//设置输出速度为100Mhz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;               //上拉电阻
   GPIO_InitStructure.GPIO_Pin = DC_OUT_PIN;                  //定义初始化的管脚为输出管脚
   GPIO_Init(DC_OUT_PORT, &GPIO_InitStructure);					  			    //初始化GPIO的管脚
   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = DC_IN_PIN;
   GPIO_Init(DC_IN_PORT,&GPIO_InitStructure);
   
   DC_OUT_PORT-> BSRRH = DC_OUT_PIN;  						 /*	将输入输出口置低电平	*/
   DC_IN_PORT -> BSRRH = DC_IN_PIN;
   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = DC_SWITCH_PIN;
   GPIO_Init(DC_SWITCH_PORT,&GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = DC_TEMPERATURE_PIN;
   GPIO_Init(DC_TEMPERATURE_PORT,&GPIO_InitStructure);
   
}

/******************************* ***********************************************************
 *	 函数名 ：DC_EXIT_Conf()
 *	 参数   ：void
 *   返回值 ：void
 *   功能   ：端口中断配置，调用前需调用DC_GPIO_Conf()来配置引脚功能
 ******************************************************************************************/
void DC_EXTI_Conf(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);          //使能SYSSCFG
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource1);   //将EXTI_LINE1连接到PC1端口

	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;				//上升沿触发中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line1);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //具有最高抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

/******************************************************************************************
 *	 函数名 ：setElecLevel
 *	 参数   ：uint16_t GPIO_Pin_x, ElecLevel DPFlag
 *   返回值 ：void
 *   功能   ：设置端口电平高低
 ******************************************************************************************/
void setElecLevel(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin_x,ElecLevel DPFlag)
{
	if (DPFlag == HIGH)
	{
		GPIOx->BSRRL = GPIO_Pin_x;
	}else 
	{
		GPIOx->BSRRH = GPIO_Pin_x;
	}
}

/******************************************************************************************
 *	函数名 ：getElecLevel
 *	参数   ：uint16_t GPIO_Pin_x
 *  返回值 ：DP
 *  功能   ：返回输出端口电平高低
 ******************************************************************************************/
ElecLevel getElecLevel(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin_x)
{
	if ((GPIOx->IDR & GPIO_Pin_x) != RESET )
	{
		return HIGH;
	}
	else
	{
		return LOW;
	}
}

/********************************************************
 * 函数名 ：GetSwitchStatus
 * 参数   ：void 
 * 返回值 ：SWITCH_STATUS
 * 功能   ：获取掉电记忆拨码状态，当拨码状态为ON时，读取掉电记忆；为OFF时，不读取掉电记忆
 ********************************************************/
SWITCH_STATUS GetSwitchStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x)
{
	if ( GPIO_ReadInputDataBit(GPIOx,GPIO_Pin_x) == 0)
	{
		return ON;
	}
	else return OFF;
}

//以下是测试回信，用于显示回信字节
/*******************************************************************************************************************************
 * 函数名 ：DC_USART_Conf
 * 参数   ：void 
 * 返回值 ：void 
 * 功能   ：配置USART6，并初始化
 *******************************************************************************************************************************/
void DC_USART_Conf(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); //开启USART6时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //开启GPIOC时钟
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);//
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);// 
	
	/*配置GPIOC*/
	/*配置GPIOC_Pin7为TX输入*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/*配置GPIOC_Pin6为RX输出*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/*配置USART6*/
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate =115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART6, &USART_ClockInitStruct);
//	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); //使能 USART6中断
	USART_Cmd(USART6, ENABLE); //使能 USART6 
	
	USART_ClearFlag(USART6, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */   
}

/********************************************************************************************************************************
*函数名：DC_USART_SendBuf()
* 参数：USART_TypeDef* USART6 所要发送到的串口的指针，u8 *str 指向发送的字符串的指针
* 返回值：void
* 功能：向某个串口发送一个字符串
*********************************************************************************************************************************/
void DC_USART_SendBuf(u8 *buf, u8 length)
{
	u8 i;
	for (i = 0; i < length; ++i)
	{		
		USART_SendData(USART6,buf[i]);																			//将*str从USART6发送出去
		while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET)				//等待发送完毕
		{
		}
	}
}

void DC_USART_PutChar( u8 ch )
{	
	USART_SendData(USART6,(u8)ch);																					//将ch从USART6发送出去
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET)				//等待发送完毕
	{
	}
}
