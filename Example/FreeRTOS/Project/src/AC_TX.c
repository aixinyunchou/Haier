/***
************************************************
�ļ�����AC_TX.c
����  ������ģ����֡�����뷢������
�汾  ��version1.0		2014.9.5
************************************************
***/
#include "AC_TX.h"
#include "FreeRTOS.h"
#include "stdlib.h"
#include "time.h"
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
��������Send
����  ��
����ֵ��
����  �� �������ݣ����ݴ�������֡����TX�����õ�ƽ
************************************************
***/
static u8 Send( u8 *pbuf, u8 szbuf )
{
	u8 errsynchro, errbit ;
	u8 i, j ;
		
	Set_ack( 0 ) ;
	Set_busy( 1 ) ;     	
	EXTI->RTSR &= ~EXTI_Line9 ;
	EXTI->FTSR |= EXTI_Line9 ;	

	i = 0 ;
	errsynchro = 1 ;              
	errbit = 1 ;  		      			
	memset(  (void*)sendingbuf,	'A', 10 ) ;	
	memset(  (void*)recvedbuf,	'A', 10 ) ;
	
	EXTI->IMR &= ~EXTI_Line8 ;
	EXTI->IMR |= EXTI_Line9 ;

	while( Get_flag_aa() && errsynchro && ( (errbit && (i < szbuf))  || ((!errbit) && (i < 10)) ) )
	{
		if( errbit )
		{
			//��������춸������ڣ����Խ��з���
			exti9flag = 1 ;
			while( exti9flag ) {}
			GPIOB->BSRRL = GPIO_Pin_7 ;
			vTaskDelay( 7/portTICK_RATE_MS ) ;
			GPIOB->BSRRH = GPIO_Pin_7 ;
			
			vTaskDelay( 7/portTICK_RATE_MS ) ;      //�ӳ�7ms���Ա�֤���ڴ����������					
			if( !(GPIOB->IDR & GPIO_Pin_8) )    
					errsynchro = 0 ;
		}
		else
		{
			exti9flag = 1 ;
			while( exti9flag ) {}
			vTaskDelay( 14/portTICK_RATE_MS ) ;
			if( !(GPIOB->IDR & GPIO_Pin_8) ) 	
				errsynchro = 0 ;
		}

			
		for( j = 0; Get_flag_aa() && errsynchro && (j < 8); j++ )
		{
			if( errbit )
			{
				if( pbuf[i] & ( 0x01 << j ) )      
				{
					exti9flag = 1 ;
					while( exti9flag ) {}
					GPIOB->BSRRL = GPIO_Pin_7 ;
					vTaskDelay( 7/portTICK_RATE_MS ) ;
					GPIOB->BSRRH = GPIO_Pin_7 ;
					Uart_putchar( 'A' ) ;
					
					vTaskDelay( 7/portTICK_RATE_MS ) ;  
					if( GPIOB->IDR & GPIO_Pin_8 )
					{
						sendingbuf[i] |= 0x01 << j ;
						Uart_putchar( 'a' ) ;
					}
					else
					{
						errbit = 0 ;
						sendingbuf[i] &= ~(0x01 << j) ;
						Uart_putchar( 'b' ) ;
					}
				}
				else 	
				{					
					exti9flag = 1 ;
					while( exti9flag ) {}
					Uart_putchar( 'B' ) ;								
					vTaskDelay( 14/portTICK_RATE_MS ) ;
					if( GPIOB->IDR & GPIO_Pin_8 )
					{
						errbit = 0 ;
						sendingbuf[i] |= 0x01 << j ;
						Uart_putchar( 'a' ) ;
					}
					else
					{
						sendingbuf[i] &= ~(0x01 << j) ;
						Uart_putchar( 'b' ) ;
					}
				}
			}
			else          
			{
				exti9flag = 1 ;
				while( exti9flag ) {}
				vTaskDelay( 14/portTICK_RATE_MS ) ;
				if( GPIOB->IDR & GPIO_Pin_8 )
					sendingbuf[i] |= 0x01 << j ;
				else
					sendingbuf[i] &= ~(0x01 << j) ;
			}
			Uart_putchar( ' ' ) ;
			if( 7 == j )
				i++ ;
		}
		Uart_putchar( '\n' ) ;   
	}

	EXTI->IMR &= ~EXTI_Line9 ;
	EXTI->IMR |= EXTI_Line8 ;
	
	Set_busy( 0 ) ;
	Set_time_busy() ;
	

	if( !Get_flag_aa() )
	{
		xSemaphoreTake( xsh_flag_aa, portMAX_DELAY );
		Set_flag_aa( 1 ) ;
		xSemaphoreGive( xsh_flag_aa );
		xQueueSend( xqh_Two, (void*)NULL, 0 ) ;
		return ERR_TP ;
	}
	else if( errbit )
		return 0 ;
	else 
		return i ;
}

