//模拟标准I2C
#include "i2c.h"  
  
 
void TWI_Initialize(void);
   
  
/******************************************************************************* 
 * 函数名称:TWI_Delay                                                                      
 * 描    述:延时函数                                                                      
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者:                                                                      
 * 修改日期:2010年6月8日                                                                     
 *******************************************************************************/  
void TWI_NOP(void)  
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
 * 函数名称:TWI_Initialize                                                                      
 * 描    述:I2C初始化函数                                                                      
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者:                                                                      
 * 修改日期:2010年6月8日                                                                     
 *******************************************************************************/  
void TWI_Initialize(void)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
	RCC_AHB1PeriphClockCmd(I2C_SDA_CLK | I2C_SCL_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
  //GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;  
  GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);  
  
  //GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;  
  GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);  
    
  TWI_SDA_1();  
  TWI_SCL_0();   
}

/******************************************************************************* 
 * 函数名称:TWI_Initialize                                                                      
 * 描    述:I2C初始化函数                                                                      
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者:                                                                      
 * 修改日期:2010年6月8日                                                                     
 *******************************************************************************/  
void TWI_Init_Tem_Rh(void)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
	RCC_AHB1PeriphClockCmd(I2C_SDA_CLK | I2C_SCL_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
  //GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;  
  GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);  
  
  //GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;  
  GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);  
    
  TWI_SDA_1();  
  TWI_SCL_0();   
}


/*----------------------------------------------------------------------------*/
//	内部函数----启动I2C通信
/*----------------------------------------------------------------------------*/
void I2CStart(void)
{
	TWI_SDA_1();			/*发送起始条件的数据信号*/
	TWI_NOP();
	TWI_SCL_1();			/*发送起始条件的时钟信号*/
	TWI_NOP();				//起始条件建立时间大于4.7us
	TWI_SDA_0();			/*发送起始信号*/
	TWI_NOP();				//起始条件锁定时间大于4us		
	TWI_SCL_0();			/*钳住I2C总线,准备发送或接收*/
	TWI_NOP();
}

/*----------------------------------------------------------------------------*/
//	内部函数----停止I2C通信
/*----------------------------------------------------------------------------*/
void I2CStop(void) //释放I2C总线*/
{
	TWI_SDA_0();			/*发送停止条件的数据信号*/
	TWI_NOP();
	TWI_SCL_1();			//发送停止条件的时钟信号
	TWI_NOP();				//起始条件建立时间大于4us	
	TWI_SDA_1();			/*发送I2C总线停止信号*/
	TWI_NOP();				/*停止条件锁定时间大于4us*/
}


/*-------------------------------------------------
内部函数--------SendAck(unsigned AckBit) 输出ACK 
 		AckBit=0 为发送应答信号 
		AckBit=1 为发送非应答信号 
---------------------------------------------------*/
void SendAck(uint8_t AckBit)
{
	/*发送的应答或非应答信号*/
	if (AckBit == 0)
	{
		TWI_SDA_0();
	} 
	else
	{
		TWI_SDA_1();
	}
	TWI_NOP();
	TWI_SCL_1();			/*置时钟线为高使应答位有效*/
	TWI_NOP(); 				/*时钟高周期大于4us*/
	TWI_SCL_0();			/*清时钟线钳住I2C 总线以便继续接收字接数据或发送停止条件*/
	TWI_NOP();
}

/*------------------------------------------
内部函数---------发送一个字节到I2C总线上
	入口:待写入的数据ucdata ;无返回值 
-------------------------------------------*/
uint8_t I2CSendByte(uint8_t ucdata)
{
	uint8_t bACK;
	uint8_t  i;
	i=8;
	while(i--)			   	/*8 位没发送完继续发送*/
	{
		if((ucdata & 0x80)==0x80)
		{
			TWI_SDA_1();		/*发送是1*/
		}
		else
		{
			TWI_SDA_0();		/*发送的是0*/
		}
		TWI_NOP();
		TWI_SCL_1();			/*置时钟线为高通知被控器开始接收数据位*/
		TWI_NOP();				/*保证时钟高周期大于4us*/	
		TWI_SCL_0();			/*钳住总线准备接收下一个数据位*/
		ucdata=ucdata<<1; /*发送下一位数据*/
		TWI_NOP();
	}
	TWI_NOP();
	TWI_SDA_1();				/*8位数据发送完,释放I2C总线,准备接收应答位*/
	TWI_NOP();
	TWI_SCL_1();				/*开始接收应答信号*/
	TWI_NOP();
	if (TWI_SDA_STATE())
	{
		 bACK = 0;				/*等待应答,SDA=0为应答*/
	}
	else
	{
			bACK = 1;				/*等待应答,SDA=1为应答*/
	}	
	TWI_SCL_0();				/*发送结束钳住总线准备下一步发送或接收数据或进行其它处理*/
	return(bACK);				/*正确应答返回0*/
}
	
