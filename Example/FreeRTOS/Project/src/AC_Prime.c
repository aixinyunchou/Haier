/***
************************************************
文件名：AC_Prime.c
描述  ：主逻辑
版本  ：version1.0		2014.9.5
************************************************
***/
#include "stm32f4xx.h"
#include "AC_TX.h"
#include "AC_Component.h"
#include "FreeRTOS.H"
#include "task.h"
/***
************************************************
函数名：
参数  ：
返回值：
描述  ：
************************************************
***/

u8  Find_Zero( void ) ;     
u8	Find_NewMac( u8 id ) ;
u8	Putout_A1( void ) ;
u8  Fetch_Speset( u8 id ) ;
u8  Fetch_Params( u8 id ) ;
u8  Putout_A3( u8 id ) ;
u8  Putout_A2( u8 id ) ;


u8  Find_Zero( void )
{
	u8 sendflag , i,j ;
	FillA3( 0 ) ;
	send_param.pbuf = (u8*)&Buf_A3_OUT ;
	send_param.szbuf = 3 ;
		
loop1:
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	Wait_busfree( THREE_HUNDRED_MS ) ;
loop2:
		
	xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
							 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
	xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	
	if( sendflag )    
	{
		if( (sendflag < 3) || (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag)) )
		{
			vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别
			goto loop2 ;
		}
					
		if( 0x5A == sendingbuf[0] )  
		{
			for( j = 0; j < 4; j++ )
			{
				Set_time_collide() ;
				Set_primeframe() ;
				while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
				
				if( Get_primeframe() >= 2 )     
				{
					return ERR_COLLIDE ;
				}      
				else if(! Get_primeframe() )
				{
					goto loop1 ;
				}
			}
			if( 4 == j )
			{
				return ERR_COLLIDE ;
			}
		}
		else                          
			goto loop1 ;
	}
	else           
	{
		while( ! Get_ack() )
		{
			if( ( ! Get_busy() ) && ( Get_time_busy() >= THREE_HUNDRED_MS ) ) 
				goto loop1 ; 	
			vTaskDelay( 50/portTICK_RATE_MS ) ;
		}        	
		return 0 ;
	}
}


u8	Find_NewMac( u8 id ) 
{
	u8 i, j, sendflag ;

	FillA3( id ) ;  
	send_param.pbuf = (u8*)&Buf_A3_OUT ;
	send_param.szbuf = 3 ;
	
loop1:             
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	i = 2 ;
	Wait_busfree( THREE_HUNDRED_MS ) ;
	
loop2:
	xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
							 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
	xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	
	if( sendflag )    
	{
		if( ERR_TP == sendflag )
			goto loop1 ;
		if( (sendflag < 3) || (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag - 1)) )  
		{
			i-- ;
			if( i )
			{
				vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别
				goto loop2 ;
			}	
			else
			{
				Set_here( id, 0 ) ;    
				return ERR_NO_FIND ;
			}
		}
		 	
		if( 0x5A == sendingbuf[0] )  
		{
			for( j = 0; j < 4; j++ )
			{
				Set_time_collide() ;
				Set_primeframe() ;
				while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
				
				if( Get_primeframe() >= 2 )     
				{
					return ERR_COLLIDE ;
				}      
				else if(! Get_primeframe() )
				{
					goto loop1 ;
				}
			}
			if( 4 == j )
			{
				return ERR_COLLIDE ;
			}
		}
		else                          
			goto loop1 ;
	}
	else              
	{	
		while( ! Get_ack() )
		{
			if( ( ! Get_busy() ) && ( Get_time_busy() >= THREE_HUNDRED_MS ) )
			{
				Set_here( id, 0 ) ;    
				return ERR_NO_FIND ;
			}
			vTaskDelay( 50/portTICK_RATE_MS ) ;
		} 	
		return 0 ;
	}		
}


u8	Putout_A1( void )
{
	u8 sendflag, j ;
	
	FillA1() ;    
	send_param.pbuf = (u8*)&Buf_A1_OUT_Prime ;
	send_param.szbuf = 7 ;
loop1:
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	Wait_busfree( THREE_HUNDRED_MS ) ;
loop2:
	xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
							 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
	xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	
	if( sendflag )    
	{
		if( ERR_TP == sendflag )  //被屏幕点击所产生的帧抢占
			goto loop1 ;
		if( (sendflag < 3) || (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag - 1)) )
		{
			vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别
			goto loop2 ;
		}
			 
		if( 0x5A == sendingbuf[0] )  
		{
			for( j = 0; j < 4; j++ )
			{
				Set_time_collide() ;
				Set_primeframe() ;
				while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
				
				if( Get_primeframe() >= 2 )     
				{
					return ERR_COLLIDE ;
				}      
				else if(! Get_primeframe() )
				{
					goto loop1 ;
				}
			}
			if( 4 == j )
			{
				return ERR_COLLIDE ;
			}
		}
		else                          
			goto loop1 ;
	}
	else             
	{	
		while( ! Get_ack() )
		{
			if( ( ! Get_busy() ) && ( Get_time_busy() >= THREE_HUNDRED_MS ) ) 
				goto loop1 ; 	
			vTaskDelay( 50/portTICK_RATE_MS ) ;
		}  
		return 0 ;
	}	
}


