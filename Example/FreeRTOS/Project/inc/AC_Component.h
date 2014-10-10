/***
************************************************
文件名：AC_Component.h
描述  ：交流模块组件函数的声明
版本  ：version1.0		2014.9.5
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
/*常量申明*/
//1,Client ID   CLT_ID_
#define	  CLT_ID_PRIMECTL				0x5A      //主线控器的客户吗
#define 	CLT_ID_SUBCTL   			0x55		  //从线控器的客户码
#define 	CLT_ID_BECTLED 				0xA5			//室内机的客户码

//2,解析报文
//报文A1回信    analysis  ALS_
#define  	ALS_SET_TEMPERATURE  		0xf0        //btA的7~4位
#define 	ALS_RUN_MODE            0x0E        //btA的3~1位
#define  	ALS_WIND_UPDOWN       	0x01        //btA的0位
#define  	ALS_ENV_TEMPERATURE  		0xFC        //btB的7~2位
#define  	ALS_WIND_SPEED          0xE0        //btC的7~5位
#define  	ALS_WIND_LEFTRIGHT      0x10        //btC的4位
#define  	ALS_ALLOW_RUN_MODE   		0x30        //btD的5~4位

//A1个报回信中取特殊设定
#define   FETCH_EDING     	0x01   //取额定 btB
#define   FETCH_AAS         0x08  //取 是否允许修改地址 btD
#define   FETCH_JYDS        0x03  //取 静压档数 btE

//A1个报（特殊设定模块）
#define  	MAC_ID		0xF0            //特征字7~4，机号  

//报文A3回信
#define  	BUG_INDOOR           0x7f          //btA的6~0位

//时间设定  以100ms为单位(这些很长的时间，在TaskCntTime任务中100ms更新一次 ）
#define  THREE_HUNDRED_MS 		0x0003
#define  TWO_HUNDRED_MS    		0x0002
#define  ONE_MIN             	0x0258
#define  FOUR_MIN           	0x0960
#define  FIVE_MIN            	0x0BB8


//ERROR
#define  ERR_E8          0xFF       //4分钟主线控没有收到室内机的回应，报E8故障
#define  ERR_COLLIDE     0XFE       //多主同控

#define  ERR_NO_FIND      0xEF       //查询某机号，该机器不存在
#define  ERR_THIS_ROUND   0xEE       //本次通信时，没有联系上室内机（这并不代表室内机不存在了 ，而只是将其存在度降一）
#define  ERR_FETCH_SPESET 0xED       //本次获取特殊设定失败

#define	 ERR_TP						0xDF        //正在发送主逻辑帧的任务被TP帧任务抢占

//sendrecv
#define  LEN_FRAMEA1			 0x07
#define  LEN_FRAMEA2       0x03
#define  LEN_FRAMEA3	     0x03
#define  LEN_FRAMEA4	     0x04
#define  LEN_FRAMEA1_SG    0x07

//任务优先级
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
/*结构申明*/
typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 btA;
	u8 btB;
	u8 btC;
	u8 btD ;              //四字节数据
	u8 checksum ;         //校验和
}MSG_A1_OUT;                    //从线控器到室内机的报文A1 

typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 btA;
	u8 btB;
	u8 btC;
	u8 btD ;              
	u8 btE ;              //五字节数据
	u8 checksum ;         //校验和
}MSG_A1_IN;                    //从室内机到线控器的报文A1 

typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 checksum ;         //校验和
}MSG_A2_OUT;                    //从线控器到室内机的报文A2 

typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 btA;
	u8 btB;
	u8 btC;
	u8 btD ;              
	u8 btE ;              
	u8 btF ;				//六字节数据
	u8 checksum ;         //校验和
}MSG_A2_IN;                    //从室内机到线控器的报文A2 

typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 checksum ;         //校验和
}MSG_A3_OUT;                    //从线控器到室内机的报文A3 

typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 btA;			      //1字节数据
	u8 checksum ;         //校验和
}MSG_A3_IN;               //从室内机到线控器的报文A3 

typedef struct
{
	u8 client_ID;         //客户码
	u8 feature_ID;         //特征字
	u8 btA;			      //1字节数据
	u8 checksum ;         //校验和
}MSG_A4_OUT;               //从线控器到室内机的报文A4  

typedef struct 
{
	u8 allorsg ;        //1 同报  0 个报
	u8 id ;
//	u8 chxaddressflag ;    //更改地址 A4报文,扩展用，在A3，A2个报查询时，查询该值，若为1，则说明要更改地址
	u8 temperature ;
	u8 runmode ;
	u8 windupdown ;
	u8 envtemperature ;
	u8 windspeed ;
	u8 windleftright ;
	u8 onoff ;          //开关机该填到哪个位置
}TP_DATA;

