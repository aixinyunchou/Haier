/***
************************************************
�ļ�����AC_TX.h
����  ������ģ����֡�����뷢������
�汾  ��version1.0		2014.9.5
************************************************
***/
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
void vTaskFillFrame( void* pvParam ) ;
void vTaskSendTpFrame( void* pvParam ) ;
void vTaskSendPrimeFrame( void* pvParam ) ;
void vTaskTest( void* pvParam ) ;
void FillA1(void) ;
void FillA1Sg( u8 id )  ;
void FillA2( u8 id ) ;
void FillA3( u8 id ) ;
void FillA4( u8 id ) ;