u8  Fetch_Speset( u8 id )
{
	u8 i, j, k, sendflag ;
	SPECIAL_SETTING temp ;
	
	FillA1Sg( id ) ;
	send_param.pbuf = (u8*)&Buf_A1_OUT_Prime ;
	send_param.szbuf = 7 ;
	for( i = 0 ; i < 2 ; i ++ )   
	{
	loop1 :
		if( Get_time_e8() >= FOUR_MIN )
			return ERR_E8 ;
		k = 2 ;
		Wait_busfree( THREE_HUNDRED_MS ) ;
	
	loop2 :
		xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
								 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
		xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
		if( Get_time_e8() >= FOUR_MIN )
			return ERR_E8 ;
			
		if( sendflag )    
		{
			if( ERR_TP == sendflag )  //被屏幕点击所产生的帧抢占
				goto loop1 ;
			if( (sendflag < 3) ||  (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag )) )  
			{
				k-- ;
				if( k )
				{
					vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别
					goto loop2 ;
				}
				else     
				{  
					Descend_existlevel( id ) ; 
					if( !Get_existlevel(id ) )
					{
						Set_here( id, 0 ) ;
						return ERR_NO_FIND ;      
					}	
					else
					{
						return ERR_FETCH_SPESET ;			
					}
				}
			}
		 		
			
			if( 0x5A == sendingbuf[0] )  
			{
				for( j = 0; j < 4; j++ )
				{
					Set_time_collide() ;
					Set_primeframe() ;
					while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
					
					if( Get_primeframe() >= 2 )     
					{
						return ERR_COLLIDE ;
					}      
					else if(! Get_primeframe() )
					{
						goto loop1 ;
					}
				}
				if( 4 == j )
				{
					return ERR_COLLIDE ;
				}
			}
			else                          
				goto loop1 ;
		}
	}
	return 0 ;        	
}


u8  Fetch_Params( u8 id )
{
	u8 j, k, sendflag ;

	FillA2( id ) ;
	send_param.pbuf = (u8*)&Buf_A2_OUT ;
	send_param.szbuf = 3 ;
loop1 :
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	Wait_busfree( THREE_HUNDRED_MS ) ;
loop2:
	xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
							 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
	xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	
	if( sendflag )   
	{
				if( 0xFF == sendflag )  //被屏幕点击所产生的帧抢占
			goto loop1 ;
		if( (sendflag < 3) || (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag )) )  
		{
			k-- ;
			if( k )
			{
				vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别
				goto loop2 ;
			}
			else     
			{  
				Descend_existlevel( id ) ; 
				if( !Get_existlevel(id ) )
				{
					Set_here( id, 0 ) ;
					return ERR_NO_FIND ;      
				}
				else
				{
					return ERR_THIS_ROUND ;     
				}
			}
		}
		
			if( 0x5A == sendingbuf[0] )  
			{
				for( j = 0; j < 4; j++ )
				{
					Set_time_collide() ;
					Set_primeframe() ;
					while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
					
					if( Get_primeframe() >= 2 )     
					{
						return ERR_COLLIDE ;
					}      
					else if(! Get_primeframe() )
					{
						goto loop1 ;
					}
				}
				if( 4 == j )
				{
					return ERR_COLLIDE ;
				}
			}
			else                          
				goto loop1 ;
	}
	return 0 ;  
}

u8  Putout_A3( u8 id )
{
	u8 k, sendflag, j ;
	
  Uart_putstr( "************" ) ;
	Uart_putstr( "************" ) ;
	Uart_putstr( "************" ) ;
	FillA3( id ) ;
	send_param.pbuf = (u8*)&Buf_A3_OUT ;
	send_param.szbuf = 3 ;
loop1 :
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	k = 2 ;
	Wait_busfree( THREE_HUNDRED_MS ) ;	
loop2:
	//sendflag = Send( (u8*)&Buf_A3_OUT, 3 ) ;
	xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
							 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
	xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	
	if( sendflag )   
	{
		if( 0xFF == sendflag )
			goto loop1 ;
		if( (sendflag < 3) || (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag )) )  
		{
			k-- ;
			if( k )
			{
				vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别				
				goto loop2 ;
			}
			else     
			{  
				Descend_existlevel( id ) ; 
				if( !Get_existlevel(id ) )
				{
					Set_here( id, 0 ) ;
					return ERR_NO_FIND ;      
				}
				else
				{
					return ERR_THIS_ROUND ;   
				}
			}
		}
		
		if( 0x5A == sendingbuf[0] )  
		{
			for( j = 0; j < 4; j++ )
			{
				Set_time_collide() ;
				Set_primeframe() ;
				while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
				
				if( Get_primeframe() >= 2 )     
				{
					return ERR_COLLIDE ;
				}      
				else if(! Get_primeframe() )
				{
					goto loop1 ;
				}
			}
			if( 4 == j )
			{
				return ERR_COLLIDE ;
			}
		}
		else                          
			goto loop1 ;
	}
	return 0 ;	
}