/***
************************************************
��������vTaskFillFrame
����  ��
����ֵ��
����  ����д�����Ļ����������֡
************************************************
***/
void vTaskFillFrame( void* pvParam )
{
	TP_DATA buf ;
	static MSG_A1_OUT temp ;
	//static u8 A1[7] = { 0X5A, 0X08, 0XFF, 0XFF, 0XFF, 0XFF, 0XA1 } ; 
	Uart_putstr( "vTaskFillFrame start " ) ;
	while(1)
	{
		
		xQueueReceive( xqh_Tpdata, &buf, portMAX_DELAY ) ;  
		//*
		Buf_A1_OUT.client_ID = Get_clientID() ;
		//��Ļ�������������д
		Buf_A1_OUT.feature_ID = buf.allorsg << 3 ;
		Buf_A1_OUT.feature_ID |= buf.id << 4  ;
		Buf_A1_OUT.btA |= buf.temperature << 4 ;        //A1ͬ��btA�ĸ���λΪ�¶�
		Buf_A1_OUT.btA |= buf.runmode << 1 ;						//A1ͬ��btA��D3~D1Ϊģʽ
		Buf_A1_OUT.btA |= buf.windupdown ;	            //A1ͬ��btA�����λΪ���°׷�
		Buf_A1_OUT.btB |= buf.envtemperature << 2 ;	    //A1ͬ�� btB ��7~2λΪ�����¶�
		Buf_A1_OUT.btC |= buf.windspeed << 4 ;
		Buf_A1_OUT.btC |= buf.windleftright << 4 ;			
		
		//�����趨��д
		Buf_A1_OUT.btB |= Registing[buf.id].speset.eding ;
		Buf_A1_OUT.btD |= Registing[buf.id].speset.adress_allow_set << 3 ;
		Buf_A1_OUT.btD |= Registing[buf.id].speset.jingyadangshu << 1 ;
		Buf_A1_OUT.checksum = CheckingSum( (u8*)&Buf_A1_OUT, 7 ) ;
		//*/
		while( uxQueueMessagesWaiting(xqh_Tpframe) )    
			xQueueReceive( xqh_Tpframe, &temp, portMAX_DELAY ) ;   //����ԭ���ʼ�
		memcpy( (void*)((u8*)&temp), (void*)((u8*)&Buf_A1_OUT), 7 ) ;  //���ݿ����ݣ������ڷ������ݹ����У�buf_a1_out����д
		xQueueSend( xqh_Tpframe, (void*)&temp, 0 ) ;
	}
}


/***
************************************************
��������vTaskSendTpFrame
����  ��
����ֵ��
����  �����͵����Ļ����������֡
************************************************
***/
void vTaskSendTpFrame( void* pvParam )
{
	static MSG_A1_OUT temp ;
	u8 ret ;
	u8 tu8 ;
	static u8 t = ERR_TP ;
	//static u8 A1[7] = { 0X5A, 0X08, 0XFF, 0XFF, 0XFF, 0XFF, 0XA1 } ; 
	Uart_putstr( "vTaskSendTpFrame start " ) ;
	while(1)
	{
		xQueueReceive( xqh_Tpframe, &temp, portMAX_DELAY ) ; 
		Uart_putstr( "vTaskSendTpFrame run " ) ;
		
		if( Get_flag_sub() )   //��������߼�
		{
			xSemaphoreTake( xsh_flag_aa, portMAX_DELAY );
			if( Get_flag_aa() )
				Set_flag_aa( 0 ) ;
			xSemaphoreGive( xsh_flag_aa );
			xQueueReceive( xqh_Two, &tu8, portMAX_DELAY ) ;
		} 
		Uart_putstr( "vTaskSendTpFrame run2 " ) ;
		Wait_busfree( THREE_HUNDRED_MS ) ;
	loop2:
		ret = Send( (u8*)&temp, 7 ) ;
		Uart_putstr( "vTaskSendTpFrame run3 " ) ;
		

		if( ret && !uxQueueMessagesWaiting( xqh_Tpframe ) )  //����ʧ�� && ������û�����ݣ����ط�һ��֮ǰ�����ݣ�����ֱ�ӷ����µ�����
		{
			vTaskDelay( 20/portTICK_RATE_MS ) ;    //������֡����һ�𣬵����޷�ʶ��
			goto loop2 ;
		}
		else
			xQueueSend( xqh_One, &t, 0 ) ;   //�����߼�������Ϣ����֪A3orA2��������ʧ��
	}
}


