//使用模拟I2C
#include "tem_rh.h"

/******************************************************************************* 
 * 函数名称:Tem_Rh_Delay                                                                      
 * 描    述:延时函数                                                                      
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者:                                                                      
 * 修改日期:2010年6月8日                                                                     
 *******************************************************************************/  
void Tem_Rh_NOP(void)  
{
 vu32 i, j;  
 vu32 sum = 0;  
 i = 5;  
 while(i--)  
 {
     for (j = 0; j < 100; j++)    // 10
     sum += i;   
 }
 i = sum;
 //sum = i;  
}
  
/******************************************************************************* 
 * 函数名称:Tem_Rh_Initialize                                                                                                                                                                                                          
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                                                                                                                                                        
 *******************************************************************************/  
void Tem_Rh_Gpio_Init(void)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
	RCC_AHB1PeriphClockCmd(TEM_RH_SDA_CLK | TEM_RH_SCL_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
  //GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 
  GPIO_InitStructure.GPIO_Pin = TEM_RH_SDA_PIN;  
  GPIO_Init(TEM_RH_SDA_GPIO, &GPIO_InitStructure);  
  
  //GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  
  GPIO_InitStructure.GPIO_Pin = TEM_RH_SCL_PIN;  
  GPIO_Init(TEM_RH_SCL_GPIO, &GPIO_InitStructure);  
    
  TEM_RH_SDA_1();  
  TEM_RH_SCL_0();   
}


/*----------------------------------------------------------------------------*/
//	内部函数----启动TEM_RH通信
/*----------------------------------------------------------------------------*/
void TEM_RH_Start(void)
{
	//SDA=1;		/*发送起始条件的数据信号*/
    TEM_RH_SDA_1();
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();

	//SCL=1;		/*发送起始条件的时钟信号*/
    TEM_RH_SCL_1();
	//nop;nop;nop;nop;nop;   //起始条件建立时间大于4.7us
    Tem_Rh_NOP();
    
	//SDA=0;		/*发送起始信号*/
    TEM_RH_SDA_0();
	//nop;nop;nop;nop;nop; //起始条件锁定时间大于4us
    Tem_Rh_NOP();

	//SCL=0;		/*钳住TEM_RH总线,准备发送或接收*/
    TEM_RH_SCL_0();
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
}

/*----------------------------------------------------------------------------*/
//	内部函数----停止TEM_RH通信
/*----------------------------------------------------------------------------*/
void TEM_RH_Stop(void) //释放TEM_RH总线*/
{
	//SDA=0;		/*发送停止条件的数据信号*/
    TEM_RH_SDA_0();
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
	//SCL=1;		//发送停止条件的时钟信号
    TEM_RH_SCL_1();
	//nop;nop;nop;nop;nop; //起始条件建立时间大于4us
    Tem_Rh_NOP();

	//SDA=1;		/*发送TEM_RH总线停止信号*/
    TEM_RH_SDA_1();
	//nop;nop;nop;nop;nop;/*停止条件锁定时间大于4us*/
    Tem_Rh_NOP();
}


/*-------------------------------------------------
内部函数--------Tem_Rh_SendAck(unsigned AckBit) 输出ACK 
 		AckBit=0 为发送应答信号 
		AckBit=1 为发送非应答信号 
---------------------------------------------------*/
void Tem_Rh_SendAck(uint8_t AckBit)
{
	//SDA=AckBit;		/*发送的应答或非应答信号*/
    if (AckBit == 0)
    {
        TEM_RH_SDA_0();
    } 
    else
    {
        TEM_RH_SDA_1();
    }
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
	//SCL=1;			/*置时钟线为高使应答位有效*/
    TEM_RH_SCL_1();
	//nop;nop;nop;nop;nop;   /*时钟高周期大于4us*/
    Tem_Rh_NOP();
	//SCL=0;			/*清时钟线钳住TEM_RH 总线以便继续接收字接数据或发送停止条件*/
    TEM_RH_SCL_0();
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
}