u8  Putout_A2( u8 id )
{
	u8 k, sendflag, j ;
	Uart_putstr( "************" ) ;
	Uart_putstr( "************" ) ;
	Uart_putstr( "************" ) ;
	FillA2( id ) ;
	send_param.pbuf = (u8*)&Buf_A2_OUT ;
	send_param.szbuf = 3 ;
loop1 :
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	k = 2 ;
	Wait_busfree( THREE_HUNDRED_MS ) ;
	
loop2:
	//sendflag = Send( (u8*)&Buf_A3_OUT, 3 ) ;
	xTaskCreate( vTaskSendPrimeFrame, (signed char *)"vTaskSendPrimeFrame", STACK_PRIMEFRAME_SEND,
							 (void*)&send_param, PRIOR_PRIMEFRAME_SEND, &th_Primeframe_Send)  ;
	xQueueReceive( xqh_One, &sendflag , portMAX_DELAY ) ;
	if( Get_time_e8() >= FOUR_MIN )
		return ERR_E8 ;
	
	if( sendflag )   
	{
		if( 0xFF == sendflag )  //被屏幕点击所产生的帧抢占
			goto loop1 ;
		if( (sendflag < 3) || (sendingbuf[sendflag - 1] != CheckingSum( (u8*)sendingbuf, sendflag )) )  
		{
			k-- ;
			if( k )
			{
				vTaskDelay( 20/portTICK_RATE_MS ) ;    //避免两帧连到一起，导致无法识别		
				goto loop2 ;
			}
			else     
			{  
				Descend_existlevel( id ) ; 
				if( !Get_existlevel(id ) )
				{
					Set_here( id, 0 ) ;
					return ERR_NO_FIND ;      
				}
				else
				{
					return ERR_THIS_ROUND ;   				
				}
			}
		}
		
		if( 0x5A == sendingbuf[0] )  
		{
			for( j = 0; j < 4; j++ )
			{
				Set_time_collide() ;
				Set_primeframe() ;
				while( (Get_time_collide() < ONE_MIN)  && ( Get_primeframe() < 2 ) ) ; 
				
				if( Get_primeframe() >= 2 )     
				{
					return ERR_COLLIDE ;
				}      
				else if(! Get_primeframe() )
				{
					goto loop1 ;
				}
			}
			if( 4 == j )
			{
				return ERR_COLLIDE ;
			}
		}
		else                          
			goto loop1 ;
	}
	return 0 ;		
}


/***
************************************************
函数名：vTaskPrime
参数  ：
返回值：
描述  ：主逻辑任务
************************************************
***/
void vTaskPrime( void* pvParam ) 
{
	u8 retval, i, flag_findnewmac = 0 ;
	
	
	
	//初始化
	RCC_Configuration() ;
	TX_GPIO_Configuration() ; 
	RX_EXTI_Configuration() ;
	INPUT_EXTI_Configuration() ;
	Uart_Configuration();	
	
	Init_Data() ;
	Uart_putstr( "vTaskPrim start" ) ;
	
	xTaskCreate( vTaskCntTime, ( signed char * )"vTaskCntTime", STACK_CNTTIME, ( void * )NULL, PRIOR_CNTTIME, NULL ) ;
	xTaskCreate( vTaskFillFrame, ( signed char * )"vTaskFillFrame", STACK_FILL_TPFRAME, ( void * )NULL, PRIOR_FILL_TPFRAME, th_Fill_Tpframe ) ;
	xTaskCreate( vTaskSendTpFrame, ( signed char * )"vTaskSendTpFrame", STACK_TPFRAME_SEND, ( void * )NULL, PRIOR_TPFRAME_SEND, th_Tpframe_Send ) ;
	//查询0#
	Set_time_e8() ;
	retval = Find_Zero() ;

	
	//查询1~15号机器是否存在
	for( i = 1; i < 16; i++ )
	{
		retval = Find_NewMac( i ) ;
	}
	//xTaskCreate( vTaskTest, ( signed char * )"vTaskTest", STACK_CNTTIME, ( void * )NULL, PRIOR_TEST, NULL ) ;
	
	//正常轮询
	while( 1 )
	{
		for( i = 0; i < 16; i++ )
		{
			retval = Putout_A1() ;
			
			if( Get_here( i ) )
			{
				if( Get_a1twice(i) || Get_a2param(i) )
				{
					if( Get_a1twice(i) )
					{
						retval = Fetch_Speset(i) ;
					}
					
					if( Get_a2param(i) )
					{
						retval = Fetch_Params(i) ;
					}	
				}
				else
				{
					retval = Putout_A3( i ) ;
					
					retval = Putout_A2( i ) ;
				}
			}
			else if( flag_findnewmac%2 )
			{
				retval = Find_NewMac(i) ;
			}
		}
		if( 0xFF == flag_findnewmac )
			flag_findnewmac = 0 ;
		else
			flag_findnewmac++ ;
	}
}