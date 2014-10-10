#include "DC_Protocol.h"

static BYTE cmd[LENGTH_OF_SEND_FRAME];
static BYTE STORE_CMD[LENGTH_OF_SEND_FRAME];
static BYTE RECEIVED_CMD[LENGTH_OF_RECEIVE_FRAME];
static __IO MachineStatus mStatus;		//内机状态，用于保存内机回信信息
static __IO u8 IS_INIT = 1;				//	是否处于初始化阶段
static BYTE DEFAULT_CMD[6] = {0X00,0X88,0X05,0X26,0XFF,0x00};   //	无掉电记忆时的默认值
__IO    CONTROL_STATUS CFlag = IDLE_STATUS;  				//	线控器状态	
__IO u8 TIM_CAP_SET[LENGTH_OF_RECEIVE_FRAME * 8 + 1];
__IO u8 BUS_STAYUS = 0;  //	0:空闲，1：忙	

/*	若拨码开关打开，则以线控器上温度为准，否者以内机温度为准	*/
//SWITCH_STATUS TEMPRATURE_MAIN = GetSwitchStatus(DC_TEMPERATURE_PORT,DC_TEMPERATURE_PIN);
SWITCH_STATUS TEMPRATURE_MAI = ON;

static void DC_InitComm(void);
static void Frame_ms(u32 time, ElecLevel DPFlag);   /*	维持time毫秒的高低电平						*/
static void CmdCopy(BYTE dest[],BYTE src[]);        /*	将命令字节从src[]复制到dest[] 		*/
static BYTE CheckFunction(BYTE buf[],u8 DataLen);		/*	校验发信与回信										*/
static void SendCmdToMachine(void);      	 					/*  发送命令字节到内机 								*/
static u8   CheckReceivedCmd(void);					 				/*	校验接收命令，若校验正确，则返回1	*/
static void AnalysisCmd(void);            					/*	解析回信													*/
void ReadCmdFromMachine(void);    		     					/*  从内机读命令字节,需在中断中调用   */
void TaskReceive(void *p_arg);
void SendCallback( xTimerHandle pxTimer);
void MainCallback( xTimerHandle pxTimer);

extern xTimerHandle xTimerMain;
extern xTimerHandle xTimerInit;
extern xTimerHandle xTimerSend;

/*********************************************************
 * 函数名：DC_Config()
 * 参数： void 
 * 返回值：void
 * 功能：初始化直流协议配置
 ********************************************************/
void DC_Config(void)
{
	DC_GPIO_Conf();
	DC_EXTI_Conf();
	DC_USART_Conf();
	DC_InitTimer();  //初始化生成软件定时器
	DC_InitComm();	 //初始化命令字节
}

void DC_InitComm(void)
{
	/*	A:0X00,B:0X08,C:0X01,D:0X26,E:0XFF	 */
	CmdCopy(cmd,DEFAULT_CMD);

//	if( ON == GetSwitchStatus(DC_SWITCH_PORT,DC_SWITCH_PIN) )  		// 
//	{
//		cmd[0] = 0;
//		cmd[0] |= (MEMORY_DATA->OOFlag << 7);
//		cmd[0] |= (MEMORY_DATA->WDFlag << 6);
//		cmd[0] |= (MEMORY_DATA->HeatFlag << 4);
//		cmd[0] |= (MEMORY_DATA->HeatFlag << 3);
//		cmd[0] |= MEMORY_DATA->Mode;
//		
//		cmd[1] &= ~POS_D05;
//		cmd[1] |= MEMORY_DATA->Temperature;
//		cmd[1] |= (MEMORY_DATA->WindSpeed << 4);
//		
//		cmd[2] &= ~POS_D45;
//		cmd[2] |= (MEMORY_DATA->FreshAir << 4);
//	}
	cmd[LENGTH_OF_SEND_FRAME-1] = CheckFunction(cmd,LENGTH_OF_SEND_FRAME-1);
}
/*********************************************************    
 * 函数名 ：CmdCopy()
 * 参数   ：BYTE *dest， BYTE *src
 * 返回值 ：void
 * 功能   ：将src[]内的命令字节复制到dest[]   
 *********************************************************/
static void CmdCopy(BYTE dest[],BYTE src[])
{
	u8 i;
	for(i = 0; i < LENGTH_OF_SEND_FRAME-1; ++i)
		dest[i] = src[i];
}

/********************************************************
 *函数名 ：CheckFunction
 *参数   ：BYTE buf[]
 *返回值 ：BYTE 
 *功能   ：校验buf[]，返回校验值
 ********************************************************/
static BYTE CheckFunction(BYTE buf[] ,u8 DataLen)
{
	BYTE res = 0;
	u8 i;
	for(i = 0; i < DataLen ;++i) res += buf[i];
	return (BYTE)res;
}