/*------------------------------------------
内部函数------从I2C接收一个字节
		无入口参数;返回接收到的字节.
--------------------------------------------*/
uint8_t I2CReadByte(void)
{
	uint8_t i=0,ByteData=0;	
	TWI_SDA_1();				/*置数据线为输入方式*/
	i=8;
	while(i--)
	{
		TWI_NOP();
		TWI_SCL_0();			//置钟线为零准备接收数据
		TWI_NOP();				/*时钟低周期大于4.7us*/	
		TWI_SCL_1();			/*置时钟线为高使数据线上数据有效*/
		TWI_NOP();
		ByteData=ByteData<<1;
		if (TWI_SDA_STATE())
		{
				ByteData++;
		}
	}		
	TWI_SCL_0();				/*8 位接收完置时钟线和数据线为低准备发送应答或非应答信号*/
	return(ByteData);
}

// 读，有一个子地址
uint8_t I2CRdStr(uint8_t ucSla,uint8_t ucAddress,uint8_t *ucBuf,uint8_t ucCount)
{
	//从有子地址的器件读取数据
	uint8_t i=0;
	I2CStart();	
	if(!I2CSendByte(ucSla))
	{
		I2CStop();
		return 0;					/*选从器件的地此*/
	}
	if(!I2CSendByte(ucAddress))
	{
		I2CStop();
		return 0;					/*选第一个寄存器地此*/
	}
	I2CStop();					//huangqin
	TWI_NOP();

	I2CStart();
	if(!I2CSendByte(ucSla+1))
	{
		I2CStop();
		return 0;					/*发送读器件命令*/
	}
	i=ucCount;
	while(i--)
	{
		*ucBuf=I2CReadByte();/*读从器件寄存器*/
		if(i)
			SendAck(0);			/*未接收完所有字节,发送应答信号*/
		ucBuf++;
	}
	SendAck(1);					/*接收完所有字节,发送非应答信号*/
	I2CStop();
	return 1;
}

// 写，有一个子地址
uint8_t I2CWrStr(uint8_t ucSla, uint8_t ucAddress, uint8_t *ucData, uint8_t ucNo)
{
	uint8_t i;
	I2CStart();
	if(!I2CSendByte(ucSla))
	{
		I2CStop();
		return 0;					/*write command*/
	}
	if(!I2CSendByte(ucAddress))
	{
		I2CStop();
		return 0;					/*write register Address*/
	}
	i=ucNo;
	while(i--)
	{
		if(!I2CSendByte(*ucData))
		{
			I2CStop();
			return 0;				/*write Data*/
		}
		ucData++;
	}
	I2CStop();
	return 1;
}

// 读，有双字节子地址
uint8_t I2CRdStr2(uint8_t ucSla,uint16_t usAddress, uint8_t *ucBuf,uint8_t ucCount)
{
	//从有子地址的器件读取数据
	uint8_t i=0;

	    I2CStart();	
	    if(!I2CSendByte(ucSla))
	    {
		    I2CStop();
		    return 0;			/*选从器件的地此*/
	    }
			if(!I2CSendByte((usAddress >> 8) && 0x00FF))
	    //if(!I2CSendByte(usAddress / 256))
	    {
		    I2CStop();
		    return 0;		/*选第一个寄存器地此*/
	    }
			if(!I2CSendByte(usAddress))
      //if(!I2CSendByte(usAddress % 256))
	    {
		    I2CStop();
		    return 0;		/*选第二个寄存器地此*/
	    }

        I2CStop();//huangqin
        //nop;nop;
        TWI_NOP();

	I2CStart();
	if(!I2CSendByte(ucSla+1))
	{
		I2CStop();
		return 0;							/*发送读器件命令*/
	}
	i=ucCount;
	while(i--)
	{
		*ucBuf=I2CReadByte();	/*读从器件寄存器*/
		if(i)
			SendAck(0);					/*未接收完所有字节,发送应答信号*/
		ucBuf++;
	}
	SendAck(1);							/*接收完所有字节,发送非应答信号*/
	I2CStop();
	return 1;
}

// 写，有双字节子地址
uint8_t I2CWrStr2C(uint8_t ucSla,uint16_t usAddress, uint8_t *ucData, uint8_t ucNo)
{	//8563的地址为0xa2
	uint8_t i;
	I2CStart();
	if(!I2CSendByte(ucSla))
	{
		I2CStop();
		return 0;		/*write command*/
	}
	if(!I2CSendByte((usAddress >> 8) && 0x00FF))
  //if(!I2CSendByte(usAddress / 256))
	{
		I2CStop();
		return 0;							/*write register Address*/
	}
	if(!I2CSendByte(usAddress))
  //if(!I2CSendByte(usAddress % 256))
	{
		I2CStop();
		return 0;							/*选第二个寄存器地此*/
	}

	i=ucNo;
	while(i--)
	{
		if(!I2CSendByte(*ucData))
		{
			I2CStop();
			return 0;	/*write Data*/
		}
		ucData++;
	}
	I2CStop();
	return 1;
}

// 写，有双字节子地址
uint8_t I2CWrStr2(uint8_t ucSla,uint16_t usAddress, uint8_t *ucData, uint8_t ucNo)
{
	uint8_t i;
	uint8_t ucRt;
	uint16_t j;

	for (i = 0; i < ucNo; i++)
	{
		ucRt = I2CWrStr2C(ucSla, (usAddress + i), (ucData + i), 1);
		if (ucRt == 0)
		{
				return 0;
		}
		
		for (j = 60000; j > 0; j--)  // 延时要足够才能保证写数据正常
		{
		}
	}

	return 1;
}
