/** @file timer.h
  * @author  刘清明
  * @version V1.0.0
  * @date    2009年10月
  * @brief   本文档介绍系统两类定时器.
  ******************************************************************************
  * @copy
  *
  *     定时器模块占用一个硬件定时器资源，采用定时中断，中断周期200微秒。
  *     在此模块中实际提供了三类定时服务：
  *     - 通用定时：针对一般性任务，对定时京都不是很高的应用。提供以下特性：
  *       - 定时精度：25ms,100ms,1sec,5sec,10sec,1min,5min和10min.
  *       - 计数模式：递减计数，递加计数。
  *       - 缩时功能：可以选择支持缩时，或者不支持缩时，缩时比例可以调节。
  *       - 暂停功能：可以在任何时候暂停计时，或重新计时。
  *       - 设定和查询计时值：对于递减计时，可以设定计时长度，递加计时的计时长度都是0xffff。
  *     - 快速定时：针对定时精度要求高，反应迅速的定时需求，精度均为200us，递减计数，不可缩时，不能暂停
  *     - 任务定时：达到设定时间后，自动执行某个动作。
  *     .
  *
  * <h2><center>&copy; COPYRIGHT 2009 青岛海尔空调电子有限公司</center></h2>
  */
#ifndef TIMER_INCLUDE_H
#define TIMER_INCLUDE_H

#ifndef TIMER_GLOBAL
#define TIMER_EXT extern
#else
#define TIMER_EXT
#endif

#include "typedefine.h"
#include "gcom.h"
#include "stm32f4xx.h"
#include "includes.h"

#define sec * 1

//时间变量定义
#define TMR1SEC_250MS  ((u16)4*1-1)    //单位250ms
#define TMR2SEC_250MS  ((u16)4*2-1)    //单位250ms
#define TMR5SEC_250MS  ((u16)4*5-1)    //单位250ms
#define TMR10SEC_250MS ((u16)4*10-1)   //单位250ms
#define TMR20SEC_250MS ((u16)4*20-1)   //单位250ms
#define TMR30SEC_250MS ((u16)4*30-1)   //单位250ms
#define TMR80SEC_250MS ((u16)4*80-1)   //单位250ms
#define TMR1MIN_250MS  ((u16)1*60*4)   //单位250ms
#define TMR2MIN_250MS  ((u16)2*60*4)   //单位250ms
#define TMR3MIN_250MS  ((u16)3*60*4)   //单位250ms
#define TMR5MIN_250MS  ((u16)5*60*4)   //单位250ms
#define TMR6MIN_250MS  ((u16)6*60*4)   //单位250ms
#define TMR10MIN_250MS ((u16)10*60*4)  //单位250ms
#define TMR15MIN_250MS ((u16)15*60*4)  //单位250ms
#define TMR60MIN_250MS ((u32)60*60*4)  //单位250ms
#define TMR240MIN_250MS ((u32)240*60*4)  //单位250ms
#define TMR360MIN_250MS ((u32)360*60*4)  //单位250ms

#define TMRDOT5SEC ((u16)20*1-1)    
#define TMR1SEC    ((u16)40*1-1)    //以25ms为单位 SEC:减一的使用if(0 != Tmr) Tmr--; else...，无减一的使用if(0 != Tmr) Tmr--; if(0 == Tmr)...
#define TMR2SEC    ((u16)40*2-1)    //以25ms为单位
#define TMR3SEC    ((u16)40*3-1)
#define TMR4SEC    ((u16)40*4-1)
#define TMR5SEC    ((u16)40*5-1)             
#define TMR10SEC   ((u16)40*10-1)
#define TMR15SEC   ((u16)40*15-1)
#define TMR20SEC   ((u16)40*20-1)
#define TMR30SEC   ((u16)40*30-1)
#define TMR45SEC   ((u16)40*45-1)
#define TMR60SEC   ((u16)40*60-1)            
#define TMR105SEC  ((u16)40*105-1)         
#define TMR120SEC  ((u16)40*120-1)         
#define TMR180SEC  ((u16)40*180-1)    
#define TMR300SEC  ((u16)40*300-1)