/*------------------------------------------
内部函数---------发送一个字节到TEM_RH总线上
	入口:待写入的数据ucdata ;无返回值 
-------------------------------------------*/
uint8_t TEM_RH_SendByte(uint8_t ucdata)
{
	uint8_t bACK;
	uint8_t  i;
	i=8;
	while(i--)			   	/*8 位没发送完继续发送*/
	{
		if((ucdata & 0x80)==0x80)
        {
			//SDA=1;		/*发送是1*/
            TEM_RH_SDA_1();
        }
		else
        {
			//SDA=0;		/*发送的是0*/
            TEM_RH_SDA_0();
        }
		//nop;nop;nop;nop;nop;
        Tem_Rh_NOP();
		//SCL=1;			/*置时钟线为高通知被控器开始接收数据位*/
        TEM_RH_SCL_1();
		//nop;nop;nop;nop;nop;  /*保证时钟高周期大于4us*/
        Tem_Rh_NOP();
		//SCL=0;			/*钳住总线准备接收下一个数据位*/
        TEM_RH_SCL_0();
		ucdata=ucdata<<1;  /*发送下一位数据*/
        //nop;nop;
        Tem_Rh_NOP();
	}
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
	//SDA=1;			/*8位数据发送完,释放TEM_RH总线,准备接收应答位*/
    TEM_RH_SDA_1();
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
	//SCL=1;			/*开始接收应答信号*/
    TEM_RH_SCL_1();
	//nop;nop;nop;nop;nop;
    Tem_Rh_NOP();
    // ?????????????????????????????????????????????????????????????????????????
	//if(SDA)		/*等待应答,SDA=0为应答*/      
	//	bACK=0;
	//else
	//	bACK=1;
    if (TEM_RH_SDA_STATE())
    {
       bACK = 0;
    }
	//while(!TEM_RH_SDA_STATE());
	//bACK = 0;
    else
    {
        bACK = 1;
    }
    
	//SCL=0;		/*发送结束钳住总线准备下一步发送或接收数据或进行其它处理*/
    TEM_RH_SCL_0();
	return(bACK);		/*正确应答返回0*/
}
	
/*------------------------------------------
//函数：从TEM_RH接收一个字节
//输入：无
//输出：无
//返回：接收到字节个数
--------------------------------------------*/
uint8_t TEM_RH_ReadByte(void)
{
	uint8_t i=0,ByteData=0;
	//SDA=1;			/*置数据线为输入方式*/
    TEM_RH_SDA_1();
	i=8;
	while(i--)
	{
	   	//nop;nop;nop;nop;
        Tem_Rh_NOP();
		//SCL=0;		//置钟线为零准备接收数据
        TEM_RH_SCL_0();
	//	Delay(3);
		//nop;nop;nop;nop;nop; /*时钟低周期大于4.7us*/
        Tem_Rh_NOP();
		//SCL=1;		/*置时钟线为高使数据线上数据有效*/
        TEM_RH_SCL_1();
		//nop;nop;nop;nop;nop;
        Tem_Rh_NOP();
		ByteData=ByteData<<1;
		//if(SDA)
		//	ByteData++;
        if (TEM_RH_SDA_STATE())
        {
            ByteData++;
        }
	}
	//SCL=0;			/*8 位接收完置时钟线和数据线为低准备发送应答或非应答信号*/
    TEM_RH_SCL_0();
	return(ByteData);
}


/****************************************************************************
 * 函数：读，有一个子地址
 * 输入：ucSla			--	I2C设备地址
 * 			 ucAddress 	--	命令/待操作寄存器
 * 			 ucNo				--	待读出数据个数
 * 输出：ucData			--	读出数据存放
 * 返回：0					--	读出失败
 *			 1					--	读出成功
****************************************************************************/
uint8_t TEM_RH_RdStr(uint8_t ucSla,uint8_t ucAddress, uint8_t *ucBuf,uint8_t ucCount)
{
	//从有子地址的器件读取数据
	// PCF8563的地址为0xa2,
	uint16_t waittime = TEM_RH_WAIT_DATA_READY_TIME;
	uint8_t i=0;

	TEM_RH_Start();	
	if(!TEM_RH_SendByte(ucSla))
	{
		TEM_RH_Stop();
		return 0;			/*选从器件的地此*/
	}
	if(!TEM_RH_SendByte(ucAddress))
	{
		TEM_RH_Stop();
		return 0;		/*选第一个寄存器地此*/
	}
		
	TEM_RH_Stop();//huangqin
	//nop;nop;
	
	while(--waittime && (!TEM_RH_SendByte(ucSla+1)))
	{
		Tem_Rh_NOP();
		Tem_Rh_NOP();
		Tem_Rh_NOP();

		TEM_RH_Start();
	}
	
	if(waittime == 0)
	{
		return 0;//规定时间内没有读到测量数据，失败
	}
	
	i=ucCount;
	while(i--)
	{
		*ucBuf=TEM_RH_ReadByte();/*读从器件寄存器*/
		if(i)
			Tem_Rh_SendAck(0);		/*未接收完所有字节,发送应答信号*/
		ucBuf++;
	}
	Tem_Rh_SendAck(1);			/*接收完所有字节,发送非应答信号*/
	TEM_RH_Stop();
	return 1;
}