/********************************************************
 * 函数名 ：Frame_ms
 * 参数   ：u32 time, ElecLevel DPFlag
 * 返回值 ：void
 * 功能   ：维持高低电平time毫秒
 ********************************************************/
static void Frame_ms(u32 time, ElecLevel DPFlag)
{
	setElecLevel(DC_OUT_PORT,DC_OUT_PIN,DPFlag);
	DC_DelayMS(time);						/*	不能抢占MPU，待修改	*/
}

/********************************************************
 * 函数名 ：SendCmdToMachine()
 * 参数   ：void
 * 返回值 ：void
 * 功能   ：将命令字节转化为引脚高低电平
 *******************************
 *数据格式: LSB
 *通信原则:
 * 引导码 : 24ms 高
 *	  0   : 12ms 高
 *	  1   :  4ms 高  
 *	 间隔 :  4ms 低
 *******************************
 ********************************************************/
static void SendCmdToMachine(void)
{
	u8 i,j;
	CFlag = SENDING_STATUS;		//线控器发送状态
	BUS_STAYUS = 1;				//总先忙
	EXTI->IMR &= ~EXTI_Line1;	//关闭一号线中断
	if ( xTimerIsTimerActive(xTimerSend) != pdFALSE)
	{
		xTimerStop(xTimerSend,0);  //停止发送定时任务
	}
	
	Frame_ms(GUIDANCE_MS,HIGH);
	for (i = 0; i < LENGTH_OF_SEND_FRAME; ++i)
	{
		for (j = 0;j < 8; ++j)
		{
			Frame_ms(INTERNAL_MS,LOW);
			if(cmd[i] & (0x01 << j))
			{
				Frame_ms(NUM1_MS,HIGH);
			}else
			{
				Frame_ms(NUM0_MS,HIGH);
			}
		}
	}
	setElecLevel(DC_OUT_PORT,DC_OUT_PIN,LOW);
	CFlag = WAIT_TO_RECEIVE;	//线控器等待接收回信
	BUS_STAYUS = 0;				//总线闲
	EXTI->IMR |= EXTI_Line1;		//打开一号线中断
	if (IS_INIT == 0 && xTimerIsTimerActive(xTimerSend) == pdFALSE)
	{
		if(xTimerSend != NULL) xTimerStart(xTimerSend,0);
	}
}

/********************************************************
 * 函数名 ：ReadCmdFromMachine
 * 参数   ：void
 * 返回值 ：void
 * 功能   ：读从内机发来的命令字节，将接收的字节存到RECEIVED_CMD[]，
 *          考虑到协议拓展，需要预留接收时间，延迟1s
 ********************************************************/
void ReadCmdFromMachine(void) 
{
	uint32_t count;
	u8 i = 0;
	
	EXTI->IMR &= ~EXTI_Line1;
	CFlag = BEGIN_TO_RECEIVE;	//线控器状态置为开始接收状态
	BUS_STAYUS = 1;  			//接收回信时，总线置忙
	
	if ( xTimerIsTimerActive(xTimerSend) != pdFALSE)
	{
		xTimerStop(xTimerSend,0);  //停止发送定时任务
	}
	
	count = 0;
	DC_DelayMS(1); //等待电平稳定
	while(getElecLevel(DC_IN_PORT,DC_IN_PIN) != LOW)
	{
		++count;
		DC_DelayMS(1);
	}
//	DC_USART_PutChar(count);	
	
	if (( count >= GUIDANCE_MS && count - GUIDANCE_MS <= 2 ) || (count < GUIDANCE_MS && GUIDANCE_MS-count <= 2)) 
	{
		memset(RECEIVED_CMD,0,sizeof(RECEIVED_CMD)); 
		for ( ; i < LENGTH_OF_RECEIVE_FRAME * 8; ++i)
		{
			DC_DelayMS(1);
			while(getElecLevel(DC_IN_PORT,DC_IN_PIN) != HIGH)
			{
				DC_DelayMS(1);
			}
			
			count = 0;
			while(getElecLevel(DC_IN_PORT,DC_IN_PIN) != LOW)
			{
				++count;
				DC_DelayMS(1);				
			}
//			DC_USART_PutChar(count);
				
			if ((count >= NUM1_MS && count-NUM1_MS <=2) || (count < NUM1_MS && NUM1_MS-count <=2))
			{
				RECEIVED_CMD[i / 8] |= (0x01<<(i % 8));
			}
			else if ((count >= NUM0_MS && count-NUM0_MS <=2) || (count < NUM0_MS && NUM0_MS-count <=2))
			{
						//相应的位已经清0
			}else
			{
				CFlag = RECEIVE_ERROR;
			}
		}
	}else
	{
		CFlag = RECEIVE_ERROR;
		DC_DelayMS(640); //引导出错，忽略下面的回信
	}
	
	DC_USART_SendBuf(RECEIVED_CMD,5);
	
	if (CFlag == RECEIVE_ERROR)
	{
		//什么都不做
	}else
	{
		//	解析帧数据		
		if (1 == CheckReceivedCmd())
		{
			CmdCopy(STORE_CMD,cmd); 			//	将控制命令存储起来，下次可能有用
			AnalysisCmd();								//	校验正确，解析命令，反馈给线控器
			CFlag = RECEIVE_COMPLETED;  	//	数据接收完成
			if (IS_INIT) 									// 判断是否是上电或复位
			{
				if( xTimerIsTimerActive( xTimerMain ) != pdFALSE )
				{
					xTimerDelete(xTimerMain,0);  //删掉主4分钟任务 
				}
				if( xTimerIsTimerActive( xTimerInit ) != pdFALSE )
				{
					xTimerDelete(xTimerInit,0);  //删掉2秒任务
				}
//				xTimerSend = xTimerCreate("TimerSend",(200    / portTICK_RATE_MS),pdTRUE ,(void *)2,SendCallback);
				IS_INIT = 0;
			}
		}
		else
		{
			//	校验出错
			CFlag =  CHECK_ERROR;
		}
	}
	
//	EXTI_ClearITPendingBit(EXTI_Line1);
	EXTI->IMR |= EXTI_Line1;
	BUS_STAYUS = 0;
	if (IS_INIT == 0 && xTimerIsTimerActive(xTimerSend) == pdFALSE)
	{
		if(xTimerSend != NULL) xTimerStart(xTimerSend,0);
	}
}