typedef struct 
{
	u8 eding ;   //btB.D0 
	u8 adress_allow_set ; //btD.D3 
	u8 jingyadangshu ;     //btE.D1D0
}SPECIAL_SETTING;   //特殊设定

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
EXTERN __IO u16 time_e8 ;			//计时长达4min
EXTERN __IO u16 time_collide ;		//碰撞计时1min
EXTERN __IO u16 time_busy;            //配合is_busfree使用可以测量总线空闲时间


EXTERN  __IO u8 clientID ;               //客户码
EXTERN  __IO u8  busy ;         //总线忙标志 （ 0 空闲  1 忙 ）
EXTERN  __IO u16 here ;        //16个主机，存在时相应为置1(假定0#号机总是存在的)
EXTERN  __IO u16 a1twice ;  		//该位为1，则需要A1 个报两次对应的室内机，取得其特殊设定 初始化时0xFFFF 
EXTERN  __IO u16 a2param ;  		//该位为1，则需要A2 个报一次取得对应室内机的参数 初始化时0xFFFF 
EXTERN  __IO u8 ack ;           //当一帧发出时，设置为0，当收到该帧回复并校验正确后设置为1
EXTERN  __IO u8 existlevel[16] ;          //存在度，当室内机i#与线控器正常通信时， exist_level[i] = 2 ;
EXTERN  __IO u8 primeframe ;      //冲突发生后，检测到主线控数据的次数


//TX 
EXTERN __IO MSG_A1_OUT Buf_A1_OUT ;           //由主线控器到室内机的A1同报报文的缓存
EXTERN __IO MSG_A2_OUT Buf_A2_OUT ;           //由主线控器到室内机的A2报文的缓存
EXTERN __IO MSG_A3_OUT Buf_A3_OUT ;           //由主线控器到室内机的A3报文的缓存
EXTERN __IO MSG_A4_OUT Buf_A4_OUT ;           //由主线控器到室内机的A3报文的缓存
EXTERN __IO MSG_A1_OUT Buf_A1_OUT_Prime ; 
EXTERN __IO u8 sendingbuf[10] ;         //发送帧时，存放每次回收的位
//EXTERN __IO u8 flag_A3A2 ;                 //标志正在发送A3A2个报
EXTERN __IO u8 ret_send ;                  //在vTaskSendPrimeFrame任务中接收send函数的返回值
EXTERN __IO SEND_PARAM send_param ;                  
EXTERN __IO u8 flag_aa ;								//有屏幕点击事件需要发送

//RX
EXTERN __IO MSG_A1_IN Buf_A1_IN;               //由室内机发来的A1同报回信的报文缓存
EXTERN __IO MSG_A2_IN Buf_A2_IN;               //由室内机发来的A2个报回信的报文缓存
EXTERN __IO MSG_A3_IN Buf_A3_IN;               //由室内机发来的A3个报回信的报文缓存
EXTERN __IO u8 recvedbuf[10];


//control 
EXTERN __IO REGIST Registing[16] ;
EXTERN __IO u8 exti9flag ;			//9号中断发生时，改变该标志，在外面查询该标志是否变化，即能知道标志是否发生


EXTERN __IO xQueueHandle xqh_One ;   		//发送主逻辑帧任务发送之后将发送邮件给主逻辑任务
EXTERN __IO xQueueHandle xqh_Tpdata ;   //等待马总的数据到来，马总通过该邮箱将数据传来
EXTERN __IO xQueueHandle xqh_Tpframe ;  //帧填充完毕，就向Tp帧发送任务发出一个邮箱消息
EXTERN __IO xQueueHandle xqh_Two ; 			//TP帧的发送任务等待该邮箱中有邮件时，就说明可以开始发送数据了

EXTERN __IO xTaskHandle th_PrimeLoc ;   			 //主逻辑任务的句柄
EXTERN __IO xTaskHandle th_Primeframe_Send ;   //发送主逻辑帧的任务的句柄
EXTERN __IO xTaskHandle th_Fill_Tpframe ;      //填帧任务
EXTERN __IO xTaskHandle th_Tpframe_Send ;      //屏幕帧发送任务的句柄

EXTERN __IO xSemaphoreHandle xsh_flag_aa ; //发送主逻辑帧的任务和发送Tp帧的任务可能同时访问修改该值

EXTERN __IO u8 flag_sub ;      //从逻辑标志，初始化时，将该值初始化位1，检测到是从逻辑时，就置为0 
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/*函数申明*/
                             
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