#define TMR1MIN    ((u16)60*1)    //以秒为单位
#define TMR2MIN    ((u16)60*2)    //以秒为单位
#define TMR3MIN   ((u16)60*3)     //以秒为单位
#define TMR5MIN   ((u16)60*5)     //以秒为单位
#define TMR6MIN   ((u16)60*6)     //以秒为单位
#define TMR8MIN   ((u16)60*8)     //以秒为单位
#define TMR10MIN   ((u16)60*10)   //以秒为单位
#define TMR15MIN   ((u16)60*15)   //以秒为单位
#define TMR20MIN   ((u16)60*20)   //以秒为单位
#define TMR30MIN   ((u16)60*30)   //以秒为单位
#define TMR60MIN   ((u16)60*60)   //以秒为单位
#define TMR120MIN  ((u16)60*120)  //以秒为单位

#define TMR20SEC_2S   ((u16)10)    //以2秒为单位
#define TMR3MIN_2S    ((u16)30*3)    //以2秒为单位
#define TMR10MIN_2S   ((u16)30*10)   //以2秒为单位
  
//! 定时器缓冲区大小:字节
#define TIMER_COUNT         64
//! 可以定义的定时任务最多数量
#define TIMERTASK_COUNT      8
//! main函数无穷循环体周期:ms
#define MAINLOOPPERIOD      25
//! 无效定时器编号
#define INVALIDTIMER        255
//! 判定定时器是否有效的宏
#define IsValidTimer(t)     ((t)<TIMER_COUNT)
//! 判定定时器是否无效的宏
#define IsInvalidTimer(t)   ((t)>=TIMER_COUNT)
//! 预定义的通用定时器个数
#define TIMER_TYPE_COUNT    8  
//! 缩时比例
#define TIMER_SHORT_RATE    60  

  


//! 25毫秒定时器类型, @sa @link TimerType TIMER_TYPE0@endlink
#define TIMER_25MS  TIMER_TYPE0
//! 100毫秒定时器类型, @sa @link TimerType TIMER_TYPE1@endlink
#define TIMER_100MS TIMER_TYPE1  
//! 1秒定时器类型, @sa @link TimerType TIMER_TYPE2@endlink
#define TIMER_SEC   TIMER_TYPE2  
//! 5秒定时器类型, @sa @link TimerType TIMER_TYPE3@endlink
#define TIMER_5SEC  TIMER_TYPE3  
//! 10秒定时器类型, @sa @link TimerType TIMER_TYPE4@endlink
#define TIMER_10SEC TIMER_TYPE4  
//! 60秒定时器类型, @sa @link TimerType TIMER_TYPE5@endlink
#define TIMER_MIN   TIMER_TYPE5  
//! 5分钟定时器类型, @sa @link TimerType TIMER_TYPE6@endlink
#define TIMER_5MIN  TIMER_TYPE6  
//! 10分钟定时器类型, @sa @link TimerType TIMER_TYPE7@endlink
#define TIMER_10MIN TIMER_TYPE7 

#define LED_BEEP_2MS_MASK       (u16)0x01  //!< LED和蜂鸣器用2ms定时标志
#define NLED_2MS_MASK           (u16)0x02  //!< NLED用2ms定时标志
#define CASSETTESWING_2MS_MASK  (u16)0x04  //!< 嵌入机摆风电机驱动用2ms定时标志
#define SWG_2MS_MASK            (u16)0x08  //!< 摆风电机驱动用2ms定时标志

//! 毫秒定时器缓冲区：9个单元
TIMER_EXT u16 FastTimerBuffer[9];

#define Uhome_BusFreeTmr 0

#define SetFastTmrVal(id, val) (FastTimerBuffer[id] = val) //设定毫秒定时器计数值
#define GetFastTmrVal(id) (FastTimerBuffer[id])            //查询毫秒定时器计数值

#define Tick_1sec_Flg MainTimerFlag._1sec   //主循环用1sec定时标志，自动维护