/****************************************************************************
 * 函数：写，有一个字节子地址
 * 输入：ucSla			--	I2C设备地址
 * 			 ucAddress 	--	命令/待操作寄存器
 * 			 ucData			--	待写入数据
 * 			 ucNo				--	待写入数据个数
 * 输出：无
 * 返回：0					--	写入失败
 * 			 1					--	写入成功
****************************************************************************/
uint8_t TEM_RHWrStr(uint8_t ucSla, uint8_t ucAddress, uint8_t *ucData, uint8_t ucNo)
{	//8563的地址为0xa2
	uint8_t i;
	TEM_RH_Start();
	if(!TEM_RH_SendByte(ucSla))
	{
		TEM_RH_Stop();
		return 0;		/*write command*/
	}
	if(!TEM_RH_SendByte(ucAddress))
	{
		TEM_RH_Stop();
		return 0;	/*write register Address*/
	}
	i=ucNo;
	while(i--)
	{
		if(!TEM_RH_SendByte(*ucData))
		{
			TEM_RH_Stop();
			return 0;	/*write Data*/
		}
		ucData++;
	}
	TEM_RH_Stop();
	return 1;
}



/**************************************************
 * 函数：软复位器件，注意软复位后需要一定延时保证器件正常工作
 * 输入：无
 * 输出：无
 * 返回：void
 * 摄氏度 = (175.72*(float)Temper)/65536 - 46.85;
***************************************************/
uint8_t Tem_Rh_SoftRestart(void)
{
	TEM_RH_Start();
	if(!TEM_RH_SendByte(TEM_RH_ADDR))
	{
		TEM_RH_Stop();
		return 0;		/*write command*/
	}
	if(!TEM_RH_SendByte(TEM_RH_RESET_COMMAND))
	{
		TEM_RH_Stop();
		return 0;	/*write register Address*/
	}
	TEM_RH_Stop();
	return 1;
}

/**************************************************
 * 函数：读取温度值，需经换算得到摄氏度
 * 输入：无
 * 输出：Temper，温度原始值
 * 返回：void
 * 摄氏度 = (175.72*(float)Temper)/65536 - 46.85;
***************************************************/
void Tem_Rh_ReadTem(uint16_t *Temper)
{
	uint8_t temp[2];
	Tem_Rh_Gpio_Init();							//模拟I2C管脚初始化
	Tem_Rh_NOP();
	Tem_Rh_SoftRestart();
	ms_Delay(20);										//延时保证复位成功
	TEM_RH_RdStr(TEM_RH_ADDR, TEM_MEAS_COMMAND, &temp[0], 2);
	temp[1] &= 0xfc;								//低两位置零（默认测量结果为14bit）
	*Temper = (uint16_t)(temp[0]<<8) + temp[1];
}


/**************************************************
 * 函数：读取湿度值，需经换算得到相对湿度
 * 输入：无
 * 输出：Rh，湿度原始值
 * 返回：void
 * 相对湿度（%） = 125*Rh/65536 - 6;
***************************************************/
void Tem_Rh_ReadRh(uint16_t *Rh)
{
	uint8_t temp[2];
	Tem_Rh_Gpio_Init();							//模拟I2C管脚初始化
	Tem_Rh_NOP();
	Tem_Rh_SoftRestart();
	ms_Delay(20);										//延时保证复位成功
	TEM_RH_RdStr(TEM_RH_ADDR, RH_MEAS_COMMAND, &temp[0], 2);
	temp[1] &= 0xfc;								//低两位置零
	*Rh = (uint16_t)(temp[0]<<8) + temp[1];
}


//函数：测试函数3
void Tem_Rh_Test3(void)
{
	char c;
	double temp;
	uint16_t t = 0, r = 0;
	Tem_Rh_Gpio_Init();
	debug_printf("Temperature & Humidity test begin!\r\n");
	while(1)
	{
		//测量温度并打印
		Tem_Rh_ReadTem(&t);
		temp = (float)t;
		temp = (float)(175.72*temp/65536 - 46.85);
		debug_printf("Temperature: %.1f'C\t", temp);
		
		
		//测量湿度并打印
		Tem_Rh_ReadRh(&r);
		temp = (float)r;
		temp = 125*temp/65536 - 6;
		debug_printf("Humidity: %.1f%%\r\n", temp);
		
		ms_Delay(1000);
		//c = debug_getchar (0);
		if(c == 0)
		{
			continue;
		}
		else if(c == 't' || c == 'T')
		{
			debug_printf ("\nCmd> "); 
			debug_printf("%c\r\n", c);
			ms_Delay(200);
			debug_printf("Temperature & Humidity test continue!\r\n");
		}
		else
		{
			debug_printf ("\nCmd> "); 
			debug_printf("%c\r\n", c);
			ms_Delay(200);
			debug_printf("Temperature & Humidity test terminates!\r\n");
			break;
		}
	}
}