/********************************************************
 * 函数名 ：CheckReceivedCmd
 * 参数   ：void
 * 返回值 ：u8
 * 功能   ：检验回信是否正确，正确则返回1，否则返回0
 ********************************************************/
static u8 CheckReceivedCmd(void)
{
	if (CheckFunction(RECEIVED_CMD,LENGTH_OF_RECEIVE_FRAME-1) == \
	   RECEIVED_CMD[LENGTH_OF_RECEIVE_FRAME-1])
	{
		return 1;
	}else 
	{
		return 0;
	}
}

/*	测试用，待修改	*/
/***************************************************
 * 参数：isOn --- 是否关机，0：关机   1：开机
 *      mode --- 模式，     1：制冷   2：除湿   4：制热
 *        temperature --- 设定温度，0 --- 16  ...... 14----30
 *	       windSpeed --- 风速，0：自动    1：低风    2：中风    3：高风 
 *       PAN --- 是否摆风    0：停止摆风    1： 开始摆风
 ***************************************************/
void DC_Control(u8 isOn, u8 mode ,u8 temperature, u8 windSpeed,u8 PAN)
{
//	CmdCopy(cmd,STORE_CMD); 			/*	将前一次的控制命令复制到cmd中*/
	CmdCopy(cmd,DEFAULT_CMD);
	
	cmd[0] |= (isOn << 7);
	cmd[0] |= (PAN << 6);
	cmd[0] |= mode;
	
	if (mode == HeatMode )  //加热模式
	{
		cmd[0] |= POS_D3;  //电热开
	}
	
	cmd[1] &= ~POS_D05;
	if(temperature >= 30)
	{
		cmd[1] |= 0x0E;
	}
	else if (temperature <= 16)
	{
		//	D03已经清0
	}
	else
	{
		cmd[1] |= (temperature - 16);
	}
	cmd[1] |= (windSpeed << 4);

/*	
	if (TEMPRATURE_MAIN == ON)
	{
		//	获取线控器上温度，写入字节E中
	}
*/	
	cmd[5] = CheckFunction(cmd,LENGTH_OF_SEND_FRAME-1);
	
//	HAS_CMD_TO_SEND = 1;
	SendCmdToMachine();
}

/**********************************************************
 * 测试发一帧数据，与老线控器第一帧数据一样，对比看有没有回信
 **********************************************************/
void DC_Test(void)
{
	cmd[0] = 0X01;
	cmd[1] = 0XA2;
	cmd[2] = 0X00;
	cmd[3] = 0X26;
	cmd[4] = 0XDB;
	cmd[5] = CheckFunction(cmd,LENGTH_OF_SEND_FRAME-1);  //cmd[5] = 0XA4;
	SendCmdToMachine();
}

