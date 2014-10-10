/***
************************************************
文件名：AC_Component.c
描述  ：包含通信输出模块与输入模块共同使用的组件函数的实现
版本  ：Version2.1          2014.8.16
************************************************
***/
#define  CPP_GLOBLE
#include "stm32f4xx.h"
#include "AC_Component.h"  
#include "string.h"

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/


/***
************************************************
函数名：Get_Time_E8
参数  ： void
返回值：u16  
描述  ： 检测E8错误时用到，返回 time_e8, 100ms为单位
************************************************
***/
u16  Get_time_e8( void )
{
	return time_e8 ;
}


/***
************************************************
函数名：Set_TIme_E8
参数  ： u16
返回值：void
描述  ： 将检测检测E8的时间置0
************************************************
***/
void  Set_time_e8( void )
{
	time_e8 = 0 ;
}

/***
************************************************
函数名：Get_Time_Collide
参数  ：  void
返回值： u16
描述  ：  得到当前检测冲突的时间
************************************************
***/
u16  Get_time_collide( void )
{
	return time_collide ;
}



/***
************************************************
函数名：Set_TIme_Collide
参数  ： void
返回值：void
描述  ： 将检测冲突的时间置0
************************************************
***/
void  Set_time_collide( void )
{
	time_collide = 0 ;
}


/***
************************************************
函数名：Get_Time_Busfree
参数  ： void
返回值：u16
描述  ： 返回总线已空闲时间，以100ms为单位
************************************************
***/
u16  Get_time_busy( void )
{
	return time_busy;
}

/***
************************************************
函数名：Set_Time_Busfree
参数  ： void
返回值：void
描述  ： 将总线空闲时间设置为0
************************************************
***/
void Set_time_busy( void )
{
	time_busy = 0 ;
}


/***
************************************************
函数名：vTaskCntTime
参数  ： void*
返回值： void
描述  ：  计时任务，以100ms为单位
注意：计时任务到时肯定优先级最高，在其他任务中，对这几个值进行写之前应该关调度
************************************************
***/
void vTaskCntTime( void* pvParam )
{
		Uart_putstr( "vTaskCntTime start " ) ;
	while( 1 )
	{
		vTaskDelay( 100/portTICK_RATE_MS ) ;
		if(	0xFFFF != time_e8 )
			time_e8 ++ ;
		if( 0xFFFF != time_busy )
			time_busy++ ;
		if( 0XFFFF != time_collide )
			time_collide++ ;
	}
}

/***
************************************************
函数名：Get_clientID
参数  ： void
返回值： u8               --》当前线控器的客户码
描述  ：  在填充帧时，调用此函数填充客户码
************************************************
***/
u8   Get_clientID( void )
{
	return clientID ;
}


/***
************************************************
函数名：Set_clientID
参数  ： u8                 
返回值：void
描述  ： 交流模块开启时，设置当前线控器的客户码
************************************************
***/
void Set_clientID( u8 id )
{
	clientID = id ;
}

/***
************************************************
函数名：Get_busy
参数  ： void
返回值：u8
描述  ：返回总线忙标志  0 闲   1 忙
************************************************
***/
u8   Get_busy( void )
{
	return busy ;
}

/***
************************************************
函数名：Set_busy
参数  ： void
返回值：void 
描述  ： 设置总线忙或闲  1 忙  0 闲
************************************************
***/
void Set_busy( u8	temp )
{
	busy = temp ;
}

/***
************************************************
函数名：Ret_here
参数  ： void
返回值：u16
描述  ： 返回 here 整体
************************************************
***/
u16  Ret_here( void )
{
	return here ;
}

/***
************************************************
函数名：Get_here
参数  ： u8         --》指定要检测的某一位
返回值：u8         --》 1  存在  0 不存在
描述  ： 检查machine_id号室内机是否存在
************************************************
***/
u8   Get_here( u8 machine_id )
{
	return here & ( 0x0001 << machine_id ) ;
}


/***
************************************************
函数名： Set_here
参数 1 ：  u8         --》要设定的室内机的号
       2:     u8       --》0  --》nbit号机器不存在   1  nbit号机器存在
返回值： void 
描述  ：   设置machine_id号机器的存在状态
************************************************
***/
void Set_here( u8 machine_id, u8 temp )
{
	if( temp )
		here |= (0x0001 << machine_id) ;
	else
		here &= ~(0x0001 << machine_id) ;
}


/***
************************************************
函数名：Get_a1twice
参数  ： u8                 --》指定nbit位机器
返回值：u8                 --》 1   没有经过两次A1个报查询  0  已经过...
描述  ：查询machine_id号机器是否已经经过两次A1个报查询
************************************************
***/
u8   Get_a1twice( u8 machine_id )
{
	return a1twice & ( 0x0001 << machine_id ) ;	
}


