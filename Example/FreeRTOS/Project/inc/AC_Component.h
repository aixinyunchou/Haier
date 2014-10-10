/***
************************************************
�ļ�����AC_Component.h
����  ������ģ���������������
�汾  ��version1.0		2014.9.5
************************************************
***/

#ifndef    _AC_COMPONENT_H
#define    _AC_COMPONENT_H
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
///////////////////////////////////////////////////////////////////////
#ifdef	CPP_GLOBLE
#define	EXTERN 
#else
#define	EXTERN	extern
#endif

//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/*��������*/
//1,Client ID   CLT_ID_
#define	  CLT_ID_PRIMECTL				0x5A      //���߿����Ŀͻ���
#define 	CLT_ID_SUBCTL   			0x55		  //���߿����Ŀͻ���
#define 	CLT_ID_BECTLED 				0xA5			//���ڻ��Ŀͻ���

//2,��������
//����A1����    analysis  ALS_
#define  	ALS_SET_TEMPERATURE  		0xf0        //btA��7~4λ
#define 	ALS_RUN_MODE            0x0E        //btA��3~1λ
#define  	ALS_WIND_UPDOWN       	0x01        //btA��0λ
#define  	ALS_ENV_TEMPERATURE  		0xFC        //btB��7~2λ
#define  	ALS_WIND_SPEED          0xE0        //btC��7~5λ
#define  	ALS_WIND_LEFTRIGHT      0x10        //btC��4λ
#define  	ALS_ALLOW_RUN_MODE   		0x30        //btD��5~4λ

//A1����������ȡ�����趨
#define   FETCH_EDING     	0x01   //ȡ� btB
#define   FETCH_AAS         0x08  //ȡ �Ƿ������޸ĵ�ַ btD
#define   FETCH_JYDS        0x03  //ȡ ��ѹ���� btE

//A1�����������趨ģ�飩
#define  	MAC_ID		0xF0            //������7~4������  

//����A3����
#define  	BUG_INDOOR           0x7f          //btA��6~0λ

//ʱ���趨  ��100msΪ��λ(��Щ�ܳ���ʱ�䣬��TaskCntTime������100ms����һ�� ��
#define  THREE_HUNDRED_MS 		0x0003
#define  TWO_HUNDRED_MS    		0x0002
#define  ONE_MIN             	0x0258
#define  FOUR_MIN           	0x0960
#define  FIVE_MIN            	0x0BB8


//ERROR
#define  ERR_E8          0xFF       //4�������߿�û���յ����ڻ��Ļ�Ӧ����E8����
#define  ERR_COLLIDE     0XFE       //����ͬ��

#define  ERR_NO_FIND      0xEF       //��ѯĳ���ţ��û���������
#define  ERR_THIS_ROUND   0xEE       //����ͨ��ʱ��û����ϵ�����ڻ����Ⲣ���������ڻ��������� ����ֻ�ǽ�����ڶȽ�һ��
#define  ERR_FETCH_SPESET 0xED       //���λ�ȡ�����趨ʧ��

#define	 ERR_TP						0xDF        //���ڷ������߼�֡������TP֡������ռ

//sendrecv
#define  LEN_FRAMEA1			 0x07
#define  LEN_FRAMEA2       0x03
#define  LEN_FRAMEA3	     0x03
#define  LEN_FRAMEA4	     0x04
#define  LEN_FRAMEA1_SG    0x07

//�������ȼ�
#define  PRIOR_CNTTIME                     20
#define  PRIOR_PRIME_LOC                   5
#define  PRIOR_PRIMEFRAME_SEND             14
#define  PRIOR_TPFRAME_SEND                11
#define  PRIOR_RECV			                   17
#define  PRIOR_FILL_TPFRAME                8       
#define  PRIOR_TEST												 18

#define  STACK_CNTTIME                    200
#define  STACK_PRIME_LOC                200
#define  STACK_PRIMEFRAME_SEND       200
#define  STACK_TPFRAME_SEND            200
#define  STACK_RECV			   200
#define  STACK_FILL_TPFRAME           200
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/*�ṹ����*/
typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 btA;
	u8 btB;
	u8 btC;
	u8 btD ;              //���ֽ�����
	u8 checksum ;         //У���
}MSG_A1_OUT;                    //���߿��������ڻ��ı���A1 

typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 btA;
	u8 btB;
	u8 btC;
	u8 btD ;              
	u8 btE ;              //���ֽ�����
	u8 checksum ;         //У���
}MSG_A1_IN;                    //�����ڻ����߿����ı���A1 

typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 checksum ;         //У���
}MSG_A2_OUT;                    //���߿��������ڻ��ı���A2 

typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 btA;
	u8 btB;
	u8 btC;
	u8 btD ;              
	u8 btE ;              
	u8 btF ;				//���ֽ�����
	u8 checksum ;         //У���
}MSG_A2_IN;                    //�����ڻ����߿����ı���A2 

typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 checksum ;         //У���
}MSG_A3_OUT;                    //���߿��������ڻ��ı���A3 

typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 btA;			      //1�ֽ�����
	u8 checksum ;         //У���
}MSG_A3_IN;               //�����ڻ����߿����ı���A3 

typedef struct
{
	u8 client_ID;         //�ͻ���
	u8 feature_ID;         //������
	u8 btA;			      //1�ֽ�����
	u8 checksum ;         //У���
}MSG_A4_OUT;               //���߿��������ڻ��ı���A4  

typedef struct 
{
	u8 allorsg ;        //1 ͬ��  0 ����
	u8 id ;
//	u8 chxaddressflag ;    //���ĵ�ַ A4����,��չ�ã���A3��A2������ѯʱ����ѯ��ֵ����Ϊ1����˵��Ҫ���ĵ�ַ
	u8 temperature ;
	u8 runmode ;
	u8 windupdown ;
	u8 envtemperature ;
	u8 windspeed ;
	u8 windleftright ;
	u8 onoff ;          //���ػ�����ĸ�λ��
}TP_DATA;

typedef struct 
{
	u8 eding ;   //btB.D0 
	u8 adress_allow_set ; //btD.D3 
	u8 jingyadangshu ;     //btE.D1D0
}SPECIAL_SETTING;   //�����趨

typedef struct 
{
	u8 elementA ;
	u8 elementB ;
	u8 elementC ;
	u8 elementD ;
	u8 elementE ;
	u8 elementF ;
}MAC_PARAM;

typedef struct 
{
	SPECIAL_SETTING speset ;
	MAC_PARAM machineparam ;
}REGIST;

typedef struct
{
	u8* pbuf ;
	u8  szbuf ;
}SEND_PARAM ;


///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////	
EXTERN __IO u16 time_e8 ;			//��ʱ����4min
EXTERN __IO u16 time_collide ;		//��ײ��ʱ1min
EXTERN __IO u16 time_busy;            //���is_busfreeʹ�ÿ��Բ������߿���ʱ��


EXTERN  __IO u8 clientID ;               //�ͻ���
EXTERN  __IO u8  busy ;         //����æ��־ �� 0 ����  1 æ ��
EXTERN  __IO u16 here ;        //16������������ʱ��ӦΪ��1(�ٶ�0#�Ż����Ǵ��ڵ�)
EXTERN  __IO u16 a1twice ;  		//��λΪ1������ҪA1 �������ζ�Ӧ�����ڻ���ȡ���������趨 ��ʼ��ʱ0xFFFF 
EXTERN  __IO u16 a2param ;  		//��λΪ1������ҪA2 ����һ��ȡ�ö�Ӧ���ڻ��Ĳ��� ��ʼ��ʱ0xFFFF 
EXTERN  __IO u8 ack ;           //��һ֡����ʱ������Ϊ0�����յ���֡�ظ���У����ȷ������Ϊ1
EXTERN  __IO u8 existlevel[16] ;          //���ڶȣ������ڻ�i#���߿�������ͨ��ʱ�� exist_level[i] = 2 ;
EXTERN  __IO u8 primeframe ;      //��ͻ�����󣬼�⵽���߿����ݵĴ���


//TX 
EXTERN __IO MSG_A1_OUT Buf_A1_OUT ;           //�����߿��������ڻ���A1ͬ�����ĵĻ���
EXTERN __IO MSG_A2_OUT Buf_A2_OUT ;           //�����߿��������ڻ���A2���ĵĻ���
EXTERN __IO MSG_A3_OUT Buf_A3_OUT ;           //�����߿��������ڻ���A3���ĵĻ���
EXTERN __IO MSG_A4_OUT Buf_A4_OUT ;           //�����߿��������ڻ���A3���ĵĻ���
EXTERN __IO MSG_A1_OUT Buf_A1_OUT_Prime ; 
EXTERN __IO u8 sendingbuf[10] ;         //����֡ʱ�����ÿ�λ��յ�λ
//EXTERN __IO u8 flag_A3A2 ;                 //��־���ڷ���A3A2����
EXTERN __IO u8 ret_send ;                  //��vTaskSendPrimeFrame�����н���send�����ķ���ֵ
EXTERN __IO SEND_PARAM send_param ;                  
EXTERN __IO u8 flag_aa ;								//����Ļ����¼���Ҫ����

