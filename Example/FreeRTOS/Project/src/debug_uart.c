#include "debug_uart.h"

/***
************************************************
函数名：UART_Configuration
参数  ： void
返回值： void
描述  ： 配置串口  
************************************************
***/
void uart_init( void )
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
	USART_InitStructure.USART_BaudRate = 9600;
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

void USART6_IRQHandler( void )
{
	if( USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == SET )
	{
		USART_ClearFlag( USART6, USART_FLAG_RXNE );
	}	 
}


void uart_putchar(const u8 ch)
{  
    USART_SendData(USART6, ch);
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);       				
}

void uart_putstr( char* pstr )
{
		u8 i = 0 , len ;
		len = strlen( pstr );
		while( i < len )
		{
			uart_putchar(pstr[i]);
			i++ ;
		}
		uart_putchar('\n');
}
/*
 ******************************************************************************
 *函数：uint8_t debug_getchar(uint8_t mode)
 *输入：uint8_t 串口接收到的字符
 *输出：mode    :  0  如果接受到数据返回字符如果没有接收到数据返回0
 *                 1  循环等待直到接收到数据，并返回该字符。
 *描述：调试串口读取字符
 ******************************************************************************
 */ 
uint8_t uart_getchar(uint8_t mode)
{
  uint8_t tmp = 0;
  
  if (mode) {
    while (USART_GetFlagStatus(DEBUG_UART, USART_FLAG_RXNE) == RESET);
    tmp = USART_ReceiveData(DEBUG_UART);
  } else {
    if (USART_GetFlagStatus(DEBUG_UART, USART_FLAG_RXNE)) {
      tmp = USART_ReceiveData(DEBUG_UART);
    }
  }
  return tmp;
}
#ifdef __MICROLIB
int fputc(int ch, FILE *f)
{
  uart_putchar(ch);
  return ch;
}

int fgetc(FILE *f)
{
  return uart_getchar(1);
}
#else

#endif

