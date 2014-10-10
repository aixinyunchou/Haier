/***
************************************************
�ļ�����AC_Component.c
����  ������ͨ�����ģ��������ģ�鹲ͬʹ�õ����������ʵ��
�汾  ��Version2.1          2014.8.16
************************************************
***/
#define  CPP_GLOBLE
#include "stm32f4xx.h"
#include "AC_Component.h"  
#include "string.h"

/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/


/***
************************************************
��������Get_Time_E8
����  �� void
����ֵ��u16  
����  �� ���E8����ʱ�õ������� time_e8, 100msΪ��λ
************************************************
***/
u16  Get_time_e8( void )
{
	return time_e8 ;
}


/***
************************************************
��������Set_TIme_E8
����  �� u16
����ֵ��void
����  �� �������E8��ʱ����0
************************************************
***/
void  Set_time_e8( void )
{
	time_e8 = 0 ;
}

/***
************************************************
��������Get_Time_Collide
����  ��  void
����ֵ�� u16
����  ��  �õ���ǰ����ͻ��ʱ��
************************************************
***/
u16  Get_time_collide( void )
{
	return time_collide ;
}



/***
************************************************
��������Set_TIme_Collide
����  �� void
����ֵ��void
����  �� ������ͻ��ʱ����0
************************************************
***/
void  Set_time_collide( void )
{
	time_collide = 0 ;
}


/***
************************************************
��������Get_Time_Busfree
����  �� void
����ֵ��u16
����  �� ���������ѿ���ʱ�䣬��100msΪ��λ
************************************************
***/
u16  Get_time_busy( void )
{
	return time_busy;
}

/***
************************************************
��������Set_Time_Busfree
����  �� void
����ֵ��void
����  �� �����߿���ʱ������Ϊ0
************************************************
***/
void Set_time_busy( void )
{
	time_busy = 0 ;
}


/***
************************************************
��������vTaskCntTime
����  �� void*
����ֵ�� void
����  ��  ��ʱ������100msΪ��λ
ע�⣺��ʱ����ʱ�϶����ȼ���ߣ������������У����⼸��ֵ����д֮ǰӦ�ùص���
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
��������Get_clientID
����  �� void
����ֵ�� u8               --����ǰ�߿����Ŀͻ���
����  ��  �����֡ʱ�����ô˺������ͻ���
************************************************
***/
u8   Get_clientID( void )
{
	return clientID ;
}


/***
************************************************
��������Set_clientID
����  �� u8                 
����ֵ��void
����  �� ����ģ�鿪��ʱ�����õ�ǰ�߿����Ŀͻ���
************************************************
***/
void Set_clientID( u8 id )
{
	clientID = id ;
}

/***
************************************************
��������Get_busy
����  �� void
����ֵ��u8
����  ����������æ��־  0 ��   1 æ
************************************************
***/
u8   Get_busy( void )
{
	return busy ;
}

/***
************************************************
��������Set_busy
����  �� void
����ֵ��void 
����  �� ��������æ����  1 æ  0 ��
************************************************
***/
void Set_busy( u8	temp )
{
	busy = temp ;
}

/***
************************************************
��������Ret_here
����  �� void
����ֵ��u16
����  �� ���� here ����
************************************************
***/
u16  Ret_here( void )
{
	return here ;
}

/***
************************************************
��������Get_here
����  �� u8         --��ָ��Ҫ����ĳһλ
����ֵ��u8         --�� 1  ����  0 ������
����  �� ���machine_id�����ڻ��Ƿ����
************************************************
***/
u8   Get_here( u8 machine_id )
{
	return here & ( 0x0001 << machine_id ) ;
}


/***
************************************************
�������� Set_here
���� 1 ��  u8         --��Ҫ�趨�����ڻ��ĺ�
       2:     u8       --��0  --��nbit�Ż���������   1  nbit�Ż�������
����ֵ�� void 
����  ��   ����machine_id�Ż����Ĵ���״̬
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
��������Get_a1twice
����  �� u8                 --��ָ��nbitλ����
����ֵ��u8                 --�� 1   û�о�������A1������ѯ  0  �Ѿ���...
����  ����ѯmachine_id�Ż����Ƿ��Ѿ���������A1������ѯ
************************************************
***/
u8   Get_a1twice( u8 machine_id )
{
	return a1twice & ( 0x0001 << machine_id ) ;	
}