/***
************************************************
��������vTaskSendPrimeFrame
����  ��
����ֵ��
����  ���������߼�֡
************************************************
***/
void vTaskSendPrimeFrame( void* pvParam ) 
{
	Uart_putstr( "vTaskSendPrimeFrame start" ) ;
	ret_send = Send( ((SEND_PARAM*)pvParam)->pbuf, ((SEND_PARAM*)pvParam)->szbuf ) ;
	if( ERR_TP != ret_send )
		xQueueSend( xqh_One, (void*)&ret_send, 0 ) ;
	vTaskDelete( NULL ) ;
}



void vTaskTest( void* pvParam ) 
{
	u8 t, i;
	Uart_putstr( "vTaskTest start" ) ;
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
	RNG_Cmd(ENABLE); 
	while( 1 )
	{
		xQueueSend( xqh_Tpframe, (void*)NULL, 0 ) ;
			while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);  
		t = RNG_GetRandomNumber(); 
		for( i = 0; i < t; i++ )
			vTaskDelay( 100/portTICK_RATE_MS ) ;
	}
}


void FillA1(void)
{
	Buf_A1_OUT_Prime.client_ID = Get_clientID() ;
	memcpy( (void*)((u8*)&Buf_A1_OUT_Prime+2), (void*)((u8*)&Buf_A1_OUT+2), 4 ) ;
	Buf_A1_OUT_Prime.feature_ID = 0X08 ;
	Buf_A1_OUT_Prime.btB |= Registing[0].speset.eding ;
	Buf_A1_OUT_Prime.btD |= Registing[0].speset.adress_allow_set << 3 ;
	Buf_A1_OUT_Prime.checksum = CheckingSum( (u8*)&Buf_A1_OUT_Prime, 7 ) ;
}

void FillA1Sg( u8 id ) 
{
	Buf_A1_OUT_Prime.client_ID = Get_clientID() ;
	memcpy( (void*)((u8*)&Buf_A1_OUT_Prime+2), (void*)((u8*)&Buf_A1_OUT+2), 4 ) ;
	Buf_A1_OUT_Prime.feature_ID = 0 ;
	Buf_A1_OUT_Prime.feature_ID |= id << 4 ;
	Buf_A1_OUT_Prime.btB |= Registing[id].speset.eding ;
	Buf_A1_OUT_Prime.btD |= Registing[id].speset.adress_allow_set << 3 ;
	Buf_A1_OUT_Prime.checksum = CheckingSum( (u8*)&Buf_A1_OUT_Prime, 7 ) ;	
}

void FillA2( u8 id )
{
	Buf_A2_OUT.client_ID = Get_clientID() ;
	Buf_A2_OUT.feature_ID = 0x01 ;
	Buf_A2_OUT.feature_ID |= id << 4 ;
	Buf_A2_OUT.checksum = CheckingSum( (u8*)&Buf_A2_OUT, 3 ) ;
}

void FillA3( u8 id )
{
	Buf_A3_OUT.client_ID = Get_clientID() ;
	Buf_A3_OUT.feature_ID = 0x02 ;
	Buf_A3_OUT.feature_ID |= id << 4 ;
	Buf_A3_OUT.checksum = CheckingSum( (u8*)&Buf_A3_OUT, 3 ) ;	
}

void FillA4( u8 id )
{
	
}