/***
************************************************
函数名：Set_a1twice
参数  ： u8          --》设置nbit号机已经经过了两次A1个报
			--》flag   0  已经取得特殊设定  1  需要取得特殊设定
返回值：void
描述  ： 设置machine_id号机是否需要两次A1个报
		在收到Find_NewMac的回应帧中，确认机器上线时设置存在，此时设置需要获得特殊设定；
		在A1个报中确认获得了特殊设定之后，设置特殊设定已经取得
************************************************
***/
void Set_a1twice( u8 machine_id, u8 flag )
{
	if( flag )
		a1twice |= (0x0001 << machine_id ) ;
	else 
		a1twice &= ~( 0x0001 << machine_id ) ;
}


/***
************************************************
函数名： Get_a2param
参数  ：  u8                  --》指定操作对象机器id
返回值： u8		   --》0  已经取得了nbit号机器的参数  1  需要取得nbit号机器的参数
描述  ：  检测存在的机器是否经过查找参数
************************************************
***/
u8   Get_a2param( u8 machine_id )
{
	return a2param & ( 0x0001 << machine_id ) ;
}


/***
************************************************
函数名： Set_a2param
参数  ：  u8                  --》指定操作对象机器id
				  --》flag  0  已经取得参数，1  需要取得参数
返回值： void
描述  ：  当获得了machine_id号机器的参数后，将其对应的位设为0，当nbit号机器不存在后，将其对应位设置为1
************************************************
***/
void Set_a2param( u8 machine_id, u8 flag )
{
	if( flag )
		a2param |= ( 0x0001 << machine_id ) ;
	else
		a2param &= ~( 0x0001 << machine_id ) ; 
}

/***
************************************************
函数名： Get_newer
参数  ：  void 
返回值： u8           --》 0  还没收到正确的回应， 1  已收到正确的回应
描述  ：	查看是否收到正确的回应
************************************************
***/
u8   Get_ack( void )
{
	return ack ;
}

/***
************************************************
函数名： Set_newer
参数  ：  u8
返回值： void
描述  ： 当一帧发出时，设置为0，当收到该帧回复并校验正确后设置为1 
************************************************
***/
void Set_ack( u8 temp )
{
	ack = temp ;
}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
u8	 Get_existlevel( u8 machine_id )
{
	return existlevel[machine_id] ;
}

/***
************************************************
函数名： Set_existlevel
参数  ：  u8                              --》指向操作的室内机i
返回值： void
描述  ：  设置内机的存在度为2
		在解析报文中，每次根据机号，将相应机器的存在度置2
************************************************
***/
void Set_existlevel( u8 machine_id )
{
	existlevel[machine_id] = 2 ;
}

/***
************************************************
函数名： Descend_existlevel
参数  ：  u8                              --》指向操作的室内机i
返回值： void
描述  ：  当成功发出报文但没收到正确回信时调用，降低响应内机的存在度
************************************************
***/
void Descend_existlevel( u8 machine_id )
{
	if( existlevel[machine_id] )
		existlevel[machine_id]-- ;
}

/***
************************************************
函数名：Get_primeframe
参数  ： void 
返回值： u8
描述  ： 返回冲突发生后，又收到主线控帧的次数
************************************************
***/
u8   Get_primeframe( void ) 
{
	return primeframe ;
}

/***
************************************************
函数名： Set_primeframe
参数  ：  u8
返回值： void 
描述  ： 设置冲突发生后，又收到主线控帧的次数
************************************************
***/
void Set_primeframe( void )
{
	primeframe = 0 ;
}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
void Ascend_primeframe( void )
{
	if( 0xFF != primeframe )
		primeframe++ ;
}


/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
//u8   Get_flag_A3A2(void )
//{
//	return flag_A3A2 ;
//}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
//void Set_flag_A3A2( u8 temp )
//{
//	flag_A3A2 = temp ;
//}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
u8   Get_flag_aa(void )
{
	return flag_aa ;
}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
void Set_flag_aa( u8 temp )
{
	flag_aa = temp ;
}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：正常初始化时，该值位1，检测到是从逻辑就设为0
************************************************
***/
u8	 Get_flag_sub( void ) 
{
	return flag_sub ;
}

/***
************************************************
函数名：
参数  ：
返回值：
描述  ：正常初始化时，该值位1，检测到是从逻辑就设为0
************************************************
***/
void Set_flag_sub( u8 temp ) 
{
	flag_sub = temp ;
}