//RX
EXTERN __IO MSG_A1_IN Buf_A1_IN;               //�����ڻ�������A1ͬ�����ŵı��Ļ���
EXTERN __IO MSG_A2_IN Buf_A2_IN;               //�����ڻ�������A2�������ŵı��Ļ���
EXTERN __IO MSG_A3_IN Buf_A3_IN;               //�����ڻ�������A3�������ŵı��Ļ���
EXTERN __IO u8 recvedbuf[10];


//control 
EXTERN __IO REGIST Registing[16] ;
EXTERN __IO u8 exti9flag ;			//9���жϷ���ʱ���ı�ñ�־���������ѯ�ñ�־�Ƿ�仯������֪����־�Ƿ���


EXTERN __IO xQueueHandle xqh_One ;   		//�������߼�֡������֮�󽫷����ʼ������߼�����
EXTERN __IO xQueueHandle xqh_Tpdata ;   //�ȴ����ܵ����ݵ���������ͨ�������佫���ݴ���
EXTERN __IO xQueueHandle xqh_Tpframe ;  //֡�����ϣ�����Tp֡�������񷢳�һ��������Ϣ
EXTERN __IO xQueueHandle xqh_Two ; 			//TP֡�ķ�������ȴ������������ʼ�ʱ����˵�����Կ�ʼ����������

EXTERN __IO xTaskHandle th_PrimeLoc ;   			 //���߼�����ľ��
EXTERN __IO xTaskHandle th_Primeframe_Send ;   //�������߼�֡������ľ��
EXTERN __IO xTaskHandle th_Fill_Tpframe ;      //��֡����
EXTERN __IO xTaskHandle th_Tpframe_Send ;      //��Ļ֡��������ľ��

EXTERN __IO xSemaphoreHandle xsh_flag_aa ; //�������߼�֡������ͷ���Tp֡���������ͬʱ�����޸ĸ�ֵ

EXTERN __IO u8 flag_sub ;      //���߼���־����ʼ��ʱ������ֵ��ʼ��λ1����⵽�Ǵ��߼�ʱ������Ϊ0 
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/*��������*/
                             
u16  Get_time_e8( void ) ;
void Set_time_e8( void ) ;
u16  Get_time_collide( void ) ;
void Set_time_collide( void ) ;
u16  Get_time_busy( void ) ;
void Set_time_busy( void ) ;
void vTaskCntTime( void* pvParam ) ;


u8   Get_clientID( void ) ;
void Set_clientID( u8 id ) ;
u8   Get_busy( void ) ;
void Set_busy( u8 temp ) ;
u16  Ret_here( void ) ;
u8   Get_here( u8 machine_id ) ;
void Set_here( u8 machine_id, u8 temp ) ;
u8   Get_a1twice( u8 machine_id ) ;
void Set_a1twice( u8 machine_id, u8 flag );
u8   Get_a2param( u8 machine_id ) ;
void Set_a2param( u8 machine_id, u8 flag ) ;
u8   Get_ack( void ) ;
void Set_ack( u8 ) ;
u8	 Get_existlevel( u8 machine_id ) ;
void Set_existlevel( u8 machine_id ) ; 
void Descend_existlevel( u8 machine_id ) ;
u8   Get_primeframe( void ) ;
void Set_primeframe( void ) ; 
void Ascend_primeframe( void ) ;
u8   Get_flag_A3A2(void ) ;
void Set_flag_A3A2( u8 temp ) ;
u8   Get_flag_aa(void ) ;
void Set_flag_aa( u8 temp ) ;
u8	 Get_flag_sub( void ) ;
void Set_flag_sub( u8 temp ) ;

void RCC_Configuration( void ) ;
void TX_GPIO_Configuration( void ) ; 
void RX_EXTI_Configuration( void ) ;
void INPUT_EXTI_Configuration( void ) ;
void Uart_Configuration( void );
void Uart_sendingbuf( void );
void Uart_putchar( u8 ch ) ;
void Uart_putstr( char* pstr ) ;
u8   CheckingSum( u8 temp[], u8 n ) ;
void Wait_busfree( u16 time ) ;


void Init_Data( void ) ;
void E8guzhang( void ) ;
void tongxunguzhang( void ) ;
void vTaskPrime( void* pvParam ) ;
///////////////////////////////////////////////////////////////////////

#endif
