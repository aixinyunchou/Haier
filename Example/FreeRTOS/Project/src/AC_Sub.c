/***
************************************************
文件名：AC_Sub.c
描述  ：主逻辑
版本  ：version1.0		2014.10.3
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


/***
************************************************
函数名：vTaskSub
参数  ：
返回值：
描述  ：从逻辑任务
************************************************
***/
void vTaskSub( void* pvParam ) 
{
	//初始化
	RCC_Configuration() ;
	TX_GPIO_Configuration() ; 
	RX_EXTI_Configuration() ;
	INPUT_EXTI_Configuration() ;
	Uart_Configuration();	
	
	Init_Data() ;
	Set_flag_sub( 0 ) ;  //设置从逻辑开启
	Uart_putstr( "vTaskSub start" ) ;
	
	xTaskCreate( vTaskCntTime, ( signed char * )"vTaskCntTime", STACK_CNTTIME, ( void * )NULL, PRIOR_CNTTIME, NULL ) ;
	xTaskCreate( vTaskFillFrame, ( signed char * )"vTaskFillFrame", STACK_FILL_TPFRAME, ( void * )NULL, PRIOR_FILL_TPFRAME, th_Fill_Tpframe ) ;
	xTaskCreate( vTaskSendTpFrame, ( signed char * )"vTaskSendTpFrame", STACK_TPFRAME_SEND, ( void * )NULL, PRIOR_TPFRAME_SEND, th_Tpframe_Send ) ;
	
	Set_time_e8() ;
	while(1)
	{
		if( Get_time_e8() >= FOUR_MIN )
		{
			//return ERR_E8 ;
		}
		vTaskDelay( 5000/portTICK_RATE_MS ) ;
	}
}