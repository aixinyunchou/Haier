/***
************************************************
文件名：AC_TX.h
描述  ：交流模块填帧任务与发送任务
版本  ：version1.0		2014.9.5
************************************************
***/
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
void vTaskFillFrame( void* pvParam ) ;
void vTaskSendTpFrame( void* pvParam ) ;
void vTaskSendPrimeFrame( void* pvParam ) ;
void vTaskTest( void* pvParam ) ;
void FillA1(void) ;
void FillA1Sg( u8 id )  ;
void FillA2( u8 id ) ;
void FillA3( u8 id ) ;
void FillA4( u8 id ) ;