static void AnalysisCmd(void)
{
	if (RECEIVED_CMD[2] != 0x00 && RECEIVED_CMD[2] != 0XFF)  /*	C字节不为00或FF，为新协议，双向刷新	*/
	{
		/*	如果以线控器环温为准，则不必解析A字节	*/
		if (RECEIVED_CMD[0] != 0xFF)
		{
			mStatus.IndoorTemperature = RECEIVED_CMD[0] & POS_D06;
		}else
		{
			mStatus.IndoorTemperature = 0xFF;
		}
//		
//		if (TEMPRATURE_MAIN == OFF)  //以内机环温为准
//		{
//			if (RECEIVED_CMD[0] != 0xFF)
//			{
//				mStatus.IndoorTemperature = RECEIVED_CMD[0] & POS_D06;
//			}else
//			{
//				mStatus.IndoorTemperature = 0xFF;
//			}
//			// 将内机传过来的温度报告给上层
//		}
		
		/*	解析B字节	*/
		mStatus.IsHeating     =    (RECEIVED_CMD[1] & POS_D7) >> 7;
		mStatus.InletGridFlag =    (RECEIVED_CMD[1] & POS_D6) >> 6;
		mStatus.MachineType   =    (RECEIVED_CMD[1] & POS_D5) >> 5;
		mStatus.ErrorInfo     =     RECEIVED_CMD[1] & POS_D04;
		
		/*	解析C字节	*/
		mStatus.OnOffFlag     =    (RECEIVED_CMD[2] & POS_D7) >> 7;
		mStatus.WindSwingFlag =    (RECEIVED_CMD[2] & POS_D6) >> 6;
		mStatus.WaterFlag     =    (RECEIVED_CMD[2] & POS_D4) >> 4;
		mStatus.HeaterFlag    =    (RECEIVED_CMD[2] & POS_D3) >> 3;
		mStatus.Mode          =     RECEIVED_CMD[2] & POS_D02;
		
		/*	解析D字节	*/
		mStatus.WindSpeed     =    (RECEIVED_CMD[3] & POS_D67) >> 6;
		mStatus.SetTemperature=    (RECEIVED_CMD[3] & POS_D25) >> 2;
		mStatus.RemoteControl =    (RECEIVED_CMD[3] & POS_D1 ) >> 1;
		mStatus.RemoteLock    =     RECEIVED_CMD[3] & POS_D0;
		
		/*	
	              将制热状态，进风栅开闭，以及机型报告给上级
	        */
		
		if (mStatus.RemoteControl)
		{
			/*	刷新显示，将需要刷新的信息反馈给线控器，并修改上一次保存的发送字节命令	*/
			/*	修改发送直接命令	*/
			cmd[0] &= ~POS_D67;
			cmd[0] &= ~POS_D03;
			cmd[0] |= (mStatus.OnOffFlag     << 7 );
			cmd[0] |= (mStatus.WindSwingFlag << 6 );
			cmd[0] |= (mStatus.HeaterFlag    << 3 );
			cmd[0] |=  mStatus.Mode;
			
			cmd[1] &= ~POS_D05;
			cmd[1] |= (mStatus.WindSpeed << 4);
			cmd[1] |= mStatus.SetTemperature;
		}
		
		if (mStatus.RemoteLock)
		{
			/*	线控器锁定，线控器不能操作		*/	
		}
		
		if (mStatus.ErrorInfo)
		{
			/*	显示故障信息,如何区分定频与变频？	*/
			switch (mStatus.ErrorInfo)
			{
				case 0:break;
				case 1:break;
				case 2:break;
				case 3:break;
				case 4:break;
				case 5:break;
				case 6:break;
				case 7:break;
				case 8:break;
				case 9:break;
				case 10:break;
				case 11:break;
				case 12:break;
				case 13:break;
				case 14:break;
				case 15:break;
				case 16:break;
				case 17:break;
				case 18:break;
				case 19:break;
				case 20:break;
				case 21:break;
				case 22:break;
				case 23:break;
				case 24:break;
				case 25:break;
				case 26:break;
				case 27:break;
				default:break;
			}
		}
	}
}


void TaskReceive(void *p_arg)
{
	ReadCmdFromMachine();
	vTaskDelete(NULL);
}

/******************************* ***********************************************************
 *	 函数名 ：EXTI1_IRQHandler()
 *	 参数   ：void
 *   返回值 ：void
 *   功能   ：外部一号线中断程序入口
 ******************************************************************************************/
void EXTI1_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line1);
	xTaskCreate( TaskReceive,  ( signed char * ) "CommReceive" , configMINIMAL_STACK_SIZE * 5, ( void * ) NULL,3, NULL );
}

void SendCallback( xTimerHandle pxTimer)
{
	SendCmdToMachine();
}

void MainCallback( xTimerHandle pxTimer)
{
	//DC_USART_PutChar('E');
	//自动关机，显示出错信息，给内机发送关机命令
}


