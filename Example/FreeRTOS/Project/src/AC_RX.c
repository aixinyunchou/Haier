/***
************************************************
文件名：AC_TX.c
描述  ：交流模块接收数据与解析数据的任务
版本  ：version1.0		2014.9.5
************************************************
***/
#include "AC_Component.h"  
#include "AC_RX.h"
#include "string.h"
/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/
u8   Recv( void ) ;
void Analysis( u8 len ) ;
void AnalysisA1( void ) ;
void AnalysisA1Sub( void ) ;
void AnalysisA2( void ) ;
void AnalysisA3( void ) ;


/***
************************************************
函数名：
参数  ：
返回值：
描述  ：配置中断
************************************************
***/
void EXTI9_5_IRQHandler(void)
{

		if( (EXTI->IMR & EXTI_Line9) && ( EXTI->PR & EXTI_Line9 )  )
		{		
				exti9flag = 0;
				EXTI->PR = EXTI_Line9 ;
				return ;		
		}
	
		if( (EXTI->IMR & EXTI_Line8) && ( EXTI->PR & EXTI_Line8 )  )
		{
			xTaskCreate( vTaskRecv, (signed char *)"vTaskRecv", STACK_RECV,
											NULL, PRIOR_RECV, NULL ) ;
			EXTI->PR = EXTI_Line8 ;
			return ;
		}
}

void vTaskRecv(void* pvParam)
{
	u8 len ;
	Uart_putstr( "vTaskRecv start " ) ;
	Uart_putstr( "XXXXXXXXXXXXXXXXXXX" ) ;
	
	len = Recv() ;
	if( 3 > len )
		vTaskDelete( NULL ) ;
	if( recvedbuf[len-1] == CheckingSum((u8*)recvedbuf, len) )
	{
		if( 0xA5 == recvedbuf[0] )
		{
			Set_ack( 1 ) ;
			Set_time_e8() ;
			Analysis( len ) ;
		}
		else if( 0x5A == recvedbuf[0] )
			Ascend_primeframe() ;
		
		vTaskDelete( NULL ) ;
	}
}

u8  Recv( void )
{
	u8 i, j, k, endflag, chxflag ; 
	EXTI->IMR &= ~EXTI_Line8 ;
	Set_busy( 1 ) ;
	i = 0 ;
	endflag = 1 ;
	Uart_putchar( 'c' ) ;

	while( endflag && ( i < 10 ) )       
	{
		if( i )      
		{
			vTaskDelay( 2/portTICK_RATE_MS ) ;
			for( chxflag = 1, k = 0; chxflag && (k < 3); k++ )
			{
				if( !(GPIOB->IDR & GPIO_Pin_9 ) )
					chxflag = 0 ;
				vTaskDelay( 2/portTICK_RATE_MS ) ;
			}
			if( chxflag )
				while( GPIOB->IDR & GPIO_Pin_9 ) ;
			
			vTaskDelay( 2/portTICK_RATE_MS ) ;
			for( chxflag = 1, k = 0; chxflag && (k < 3); k++ )
			{
				if( (GPIOB->IDR & GPIO_Pin_9) )
					chxflag = 0 ;
				vTaskDelay( 2/portTICK_RATE_MS ) ;
			}
			if( chxflag )
				while( !(GPIOB->IDR & GPIO_Pin_9) ) ;
			
			vTaskDelay( 2/portTICK_RATE_MS ) ;
			if(GPIOB->IDR & GPIO_Pin_9)
			{
				if( GPIOB->IDR & GPIO_Pin_8 ) 
				{
					Uart_putchar( 'c' ) ;
				}
				else
				{
					endflag = 0 ;
					Uart_putchar( 'z' ) ;
					Set_busy( 0 ) ;
					Set_time_busy() ;
					EXTI->IMR |= EXTI_Line8 ;
					return i ;
				}
			}
			else
			{
				Uart_putchar( 'L' ) ;
			}
		}

		for( j = 0; j < 8; j++ )
		{
			vTaskDelay( 2/portTICK_RATE_MS ) ;
			for( chxflag = 1, k = 0; chxflag && (k < 3); k++ )
			{
				if( !(GPIOB->IDR & GPIO_Pin_9 ) )
						chxflag = 0 ;
				vTaskDelay( 2/portTICK_RATE_MS ) ;
			}
			if( chxflag )
				while( GPIOB->IDR & GPIO_Pin_9 ) ;  
			
			vTaskDelay( 2/portTICK_RATE_MS ) ;
			for( chxflag = 1, k = 0; chxflag && (k < 3); k++ )
			{
				if( (GPIOB->IDR & GPIO_Pin_9) )
						chxflag = 0 ;
				vTaskDelay( 2/portTICK_RATE_MS ) ;
			}
			if( chxflag )
				while( !(GPIOB->IDR & GPIO_Pin_9) ) ;
			
			vTaskDelay( 2/portTICK_RATE_MS ) ;
			if( GPIOB->IDR & GPIO_Pin_9 ) 
			{
				if( GPIOB->IDR & GPIO_Pin_8 ) 
				{
					recvedbuf[i] |= 0x01 << j ;
					Uart_putchar( 'a' ) ;		
				}
				else
				{
					recvedbuf[i] &= ~(0x01 << j) ;
					Uart_putchar( 'b' ) ;		
				}
			}
			else
			{
				Uart_putchar( 'l' ) ;
			}	
		}
		Uart_putchar( '\n' ) ;
		i++ ;
	}	
}