/***
************************************************
��������Set_a1twice
����  �� u8          --������nbit�Ż��Ѿ�����������A1����
			--��flag   0  �Ѿ�ȡ�������趨  1  ��Ҫȡ�������趨
����ֵ��void
����  �� ����machine_id�Ż��Ƿ���Ҫ����A1����
		���յ�Find_NewMac�Ļ�Ӧ֡�У�ȷ�ϻ�������ʱ���ô��ڣ���ʱ������Ҫ��������趨��
		��A1������ȷ�ϻ���������趨֮�����������趨�Ѿ�ȡ��
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
�������� Get_a2param
����  ��  u8                  --��ָ�������������id
����ֵ�� u8		   --��0  �Ѿ�ȡ����nbit�Ż����Ĳ���  1  ��Ҫȡ��nbit�Ż����Ĳ���
����  ��  �����ڵĻ����Ƿ񾭹����Ҳ���
************************************************
***/
u8   Get_a2param( u8 machine_id )
{
	return a2param & ( 0x0001 << machine_id ) ;
}


/***
************************************************
�������� Set_a2param
����  ��  u8                  --��ָ�������������id
				  --��flag  0  �Ѿ�ȡ�ò�����1  ��Ҫȡ�ò���
����ֵ�� void
����  ��  �������machine_id�Ż����Ĳ����󣬽����Ӧ��λ��Ϊ0����nbit�Ż��������ں󣬽����Ӧλ����Ϊ1
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
�������� Get_newer
����  ��  void 
����ֵ�� u8           --�� 0  ��û�յ���ȷ�Ļ�Ӧ�� 1  ���յ���ȷ�Ļ�Ӧ
����  ��	�鿴�Ƿ��յ���ȷ�Ļ�Ӧ
************************************************
***/
u8   Get_ack( void )
{
	return ack ;
}

/***
************************************************
�������� Set_newer
����  ��  u8
����ֵ�� void
����  �� ��һ֡����ʱ������Ϊ0�����յ���֡�ظ���У����ȷ������Ϊ1 
************************************************
***/
void Set_ack( u8 temp )
{
	ack = temp ;
}

/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/
u8	 Get_existlevel( u8 machine_id )
{
	return existlevel[machine_id] ;
}

/***
************************************************
�������� Set_existlevel
����  ��  u8                              --��ָ����������ڻ�i
����ֵ�� void
����  ��  �����ڻ��Ĵ��ڶ�Ϊ2
		�ڽ��������У�ÿ�θ��ݻ��ţ�����Ӧ�����Ĵ��ڶ���2
************************************************
***/
void Set_existlevel( u8 machine_id )
{
	existlevel[machine_id] = 2 ;
}

/***
************************************************
�������� Descend_existlevel
����  ��  u8                              --��ָ����������ڻ�i
����ֵ�� void
����  ��  ���ɹ��������ĵ�û�յ���ȷ����ʱ���ã�������Ӧ�ڻ��Ĵ��ڶ�
************************************************
***/
void Descend_existlevel( u8 machine_id )
{
	if( existlevel[machine_id] )
		existlevel[machine_id]-- ;
}

/***
************************************************
��������Get_primeframe
����  �� void 
����ֵ�� u8
����  �� ���س�ͻ���������յ����߿�֡�Ĵ���
************************************************
***/
u8   Get_primeframe( void ) 
{
	return primeframe ;
}

/***
************************************************
�������� Set_primeframe
����  ��  u8
����ֵ�� void 
����  �� ���ó�ͻ���������յ����߿�֡�Ĵ���
************************************************
***/
void Set_primeframe( void )
{
	primeframe = 0 ;
}

/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/
void Ascend_primeframe( void )
{
	if( 0xFF != primeframe )
		primeframe++ ;
}


/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/
//u8   Get_flag_A3A2(void )
//{
//	return flag_A3A2 ;
//}

/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/
//void Set_flag_A3A2( u8 temp )
//{
//	flag_A3A2 = temp ;
//}

/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/
u8   Get_flag_aa(void )
{
	return flag_aa ;
}

/***
************************************************
��������
����  ��
����ֵ��
����  ��
************************************************
***/
void Set_flag_aa( u8 temp )
{
	flag_aa = temp ;
}

/***
************************************************
��������
����  ��
����ֵ��
����  ��������ʼ��ʱ����ֵλ1����⵽�Ǵ��߼�����Ϊ0
************************************************
***/
u8	 Get_flag_sub( void ) 
{
	return flag_sub ;
}

/***
************************************************
��������
����  ��
����ֵ��
����  ��������ʼ��ʱ����ֵλ1����⵽�Ǵ��߼�����Ϊ0
************************************************
***/
void Set_flag_sub( u8 temp ) 
{
	flag_sub = temp ;
}