TIMER_EXT u32 ThermoOnOffTimeSec;
TIMER_EXT u32 INVOnOffTimeSec;

//! 定时器状态的枚举值
typedef enum{
    TIMER_IDLE=0,   //!< 没有被指派
    TIMER_STOP=1,   //!< 暂停
    TIMER_RUNNING=2 //!< 正在计数
} TimerState;

//! 定时器缩时的枚举值
typedef enum{
    TIMER_SHORT_DISABLE=0, //!< 不能缩时的定时器
    TIMER_SHORT_ENABLE=1   //!< 可以缩时的定时器
} TimerShort;

//! 定时器递增还是递减计时
typedef enum{
    TIMER_DEC=0,  //!< 递增定时
    TIMER_INC=1   //!< 递减定时
} TimerDirection;

//! 定时器类型：原始类型，取0..7
typedef enum{        //自己定制：按照需要定义定时器的种类，并改成有意义的名称
    TIMER_TYPE0=0,   //!< 定时精度-0
    TIMER_TYPE1=1,   //!< 定时精度-1
    TIMER_TYPE2=2,   //!< 定时精度-2
    TIMER_TYPE3=3,   //!< 定时精度-3
    TIMER_TYPE4=4,   //!< 定时精度-4
    TIMER_TYPE5=5,   //!< 定时精度-5
    TIMER_TYPE6=6,   //!< 定时精度-6
    TIMER_TYPE7=7    //!< 定时精度-7
} TimerType;

typedef u16 GeneralTimer;    //!< 通用定时器类型
typedef u16 FastTimer;       //!< 快速定时器类型：此处是200us定时器,都不缩时，递减计数器

//! 系统使用的几个重要的定时标志
typedef struct
{
  u16 MS25:1;  //!< 主循环25ms定时标志
  u16 MS32:1;  //!< 主循环32ms定时标志
  u16 _1sec:1; //!< 1sec定时标志 主循环使用，自动维护
  u16 MS1:5;   //!< 5个1ms定时标志
  u16 MS2:8;   //!< 8个2ms定时标志
} TimerFlag;

//主循环25ms定时标志
extern TimerFlag MainTimerFlag;
//系统是否缩时的标志
extern u8 TimeShortFlag;  
//系统接收的缩时指令
extern u8 TimeShortCmd;        



//定时器维护, 按照硬件定时器的定时中断，逐个将定时器置位
void updateTimer(void); 

//申请定时器93

GeneralTimer getGTimer(TimerType Type,TimerDirection Direction,
    TimerShort ShortEnable,TimerState State,u16 value);
//释放定时器
void freeGTimer(GeneralTimer);
//设置定时器
void configGTimer(GeneralTimer tmr,TimerType Type,TimerDirection Direction,
    TimerShort ShortEnable,TimerState State,u16 value);
//查询计数值
u16 getGTimerValue(GeneralTimer);
//设置计数值
void setGTimerValue(GeneralTimer,u16);
//暂停计数
void stopGTimer(GeneralTimer);
//恢复计数
void startGTimer(GeneralTimer);


//设定定时任务,返回255表示设置失败，函数返回值等于零时，函数执行完毕后将自动清除该任务
//ms: 定时时间，毫秒，最小分辨率为25；f：任务函数；param：传递给任务函数的参数；short_enable：是否允许缩时
//u8 setTimerTask(u16 ms,u8 (*f)(u8 idx,u16 param),u16 param=0,u8 short_enable=0);
u8 setTimerTask(u16 ms,u8 (*f)(u8,u16),u16 param,u8 short_enable);
//取消并清除定时任务
void clearTimerTask(u8 idx);
//重新设定任务定时器
void setTimerTaskValue(u8 idx,u16 ms);
//设定任务参数
void setTimerTaskParam(u8 idx,u16 param);
//恢复定时任务
void resumeTimerTask(u8 idx);
//挂起定时任务
void suspendTimerTask(u8 idx);

void Tenms_Timer_Init(void);
#endif 
