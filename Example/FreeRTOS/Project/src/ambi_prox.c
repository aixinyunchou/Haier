#include "ambi_prox.h"

/**************************************************
 * 函数：距离、光感应中断脚初始化，暂时不用
 * 输入：无
 * 输出：无
 * 返回：void
***************************************************/
void Ambi_Prox_Gpio_Init(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(AMBI_PROX_INT_CLK, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure Px pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = AMBI_PROX_INT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(AMBI_PROX_INT_GPIO, &GPIO_InitStructure);

  /* Connect EXTI Line3 to Px pin */
  SYSCFG_EXTILineConfig(AMBI_PROX_INT_PORT, EXTI_PinSource3);

  /* Configure EXTI Line3 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line3 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/**************************************************
 * 函数：读器件ID号，测试用，uart6输出信息
 * 输入：无
 * 输出：无
 * 返回：void
***************************************************/
void Ambi_Prox_ReadID(void)
{
	uint8_t id = 0;
	TWI_Initialize();
	ms_Delay(100);
	I2CRdStr(AMBI_PROX_ADDR, REGISTER_ID, &id, 1);
	ms_Delay(100);
	I2CRdStr(AMBI_PROX_ADDR, REGISTER_ID, &id, 1);
	if(id != 0x21)
	{
		//Serial_PutString((uint8_t*)"AMBI_PROX sensor error!");
		while(1);//读取失败，死循环
	}
	//Serial_PutString((uint8_t*)"AMBI_PROX sensor ID id 0x21!");
}



/**************************************************
 * 函数：设置红外灯管LED电流
 * 输入：Current -- 设置电流大小，参看规格书（vcnl4010）
 * 输出：无
 * 返回：void
***************************************************/
void Set_Current(uint8_t Current)
{
	uint8_t reg, send;
	reg = REGISTER_PROX_CURRENT;
	send = Current;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//
/**************************************************
 * 函数：读取红外灯管LED电流
 * 输入：无
 * 输出：Current -- 读出存放，参看规格书（vcnl4010）
 * 返回：void
***************************************************/
void Read_Current(uint8_t *Current)
{
	uint8_t reg, recv;
	I2CRdStr(AMBI_PROX_ADDR, reg, &recv, 1);
	*Current = recv;
}

//设置命令寄存器
void Set_CommandRegister(uint8_t Command)
{
	uint8_t reg, send;
	reg = REGISTER_COMMAND;
	send = Command;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//读取命令寄存器
void Read_CommandRegister(uint8_t *Command)
{
	uint8_t reg, recv;
	reg = REGISTER_COMMAND;
	I2CRdStr(AMBI_PROX_ADDR, reg, &recv, 1);
	*Command = recv;
}


//设置距离感应测量速度
void Set_ProximityRate(uint8_t ProximityRate)
{
	uint8_t reg, send;
	reg = REGISTER_PROX_RATE;
	send = ProximityRate;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//光感应相关设置
void Set_AmbiConfiguration(uint8_t AmbiConfiguration)
{
	uint8_t reg, send;
	reg = REGISTER_AMBI_PARAMETER;
	send = AmbiConfiguration;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//设置中断模式
void Set_InterruptControl(uint8_t InterruptControl)
{
	uint8_t reg, send;
	reg = REGISTER_INTERRUPT_CONTROL;
	send = InterruptControl;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//读取中断模式
void Read_InterruptControl(uint8_t *InterruptControl)
{
	uint8_t reg, recv;
	reg = REGISTER_INTERRUPT_CONTROL;
	I2CRdStr(AMBI_PROX_ADDR, reg, &recv, 1);
	*InterruptControl = recv;
}

//设置中断状态位，主要用于中断flag的clear
void Set_InterruptStatus(uint8_t InterruptStatus)
{
	uint8_t reg, send;
	reg = REGISTER_INTERRUPT_STATUS;
	send = InterruptStatus;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//读取中断状态，用于判断中断来源等信息
void Read_InterruptStatus(uint8_t *InterruptStatus)
{
	uint8_t reg, recv;
	reg = REGISTER_INTERRUPT_STATUS;
	I2CRdStr(AMBI_PROX_ADDR, reg, &recv, 1);
	*InterruptStatus = recv;
}


void Set_ModulatorTimingAdjustment(uint8_t ModulatorTimingAdjustment)
{
	uint8_t reg, send;
	reg = REGISTER_PROX_TIMING;
	send = ModulatorTimingAdjustment;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send, 1);
}

//读取当前距离值，单位为counts，counts无实际意义
void Read_ProxiValue(uint16_t *ProxiValue)
{
	uint8_t reg, recv[2];
	reg = REGISTER_PROX_VALUE;
	I2CRdStr(AMBI_PROX_ADDR, reg, &recv[0], 2);
	*ProxiValue = recv[1] | (recv[0]<<8);
}


//读取当前光线强度值
void Read_AmbiValue(uint16_t *AmbiValue)
{
	uint8_t reg, recv[2];
	reg = REGISTER_AMBI_VALUE;
	I2CRdStr(AMBI_PROX_ADDR, reg, &recv[0], 2);
	*AmbiValue = recv[1] | (recv[0]<<8);
}


//设置高中断阈值
void Set_HighThreshold(uint16_t HighThreshold)
{
	uint8_t reg, send[2];
	reg = REGISTER_INTERRUPT_HIGH_THRES;
	send[0] = HighThreshold >> 8;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send[0], 1);
	reg = REGISTER_INTERRUPT_HIGH_THRES+1;
	send[1] = HighThreshold;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send[1], 1);
}


//设置低中断阈值
void Set_LowThreshold(uint16_t LowThreshold)
{
	uint8_t reg, send[2];
	reg = REGISTER_INTERRUPT_LOW_THRES;
	send[0] = LowThreshold >> 8;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send[0], 1);
	reg = REGISTER_INTERRUPT_LOW_THRES + 1;
	send[1] = LowThreshold;
	I2CWrStr(AMBI_PROX_ADDR, reg, &send[1], 1);
}


//测试用
void AMBI_PROX_Test(void)
{
	uint8_t c1=0, c2, i = 0, current = 2;
	uint16_t prox, prox1, AmbiValue;
	uint32_t sumprox = 0;
	Set_Current(current);
	Read_Current(&c1);
	if(c1 != current)
	{
		//Serial_PutString((uint8_t*)"Prox current set error!");
		while(1);
	}
	Set_CommandRegister(COMMAND_ALL_DISABLE);							//设置前失能
	Set_ProximityRate(PROX_MEASUREMENT_RATE_2);						//距离感应测量速率设置
	Set_CommandRegister(COMMAND_PROX_ENABLE | 						//距离感应功能打开
											COMMAND_AMBI_ENABLE |							//光感应功能打开
											COMMAND_SELFTIMED_MODE_ENABLE);
	Set_AmbiConfiguration(AMBI_PARA_AVERAGE_32 |
												AMBI_PARA_AUTO_OFFSET_ENABLE |
												AMBI_PARA_MEAS_RATE_2);
	
	//先读取环境距离状态，可用于中断高阈值、低阈值的设置
	for(i = 0; i < 30; i++)
	{
	do{
			Read_CommandRegister(&c2);
		}while(!(c2 & COMMAND_MASK_PROX_DATA_READY));
		Read_ProxiValue(&prox);
		sumprox += prox;
	}
	prox = sumprox/30;
	//Serial_PutString((uint8_t*)"Average prox is:");
	//Serial_SendData((uint8_t*)(&prox), 2);
	
	//测试、串口6输出距离感应/光感应信息
	while(1)
	{
		do{
			Read_CommandRegister(&c2);
		}while(!(c2 & (COMMAND_MASK_PROX_DATA_READY | COMMAND_MASK_AMBI_DATA_READY)));
		if(c2 & COMMAND_MASK_AMBI_DATA_READY)
		{
			Read_AmbiValue(&AmbiValue);
		}
		if(c2 & COMMAND_MASK_PROX_DATA_READY)
		{
			Read_ProxiValue(&prox1);
		}
		//Serial_SendData((uint8_t*)(&AmbiValue), 2);
		//Serial_SendData((uint8_t*)(&prox1), 2);
		ms_Delay(500);
	}
}


/****************************************************************************
 * 函数：距离、光感应初始化
 * 输入：无
 * 输出：无
 * 返回：0			--	初始化失败
 * 			 1			--	初始化成功
****************************************************************************/
uint8_t Ambi_Prox_Init(void)
{
	uint8_t command = 0, current = 2;
	TWI_Initialize();
	Set_Current(current);
	Read_Current(&command);
	if(command != current)
	{
		//Serial_PutString((uint8_t*)"Prox current set error!");
		return 0;			//fail
		//while(1);
	}
	Set_CommandRegister(COMMAND_ALL_DISABLE);							//设置前失能
	Set_ProximityRate(PROX_MEASUREMENT_RATE_2);						//距离感应测量速率设置
	Set_CommandRegister(COMMAND_PROX_ENABLE | 						//距离感应功能打开
											COMMAND_AMBI_ENABLE |							//光感应功能打开
											COMMAND_SELFTIMED_MODE_ENABLE);
	Set_AmbiConfiguration(AMBI_PARA_AVERAGE_32 |
												AMBI_PARA_AUTO_OFFSET_ENABLE |
												AMBI_PARA_MEAS_RATE_2);
	return 1;				//done
}


/****************************************************************************
 * 函数：读取环境距离状态，可用于中断高阈值、低阈值的设置
 * 输入：无
 * 输出：无
 * 返回: 距离感应结果平均值，单位counts，counts无实际意义
****************************************************************************/
uint16_t Read_Aver_Prox(void)
{
	uint8_t i = 0, command;
	uint16_t prox = 0;
	uint32_t sumprox = 0;
	
	for(i = 0; i < 30; i++)
	{
	do{
			Read_CommandRegister(&command);
		}while(!(command & COMMAND_MASK_PROX_DATA_READY));
		Read_ProxiValue(&prox);
		sumprox += prox;
	}
	prox = sumprox/30;
	return prox;
}


/****************************************************************************
 * 函数：读取单次距离值，单位为counts，counts无实际意义
 * 输入：无
 * 输出：无
 * 返回: 单次距离感应结果，单位counts，counts无实际意义
****************************************************************************/
uint16_t Read_single_Prox(void)
{
	uint8_t command = 0;
	uint16_t prox = 0;
	do{
			Read_CommandRegister(&command);
		}while(!(command & COMMAND_MASK_PROX_DATA_READY));
	if(command & COMMAND_MASK_PROX_DATA_READY)
	{
		Read_ProxiValue(&prox);
	}
	return prox;
}


/****************************************************************************
 * 函数：读取单次光感应值，单位为counts，除以4可得到illuminance
 * 输入：无
 * 输出：无
 * 返回: 单次光感应结果值
****************************************************************************/
uint16_t Read_single_Ambi(void)
{
	uint8_t command = 0;
	uint16_t ambi = 0;
	do{
			Read_CommandRegister(&command);
		}while(!(command & COMMAND_MASK_AMBI_DATA_READY));
	if(command & COMMAND_MASK_AMBI_DATA_READY)
	{
		Read_AmbiValue(&ambi);
	}
	return ambi;
}


void Ambi_Prox_Test3(void)
{
	char c;
	float temp;
	uint16_t prox, ambi;
	Ambi_Prox_Init();
	while(1)
	{
		//距离
		prox = Read_single_Prox();
		debug_printf("Proxi: %d counts,\t", prox);
		
		//光感应
		ambi = Read_single_Ambi();
		temp = (float)ambi/4;
		debug_printf("Ambi: %.2f lx\r\n", temp);
	
		ms_Delay(500);
		//c = debug_getchar (0);
		if(c != 0)
			break;
	}
}