void Analysis( u8 len )
{
	if( Get_flag_sub() )  //主逻辑
	{
		if( 8 == len ) //A1回信
		{
			memcpy( (void*)&Buf_A1_IN, (void*)recvedbuf, 8 ) ;
			AnalysisA1() ;
		}
		else if( 9 == len )  //A2回信
		{
			memcpy( (void*)&Buf_A2_IN, (void*)recvedbuf, 9 ) ;
			AnalysisA2() ;
		}
		else if( 4 == len )   //A3回信
		{
			memcpy( (void*)&Buf_A3_IN, (void*)recvedbuf, 4 ) ;
			AnalysisA3() ;
		}
	}
	else  //从逻辑
	{
		if( (8 == len)	&& (	(recvedbuf[1] == 0x00) || (recvedbuf[1] == 0x08) ) ) //0#的A1回信(包括同报和个报)
		{
			memcpy( (void*)&Buf_A1_IN, (void*)recvedbuf, 8 ) ;
			AnalysisA1Sub() ;
		}
	}
}

void AnalysisA1( void )
{
	SPECIAL_SETTING temp ;  
	u8 id = (MAC_ID & Buf_A1_IN.feature_ID) >> 4 ;	
	
	if( ((u8*)&Buf_A1_IN)[1] & 0x08 )  //同报
	{
		Set_existlevel( 0 ) ;
	}
	else  //个报
	{
		Set_existlevel( id ) ;        
		if( Get_a1twice( id ) )  
		{
			temp.eding = (FETCH_EDING & Buf_A1_IN.btB) ; 
			temp.adress_allow_set = ( (FETCH_AAS & Buf_A1_IN.btD) >> 3 ) ;
			temp.jingyadangshu = ( FETCH_JYDS & Buf_A1_IN.btE ) ;
			
			if( 0 != memcmp( (void*) &(Registing[id].speset), (void*)&temp, sizeof(SPECIAL_SETTING) ) )
			{
				memcpy( (void*) &(Registing[id].speset), &temp, sizeof(SPECIAL_SETTING) ) ;
			}
			else
				Set_a1twice( id, 0 ) ;	
		}
		//获得其他上层数据
	}
}

void AnalysisA1Sub( void )
{
	//取得从逻辑需要的数据-
}

void AnalysisA2( void )
{
	u8 id = (MAC_ID & Buf_A2_IN.feature_ID) >> 4 ;
	
	Set_existlevel( id ) ;
	if( Get_a2param( id ) )   
	{
		memcpy( (void*) &(Registing[id].machineparam), ((u8*)&Buf_A2_IN) + 2, sizeof(MAC_PARAM) ) ;
		Set_a2param( id, 0 ) ;  
	}
	
	//获得其他数据
}

void AnalysisA3( void )
{
	u8 id = (MAC_ID & Buf_A3_IN.feature_ID) >> 4 ;
	
	Set_existlevel( id ) ;
	if( Get_here( id ) )  
	{
		
		
	}
	else
	{
		Set_here( id, 1 ) ;    
		Set_a1twice( id, 1 ) ;   
		Set_a2param( id, 1 ) ;    
	}	
}