#ifndef __DC_PROTOCOL_H
#define __DC_PROTOCOL_H

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
//#include "semphr.h"
#include "DC_Config.h"
#include "DC_Timer.h"
#include "stm32f4xx.h"

typedef unsigned char BYTE;

/*   线控器状态，待修改 */
typedef enum 
{
	IDLE_STATUS      = 0x00,       /*	空闲/等待发送状态			*/
	SENDING_STATUS   = 0x01,			 /* 开始发送							*/ 
	WAIT_TO_RECEIVE  = 0x02,		   /*	等待接收状态					*/
	BEGIN_TO_RECEIVE = 0x03,		   /*	开始接收/识别引导状态	*/
	RECEIVE_ERROR    = 0x04,		   /*	接收字节出错					*/
	RECEIVE_COMPLETED= 0x05,		   /*	接收字节完成					*/
	CHECK_ERROR      = 0x06        /*	校验出错							*/
}CONTROL_STATUS;

/*    掉电记忆存取结构体    */
typedef struct
{
	BYTE OOFlag;     //开关机标志
	BYTE Mode;   	 //模式
	BYTE Temperature;//温度
	BYTE WindSpeed;  //风速
	BYTE WDFlag;     //摆风
	BYTE HeatFlag;   //加热标志
	BYTE HealthFlag; //健康标志
	BYTE FreshAir;   //新风状态
} MemoryStatus;

/*   发送一帧的字节长度    */
#define LENGTH_OF_SEND_FRAME 6
/*   接收一帧的字节长度    */
#define LENGTH_OF_RECEIVE_FRAME 5

/*	2s  = 10000 * 200us	*/
#define RESEND_TIME 	10000
/*	4min = 1200000 * 200us 	*/
#define SHUTDOWN_TIME  1200000

/*  待修改  掉电记忆的地址定义   */
#define MEMORY_BASE_ADDR   ((uint32_t) 0x20000000)
#define MEMORY_ADDR (MEMORY_BASE_ADDR + 0x00000000)

/*   掉电记忆的存储数据结构   */
#define MEMORY_DATA ((MemoryStatus *)MEMORY_ADDR)

/*    
        数据格式：LSB 
	通信原则：
          引导码 : 24ms 低 
	       0   : 12ms 低
	       1   :  4ms 低
	     间隔 :  4ms 高
*/

#define GUIDANCE_MS 24
#define NUM0_MS 12
#define NUM1_MS 4
#define INTERNAL_MS 4

/*   bit defines    */
#define POS_D0  ((BYTE)0x01)
#define POS_D1  ((BYTE)0x02)
#define POS_D2  ((BYTE)0x04)
#define POS_D3  ((BYTE)0x08)
#define POS_D4  ((BYTE)0x10)
#define POS_D5  ((BYTE)0x20)
#define POS_D6  ((BYTE)0x40)
#define POS_D7  ((BYTE)0x80)

#define POS_D02 ((BYTE)0x07)
#define POS_D03 ((BYTE)0x0F)
#define POS_D04 ((BYTE)0x1F)
#define POS_D05 ((BYTE)0x3F)
#define POS_D06 ((BYTE)0x7F)
#define POS_D25 ((BYTE)0x3C)
#define POS_D45 ((BYTE)0x30)
#define POS_D67 ((BYTE)0xC0)

/*	模式定义	*/
#define AutoMode ((BYTE)0x00)
#define CoolMode ((BYTE)0x01)
#define WetMode  ((BYTE)0x02)
#define AirMode  ((BYTE)0x03)
#define HeatMode ((BYTE)0x04)
#define ComfMode ((BYTE)0x05)

#define DC_DelayMS(x)  vTaskDelay((x)/portTICK_RATE_MS);
/*	内机状态	*/
typedef struct 
{
	__IO BYTE IndoorTemperature;  //室内温度
	
	__IO BYTE IsHeating;          //制热标志
	__IO BYTE InletGridFlag;      //进风栅开闭
	__IO BYTE MachineType;        //机型
	__IO BYTE ErrorInfo;          //故障信息
	
	__IO BYTE OnOffFlag;          //开关机标志
	__IO BYTE WindSwingFlag;      //风门摆动
	__IO BYTE WaterFlag;          //水箱水满标志
	__IO BYTE HeaterFlag;         //电热标志
	__IO BYTE Mode;
	
	__IO BYTE WindSpeed;
	__IO BYTE SetTemperature;    //设定温度
	__IO BYTE RemoteControl;     //远程控制
	__IO BYTE RemoteLock;        //远程锁定
} MachineStatus;

void DC_Config(void);				/*	直流协议初始化	*/

/***************************************************
 * 参数：isOn --- 是否关机，0：关机   1：开机
 *      mode --- 模式，     1：制冷   2：除湿   4：制热
 *        temperature --- 设定温度，0 --- 16  ...... 14----30
 *	       windSpeed --- 风速，0：自动    1：低风    2：中风    3：高风 
 *       PAN --- 是否摆风    0：停止摆风    1： 开始摆风
 ***************************************************/
void DC_Control(u8 isOn, u8 mode ,u8 temperature, u8 windSpeed,u8 PAN); 
#endif