/***
************************************************
函数名： RCC_Configuration
参数  ：  void
返回值： void
描述  ：  配置时钟
************************************************
***/
void RCC_Configuration( void ) 
{
	RCC_DeInit();              //RCC寄存器初始化
	RCC_HSEConfig(RCC_HSE_ON);  //使用外部时钟
	if (RCC_WaitForHSEStartUp() == SUCCESS) //等待外部时钟启动
	{
		RCC_PLLCmd(DISABLE);                    //配置PLL前应先关闭主PLL
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //选择PLL时钟为系统时钟
		RCC_HCLKConfig(RCC_SYSCLK_Div1);     //HCLK(AHB)时钟为系统时钟1分频
		RCC_PCLK1Config(RCC_HCLK_Div4);     //PCLK1(APB1)时钟为HCLK时钟8分频，则TIM2时钟为HCLK时钟4分频
		RCC_PCLK2Config(RCC_HCLK_Div2);     //PCLK2(APB2)时钟为HCLK时钟2分频
		RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 7); //PLL时钟配置，公式详见‘system_stm43f4xx.c’ Line149
		RCC_PLLCmd(ENABLE);        //PLL时钟开启
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) { } //等待PLL时钟准备好
	}
}


/***
************************************************
函数名：TX_GPIO_Configuration
参数  ： void
返回值：void
描述  ： 配置GPIO,TX口
************************************************
***/
void TX_GPIO_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );

	GPIO_StructInit( &GPIO_InitStructure ) ;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init( GPIOB, &GPIO_InitStructure );
}


/***
************************************************
函数名：RX_EXTI_Configuration
参数  ：void 
返回值：void 
描述  ： 配置中断（ 1、过零检测的中断 Pin9。 2、）
************************************************
***/
void RX_EXTI_Configuration( void )
{
	GPIO_InitTypeDef   GPIO_InitStructure ;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	GPIO_StructInit( &GPIO_InitStructure ) ;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init( GPIOB, &GPIO_InitStructure );
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8);
	
	EXTI_StructInit( &EXTI_InitStructure ) ;
	EXTI_InitStructure.EXTI_Line = EXTI_Line8 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt ;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE ;
	EXTI_Init( &EXTI_InitStructure ) ;
	EXTI->IMR &= ~EXTI_Line8 ;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
}

/***
************************************************
函数名：INPUT_EXTI_Configuration
参数  ：void 
返回值：void 
描述  ： 配置中断（ 1、过零检测的中断 Pin9。 2、）
************************************************
***/
void INPUT_EXTI_Configuration( void )
{
	GPIO_InitTypeDef   GPIO_InitStructure ;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	GPIO_StructInit( &GPIO_InitStructure ) ;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init( GPIOB, &GPIO_InitStructure );
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
	
	EXTI_StructInit( &EXTI_InitStructure ) ;
	EXTI_InitStructure.EXTI_Line = EXTI_Line9 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt ;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE ;
	EXTI_Init( &EXTI_InitStructure ) ;
	EXTI->IMR &= ~EXTI_Line9 ;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);		
}



/***
************************************************
函数名：UART_Configuration
参数  ： void
返回值： void
描述  ： 配置串口   （数据收发还有待实现）
************************************************
***/
void Uart_Configuration( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure ;
	USART_InitTypeDef USART_InitStructure; 
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure ;
	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART6, ENABLE ) ;
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource6 , GPIO_AF_USART6 );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_USART6 );
		  
	GPIO_StructInit( &GPIO_InitStructure ) ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;   
    GPIO_Init( GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;   
    GPIO_Init( GPIOC, &GPIO_InitStructure);
	
	USART_StructInit( &USART_InitStructure ) ;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USART6, &USART_InitStructure ); 
	USART_ClockStructInit( &USART_ClockInitStructure ) ;
	USART_ClockInit( USART6, &USART_ClockInitStructure ) ;
	
			NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);     //当接收到数据时中断
    USART_Cmd(USART6,ENABLE);
	USART_ClearFlag( USART6, USART_FLAG_RXNE ) ;
}

//void USART6_IRQHandler( void )
//{
//	if( USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == SET )
//	{
//		xQueueSend( xqh_Tpframe, (void*)NULL, 0 ) ;
//		Uart_putchar( 'x' ) ;
//		USART_ClearFlag( USART6, USART_FLAG_RXNE ) ;
//	}	 
//}

void Uart_sendingbuf( void )
{  
	u8 i ;
	for( i = 0; i < 10; i++ )
	{
            USART_SendData(USART6, sendingbuf[i]);
            while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);       				
	}
}

void Uart_putchar( u8 ch )
{  
    USART_SendData(USART6, ch);
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);       				
}

void Uart_putstr( char* pstr )
{
		u8 i = 0 , len ;
		len = strlen( pstr ) ;
		while( i < len )
		{
			Uart_putchar( pstr[i] ) ;
			i++ ;
		}
		Uart_putchar( '\n' ) ;
}