/***
************************************************
�������� RCC_Configuration
����  ��  void
����ֵ�� void
����  ��  ����ʱ��
************************************************
***/
void RCC_Configuration( void ) 
{
	RCC_DeInit();              //RCC�Ĵ�����ʼ��
	RCC_HSEConfig(RCC_HSE_ON);  //ʹ���ⲿʱ��
	if (RCC_WaitForHSEStartUp() == SUCCESS) //�ȴ��ⲿʱ������
	{
		RCC_PLLCmd(DISABLE);                    //����PLLǰӦ�ȹر���PLL
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //ѡ��PLLʱ��Ϊϵͳʱ��
		RCC_HCLKConfig(RCC_SYSCLK_Div1);     //HCLK(AHB)ʱ��Ϊϵͳʱ��1��Ƶ
		RCC_PCLK1Config(RCC_HCLK_Div4);     //PCLK1(APB1)ʱ��ΪHCLKʱ��8��Ƶ����TIM2ʱ��ΪHCLKʱ��4��Ƶ
		RCC_PCLK2Config(RCC_HCLK_Div2);     //PCLK2(APB2)ʱ��ΪHCLKʱ��2��Ƶ
		RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 7); //PLLʱ�����ã���ʽ�����system_stm43f4xx.c�� Line149
		RCC_PLLCmd(ENABLE);        //PLLʱ�ӿ���
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) { } //�ȴ�PLLʱ��׼����
	}
}


/***
************************************************
��������TX_GPIO_Configuration
����  �� void
����ֵ��void
����  �� ����GPIO,TX��
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
��������RX_EXTI_Configuration
����  ��void 
����ֵ��void 
����  �� �����жϣ� 1����������ж� Pin9�� 2����
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
��������INPUT_EXTI_Configuration
����  ��void 
����ֵ��void 
����  �� �����жϣ� 1����������ж� Pin9�� 2����
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
��������UART_Configuration
����  �� void
����ֵ�� void
����  �� ���ô���   �������շ����д�ʵ�֣�
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
	
    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);     //�����յ�����ʱ�ж�
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
��������CheckingSum
����  1��u8[]             --������
	2��u8               --�����ĳ���
����ֵ��u8
����  �� ����У���
************************************************
***/
u8 CheckingSum( u8 temp[], u8 n )
{
	u16 sum = 0;
	u8 i = 0;
	for( ; i < n - 1; i++ )          //���һ�ֽ�ΪУ��ͣ�δ����ʱ������
		sum += temp[i] ;
	return ( u8 )( ~sum ) ;
}

/***
************************************************
��������Wait_busfree
����  �� u16  ���ʱ��   
����ֵ�� void
����  �����߿���xms,��100msΪ��λ
************************************************
***/
//void Wait_busfree( u16 time )
//{
//	u8 flag = 1 ;  //��û�еȵ����߿���time����ʱ��ʱΪ1 ��������߿���ʱ�䳤��time����Ϊ0 
//	while( flag )
//	{
//		while( Get_busy() ) ;   //����æ����ȴ���ֱ����
//		
//		while( ( !Get_busy() )  &&  ( Get_time_busy() < time ) ) ; //ѭ���˳�ʱ��Ҫô����ʱ��ﵽtime��Ҫô������æ
//		
//		if( Get_time_busy() >= time )
//			flag = 0 ;
//	}
//}

void Wait_busfree( u16 time )
{
	while( 1 )
	{
		//��ǰ����æ����ȴ���ֱ����
		while( 1 )  
		{
			if( Get_busy() )
				vTaskDelay( 20/portTICK_RATE_MS );
			else 
				break ;
		}
		
		//���ߴ�æ�䵽�У������е�ʱ��
		//ѭ���˳�ʱ��Ҫô����ʱ��ﵽtime��Ҫô������æ
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
��������  Init_Data
����  ��
����ֵ��
����  ��  ��ʼ��һЩȫ������
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
	
	xqh_Tpdata = xQueueCreate( 1, sizeof( TP_DATA ) ) ;     //TP_DATA��������֮�䴫����Ϣ�Ľṹ��
	//xqh_Tpdata = xQueueCreate( 1, sizeof( u8 ) ) ;					//�����ù�
	xqh_Tpframe = xQueueCreate( 1, sizeof(MSG_A1_OUT) ) ; 
	xqh_One = xQueueCreate( 1, sizeof(u8) ) ;
	xqh_Two = xQueueCreate( 1, sizeof(u8) ) ;
	
	ret_send = 0 ;
	th_PrimeLoc = NULL ;   				//���߼�����ľ��
	th_Primeframe_Send = NULL ;  	//�������߼�֡������ľ��
	th_Fill_Tpframe = NULL ;      //��֡����
	th_Tpframe_Send = NULL ;      //��Ļ֡��������ľ��
	
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