/***
************************************************
函数名：CheckingSum
参数  1：u8[]             --》报文
	2：u8               --》报文长度
返回值：u8
描述  ： 计算校验和
************************************************
***/
u8 CheckingSum( u8 temp[], u8 n )
{
	u16 sum = 0;
	u8 i = 0;
	for( ; i < n - 1; i++ )          //最后一字节为校验和，未填入时不计算
		sum += temp[i] ;
	return ( u8 )( ~sum ) ;
}

/***
************************************************
函数名：Wait_busfree
参数  ： u16  间隔时间   
返回值： void
描述  ：总线空闲xms,以100ms为单位
************************************************
***/
//void Wait_busfree( u16 time )
//{
//	u8 flag = 1 ;  //当没有等到总线空闲time长的时间时为1 ，如果总线空闲时间长达time，则为0 
//	while( flag )
//	{
//		while( Get_busy() ) ;   //总线忙，则等待，直到闲
//		
//		while( ( !Get_busy() )  &&  ( Get_time_busy() < time ) ) ; //循环退出时，要么空闲时间达到time，要么总线又忙
//		
//		if( Get_time_busy() >= time )
//			flag = 0 ;
//	}
//}

void Wait_busfree( u16 time )
{
	while( 1 )
	{
		//当前总线忙，则等待，直到闲
		while( 1 )  
		{
			if( Get_busy() )
				vTaskDelay( 20/portTICK_RATE_MS );
			else 
				break ;
		}
		
		//总线从忙变到闲，则检测闲的时间
		//循环退出时，要么空闲时间达到time，要么总线又忙
		while( !Get_busy() ) 
		{
			if( Get_time_busy() < time )
				vTaskDelay( 20/portTICK_RATE_MS );
			else
				break ;
		}
		if( Get_time_busy() >= time )
			break  ;
	}
}







/***
************************************************
函数名：  Init_Data
参数  ：
返回值：
描述  ：  初始化一些全局数据
************************************************
***/
void Init_Data( void )
{
	u8 i ;
	//ac_component 
	
	
	//TX 
	memset( (void*)&Buf_A1_OUT, 0, sizeof(MSG_A1_OUT) ) ;
	memset( (void*)&Buf_A2_OUT, 0, sizeof(MSG_A2_OUT) ) ;
	memset( (void*)&Buf_A3_OUT, 0, sizeof(MSG_A3_OUT) ) ;
	memset( (void*)&Buf_A4_OUT, 0, sizeof(MSG_A4_OUT) ) ;
	memset( (void*)&Buf_A1_OUT_Prime, 0, sizeof(MSG_A1_OUT) ) ;
	memset( (void*)sendingbuf, 0, 10*sizeof(u8) ) ;
	
	//RX
	memset( (void*)&Buf_A1_IN, 0, sizeof(MSG_A1_IN) ) ;
	memset( (void*)&Buf_A2_IN, 0, sizeof(MSG_A2_IN) ) ;
	memset( (void*)&Buf_A3_IN, 0, sizeof(MSG_A3_IN) ) ;
	memset( (void*)recvedbuf, 0, 10*sizeof(u8) ) ;
	
	 
	for( i = 0; i < 16; i++ )
		memset( (void*)&Registing[i], 0, sizeof(REGIST) ) ;
	
	xqh_Tpdata = xQueueCreate( 1, sizeof( TP_DATA ) ) ;     //TP_DATA是与马总之间传递信息的结构体
	//xqh_Tpdata = xQueueCreate( 1, sizeof( u8 ) ) ;					//测试用过
	xqh_Tpframe = xQueueCreate( 1, sizeof(MSG_A1_OUT) ) ; 
	xqh_One = xQueueCreate( 1, sizeof(u8) ) ;
	xqh_Two = xQueueCreate( 1, sizeof(u8) ) ;
	
	ret_send = 0 ;
	th_PrimeLoc = NULL ;   				//主逻辑任务的句柄
	th_Primeframe_Send = NULL ;  	//发送主逻辑帧的任务的句柄
	th_Fill_Tpframe = NULL ;      //填帧任务
	th_Tpframe_Send = NULL ;      //屏幕帧发送任务的句柄
	
	Set_clientID( CLT_ID_PRIMECTL ) ;
	here = 0 ;
	Set_flag_aa( 1 ) ;
	Set_flag_sub( 1 ) ;
	
	xsh_flag_aa = xSemaphoreCreateMutex() ;
}


void E8guzhang( void ) 
{
	return ;
}

void tongxunguzhang( void ) 
{
	return ;
}
/*
__asm void wait()
{
      BX lr
}

 

void HardFault_Handler(void)
{
       wait();
}
*/




