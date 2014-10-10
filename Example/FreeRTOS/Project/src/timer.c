#define TIMER_GLOBAL
/** @file timer.c
  * @author  刘清明
  * @version V1.0.0
  * @date    2009年10月
  * @brief   定时器实现文件.
  ******************************************************************************
  * @copy
  *
  *     定时器模块占用一个硬件定时器资源，采用定时中断，中断周期250微秒。
  *     在此模块中实际提供了三类定时服务：
  *     - 通用定时：针对一般性任务，对定时京都不是很高的应用。提供以下特性：
  *       - 定时精度：25ms,100ms,1sec,5sec,10sec,1min,5min和10min.
  *       - 计数模式：递减计数，递加计数。
  *       - 缩时功能：可以选择支持缩时，或者不支持缩时，缩时比例可以调节。
  *       - 暂停功能：可以在任何时候暂停计时，或重新计时。
  *       - 设定和查询计时值：对于递减计时，可以设定计时长度，递加计时的计时长度都是0xffff。
  *     - 快速定时：针对定时精度要求高，反应迅速的定时需求，精度均为250us，递减计数，不可缩时，不能暂停
  *     - 任务定时：达到设定时间后，自动执行某个动作。
  *     .
  *
  * <h2><center>&copy; COPYRIGHT 2009 青岛海尔空调电子有限公司</center></h2>
  */
#include "timer.h"
#include "wifi.h"
#ifdef DOXYGEN
#define _static
#define _extern
#else
#define _static static
#define _extern extern
#endif
 

//! 主循环25ms定时标志
TimerFlag MainTimerFlag = {0, 0};
//! 系统是否缩时的标志
u8 TimeShortFlag = 0;
//! 系统接收的缩时指令
u8 TimeShortCmd = 0;

 
//! 通用定时器专用数据结构
struct
{
    u16 Counter;         //!< 计数变量
    u8 State:2;          //!< 定时状态，TimerState类型，不直接采用TimerState类型原因是为了节省RAM空间
    u8 Type:4;           //!< 定时精度，TimerType类型，不直接采用TimerType类型原因是为了节省RAM空间
    u8 Direction:1;      //!< 递加还是递减，TimerDirection类型，不直接采用TimerDirection类型原因是为了节省RAM空间
    u8 ShortEnable:1;    //!< 缩时标志，TimerShort类型，不直接采用TimerShort类型原因是为了节省RAM空间
} TimerBuffer[TIMER_COUNT] = {
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
    {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};

//! 正常时基变量的模值
//! 定制通用定时器类型：数组单元的数值 * 25ms = 定时单位
const u16 TimerPattern[TIMER_TYPE_COUNT+1] = {1,4,40,200,400,2400,12000,24000,0};
//! 基准定时专用结构：一般结构和缩时结构
struct
{
    u16 Counter;  //!< 时基的计数值
    u16 Value;    //!< 时基的增量值
} TimerBase[TIMER_TYPE_COUNT] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},       //!<正常时基变量
  TimerBase_Short[TIMER_TYPE_COUNT] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //!<缩时时基变量


//! 定义此宏，支持定时任务，取消此宏，将不支持
#define TIMERTASK_ENABLED
#ifdef TIMERTASK_ENABLED
//! 定时任务用数据结构和变量
struct
{
    u16 Counter_25ms:12; //!< 任务的计时器
    u16 Enabled:1;       //!< 是否有效
    u16 Running:1;       //!< 是否在运行中标识
    u16 ShortEnable:1;   //!< 是否可以缩时
    u16 Res:1;           //!< 保留空位
    u16 Param;           //!< 执行任务时传给任务的参数
    u8 (*Handler)(u8,u16);//!< 任务函数指针
} TimerTasks[TIMERTASK_COUNT]={
    {0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
#endif
/*****************************************************************************************
 * 函数：Tenms_Timer_Config（）
 * 描述：定时器初始化函数，三个参数主要设置进入中断的时长。具体参考mcu手册，
				 目前只是随意设定的时间，不是10ms
 * 输入：无
 * 输出：无
 * 返回: 无
*****************************************************************************************/
void Tenms_Timer_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 199;			   
	TIM_TimeBaseStructure.TIM_Prescaler = 83;    	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM7, ENABLE);
}
/***************************************************************************************
 * 函数：Tenms_Timer_NVIC_Config（）
 * 描述：中断相关设置
 * 输入：无
 * 输出：无
 * 返回: 无
****************************************************************************************/
void Tenms_Timer_NVIC_Config(void) 
{
	NVIC_InitTypeDef NVIC_InitStruct; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 

	NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn; 
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStruct); 
}

void Tenms_Timer_Init(void)
{
	Tenms_Timer_Config();
	Tenms_Timer_NVIC_Config();
}

/****************************************************************************
 * 函数：TIM7_IRQHandler（）
 * 描述：中断处理函数
 * 输入：无
 * 输出：无
 * 返回: 无
****************************************************************************/
void TIM7_IRQHandler(void)
{
	static u8 tmr25ms = 0,tmr32ms = 0, tmr2ms = 0, tmr1ms = 0;
	if(TIM_GetFlagStatus(TIM7, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_FLAG_Update);
    
    //! @sp4 1. 维护主循环25ms定时标志，以及2ms定时标志, 参见@ref MainTimerFlag
    if(0 != tmr1ms) tmr1ms--;
    else
    {
        if(0 != tmr2ms ) tmr2ms = 0; else {MainTimerFlag.MS2 = 255; tmr2ms  =  2 - 1;}    
        if(0 != tmr25ms) tmr25ms-- ; else {MainTimerFlag.MS25 =  1; tmr25ms = 25 - 1;}
        if(0 != tmr32ms) tmr32ms-- ; else {MainTimerFlag.MS32 =  1; tmr32ms = 32 - 1;}    
        MainTimerFlag.MS1 = 0x1F; tmr1ms = 1 * 5 - 1;
    }
		if(0 != FastTimerBuffer[0]) FastTimerBuffer[0]--;
    if(0 != FastTimerBuffer[1]) FastTimerBuffer[1]--;
    if(0 != FastTimerBuffer[2]) FastTimerBuffer[2]--;
    if(0 != FastTimerBuffer[3]) FastTimerBuffer[3]--;
    if(0 != FastTimerBuffer[4]) FastTimerBuffer[4]--;
    if(0 != FastTimerBuffer[5]) FastTimerBuffer[5]--;
    if(0 != FastTimerBuffer[6]) FastTimerBuffer[6]--;
    if(0 != FastTimerBuffer[7]) FastTimerBuffer[7]--;
    if(0 != FastTimerBuffer[8]) FastTimerBuffer[8]--;
	}
}



//! @brief  定时器维护,25ms执行一次.
//! @param  无
//! @retval 无
void updateTimer(void)
{
    //! <h3>实现过程：</h3>
    u16 idx;
    u8 temp;
    static struct
    {
        u8 Delay:7;
        u8 Flag:1;
    } TimeShort_Struct={0,0};
    static u8 tmr1sec;
    
    Tick_1sec_Flg = 0;
    
    if(0 != tmr1sec) tmr1sec--;                //1s时基定时器管理
    else
    {
        if(ThermoOnOffTimeSec < (U32_MAX - 60))
        {
            if(0 == TimeShortFlag) ThermoOnOffTimeSec++; else ThermoOnOffTimeSec += 60;
        }
        if(INVOnOffTimeSec < (U32_MAX - 60))
        {
            if(0 == TimeShortFlag) INVOnOffTimeSec++; else INVOnOffTimeSec += 60;
        }
        Tick_1sec_Flg = 1; tmr1sec = 40 * 1 - 1;
    }

    //! 3. 维护时基参数
    //! <br/>@sp4各个时基的计数值counter递加：对于正常时基, counter加1，对于缩时时基, counter加缩时比例；
    //! <br/>@sp4计数值counter达到或超过模值 @ref TimerPattern 的时基，将其计数值counter与模值 @ref TimerPattern 的商赋给时基的增量value，余数留给计数值counter；
    for(idx = 0;idx < TIMER_TYPE_COUNT;++idx)
    if(TimerPattern[idx] != 0)
    {
        //正常时基
        TimerBase[idx].Counter++;
        if(TimerBase[idx].Counter >= TimerPattern[idx])
        {
            TimerBase[idx].Counter = 0;
            TimerBase[idx].Value = 1;
        }
        else TimerBase[idx].Value = 0;
        
        //缩时时基
        if(TimeShortFlag != 0)
        {
            TimerBase_Short[idx].Counter += TIMER_SHORT_RATE;
            if(TimerBase_Short[idx].Counter >= TimerPattern[idx])
            {
                TimerBase_Short[idx].Value = TimerBase_Short[idx].Counter / TimerPattern[idx];
                TimerBase_Short[idx].Counter -= TimerBase_Short[idx].Value * TimerPattern[idx];
            }
            else TimerBase_Short[idx].Value = 0;
        }
        else 
        {
            TimerBase_Short[idx].Counter = 0;
            TimerBase_Short[idx].Value = 0;
        }
    }
    else break;
    
    //! 4. 通用定时器缓冲区维护
    //! <br/>@sp4逐个维护已经指派，并且没有暂停的定时器：
    //! <br/>@sp4@sp4①不需要缩时的定时器
    //! <br/>@sp4@sp4@sp4递减定时器计数值每次减去相应正常时基的增量值，直到零为止；
    //! <br/>@sp4@sp4@sp4递减定时器计数值每次加上相应正常时基的增量值，直到0xffff为止；
    //! <br/>@sp4@sp4②需要缩时的定时器
    //! <br/>@sp4@sp4@sp4递减定时器计数值每次减去相应缩时时基的增量值，直到零为止；
    //! <br/>@sp4@sp4@sp4递减定时器计数值每次加上相应缩时时基的增量值，直到0xffff为止；
    for(idx = 0;idx < TIMER_COUNT;++idx)
    if(TimerBuffer[idx].State == TIMER_RUNNING &&       //定时器在运转中
       TimerPattern[TimerBuffer[idx].Type] != 0)        //定时器类型已经定义
    {
        
        if((TimeShortFlag == 0 || TimerBuffer[idx].ShortEnable == TIMER_SHORT_DISABLE) &&
        TimerBase[TimerBuffer[idx].Type].Value != 0)
        {
            //不缩时定时器的维护
            if(TimerBuffer[idx].Direction == TIMER_DEC)
            {
                //递减计时：到0停止
                if(TimerBuffer[idx].Counter > TimerBase[TimerBuffer[idx].Type].Value)
                    TimerBuffer[idx].Counter -= TimerBase[TimerBuffer[idx].Type].Value;
                else TimerBuffer[idx].Counter=0;
            }
            else
            {
                //递加计时：到65535停止
                if(TimerBuffer[idx].Counter < ~(TimerBase[TimerBuffer[idx].Type].Value))
                    TimerBuffer[idx].Counter += TimerBase[TimerBuffer[idx].Type].Value;
                else TimerBuffer[idx].Counter = 0xffff;
            }
        }
        else if(TimeShortFlag != 0 && TimerBuffer[idx].ShortEnable  == TIMER_SHORT_ENABLE &&
            TimerBase_Short[TimerBuffer[idx].Type].Value != 0)
        {
            //缩时定时器的维护
            if(TimerBuffer[idx].Direction == TIMER_DEC)
            {
               //递减计时：到0停止
                if(TimerBuffer[idx].Counter > TimerBase_Short[TimerBuffer[idx].Type].Value)
                    TimerBuffer[idx].Counter -= TimerBase_Short[TimerBuffer[idx].Type].Value;
                else TimerBuffer[idx].Counter = 0;
            }
            else
            {
                //递加计时：到65535停止
                if(TimerBuffer[idx].Counter < ~(TimerBase_Short[TimerBuffer[idx].Type].Value))
                    TimerBuffer[idx].Counter += TimerBase_Short[TimerBuffer[idx].Type].Value;
                else TimerBuffer[idx].Counter = 0xffff;
            }
        }
    }

    //! 5. 维护定时任务，此功能可用TIMERTASK_ENABLED宏屏蔽
    //! <br/>@sp4维护正在运行中的，有效的定时任务
    //! <br/>@sp4@sp4更新任务的定时器，一旦达到指定时间，调用相关任务函数
    //! <br/>@sp4@sp4任务函数返回零值：清除该任务，以后不再执行，释放资源
    //! <br/>@sp4@sp4任务函数返回非零值：保持该任务，但暂停计时，要想继续执行，必须重新启动
#ifdef TIMERTASK_ENABLED
    for(idx = 0;idx < TIMERTASK_COUNT;++idx)
    if(TimerTasks[idx].Enabled!=0 && TimerTasks[idx].Running!=0)
    {
        //更新定时器
        if(TimeShortFlag == 0 || TimerTasks[idx].ShortEnable == 0)
        {
            if(TimerTasks[idx].Counter_25ms!=0)TimerTasks[idx].Counter_25ms--;
        }
        else if(TimerTasks[idx].Counter_25ms >= TIMER_SHORT_RATE)TimerTasks[idx].Counter_25ms-=TIMER_SHORT_RATE;
        else TimerTasks[idx].Counter_25ms = 0;
        
        //执行定时任务
        if(TimerTasks[idx].Counter_25ms==0)
        {
            temp=0;
            if(TimerTasks[idx].Handler!=0)
                temp=TimerTasks[idx].Handler(idx,TimerTasks[idx].Param);
                
            if(temp==0)
            {
                TimerTasks[idx].Enabled=0;
                TimerTasks[idx].Running=0;
                TimerTasks[idx].ShortEnable=0;
                TimerTasks[idx].Res=0;
                TimerTasks[idx].Param=0;
                TimerTasks[idx].Handler=0;
            }
            else TimerTasks[idx].Running=0;
        }
    }
#endif
}

/**
  * @brief 申请通用定时器.
  * @param type : 定时器的定时精度类型。可取一下数值：
  *   @arg@ref TIMER_25MS
  *   @arg@ref TIMER_100MS 
  *   @arg@ref TIMER_SEC 
  *   @arg@ref TIMER_5SEC  
  *   @arg@ref TIMER_10SEC 
  *   @arg@ref TIMER_MIN
  *   @arg@ref TIMER_5MIN
  *   @arg@ref TIMER_10MIN
  * @param  direction : 指定定时器是递加计时还是递减计时。 可取一下数值：
  *     @arg@ref TIMER_DEC : 表示递加计时t
  *     @arg@ref TIMER_INC : 表示递减计时
  * @param  shortEnable : 指定定时器是否可以缩时。
  *  <br>@sp4可以取@ref TIMER_SHORT_ENABLE 表示可以缩时，或者取@ref TIMER_SHORT_DISABLE 表示不能缩时
  * @param  state ：指定定时器的状态，可以取以下值：
  *     @arg@ref TIMER_IDLE：表示没有被指派，一般情况下不用此值，一旦使用表示将释放该定时器。
  *     @arg@ref TIMER_STOP：表示暂停
  *     @arg@ref TIMER_RUNNING：表示正在运行
  * @param  value : 指定定时器计数的初始值，表示多少个计时精度间隔，定时器溢出。
  * @retval 分配到的定时器序号
  */
GeneralTimer getGTimer(TimerType type,TimerDirection direction,
    TimerShort shortEnable,TimerState state,u16 value)
{
    /** <h3>实现过程：</h3> */
    u16 idx;
    
    /** @arg 1.若 state = TIMER_IDLE，返回无效的索引号 */
    if(state == TIMER_IDLE)return INVALIDTIMER;
    
    /** @arg 2.搜索整个定时器缓冲区，找到一个TIMER_IDLE状态的定时器，就返回其索引号。
      * - 要调用 @ref configGTimer 函数
      */
    for(idx = 0;idx < TIMER_COUNT; ++idx)
    {
        if(TimerBuffer[idx].State == TIMER_IDLE)
        {
            configGTimer(idx,type,direction,shortEnable,state,value); 
            return (GeneralTimer)idx;
        }
    }
    
    /** @arg 3.找不到TIMER_IDLE状态的定时器，就返回无效的索引号 */
    return INVALIDTIMER;
}

/**
  * @brief 释放通用定时器
  * @param tmr : 要释放的定时器序号
  * @retval 无
  */
void freeGTimer(GeneralTimer tmr)
{
    if(tmr < TIMER_COUNT)
    {
        TimerBuffer[tmr].State = TIMER_IDLE;
        TimerBuffer[tmr].Direction = TIMER_DEC;
        TimerBuffer[tmr].ShortEnable = TIMER_SHORT_DISABLE;
        TimerBuffer[tmr].Type = TIMER_TYPE0;
        TimerBuffer[tmr].Counter = 0;
    }
}

/**
  * @brief 配置通用定时器
  * @param tmr : 要配置的定时器序号
  * @param type : 定时器的定时精度类型。可取一下数值：
  *   @arg@ref TIMER_25MS
  *   @arg@ref TIMER_100MS 
  *   @arg@ref TIMER_SEC 
  *   @arg@ref TIMER_5SEC  
  *   @arg@ref TIMER_10SEC 
  *   @arg@ref TIMER_MIN
  *   @arg@ref TIMER_5MIN
  *   @arg@ref TIMER_10MIN
  * @param  direction : 指定定时器是递加计时还是递减计时。 可取一下数值：
  *     @arg@ref TIMER_DEC : 表示递加计时t
  *     @arg@ref TIMER_INC : 表示递减计时
  * @param  shortEnable : 指定定时器是否可以缩时。
  *  <br>@sp4可以取@ref TIMER_SHORT_ENABLE 表示可以缩时，或者取@ref TIMER_SHORT_DISABLE 表示不能缩时
  * @param  state ：指定定时器的状态，可以取以下值：
  *     @arg@ref TIMER_IDLE：表示没有被指派，一般情况下不用此值，一旦使用表示将释放该定时器。
  *     @arg@ref TIMER_STOP：表示暂停
  *     @arg@ref TIMER_RUNNING：表示正在运行
  * @param  value : 指定定时器计数的初始值，表示多少个计时精度间隔，定时器溢出。
  * @retval 无
  */
void configGTimer(GeneralTimer tmr,TimerType type,TimerDirection direction,
    TimerShort shortEnable,TimerState state,u16 value)
{
    if(tmr<TIMER_COUNT)
    {
        if(state == TIMER_IDLE)
        {
            freeGTimer(tmr);
            return;
        }
        else TimerBuffer[tmr].State = state;
        TimerBuffer[tmr].Direction = direction;
        TimerBuffer[tmr].Type = type;
        TimerBuffer[tmr].ShortEnable = shortEnable;
        TimerBuffer[tmr].Counter = value;
    }
}

/**
  * @brief 查询通用定时器计数值
  * @param tmr : 要查询的定时器序号
  * @retval 当前计数值
  */
u16 getGTimerValue(GeneralTimer tmr)
{
    if(tmr < TIMER_COUNT)return TimerBuffer[tmr].Counter;
    else return 0;
}
/**
  * @brief 设定通用定时器计数值
  * @param tmr : 要配置的定时器序号
  * @param time : 计数值
  * @retval 无
  */
void setGTimerValue(GeneralTimer tmr, u16 time)
{
    if(tmr < TIMER_COUNT)TimerBuffer[tmr].Counter = time;
}

/**
  * @brief 恢复定时器计数
  * @param tmr : 要操作的定时器序号
  * @retval 无
  */
void startGTimer(GeneralTimer tmr)
{
    if(tmr < TIMER_COUNT)
    if(TimerBuffer[tmr].State != TIMER_IDLE)TimerBuffer[tmr].State = TIMER_RUNNING;
}

/**
  * @brief 暂停定时器计数
  * @param tmr : 要操作的定时器序号
  * @retval 无
  */
void stopGTimer(GeneralTimer tmr)
{
    if(tmr < TIMER_COUNT)
    if(TimerBuffer[tmr].State != TIMER_IDLE)TimerBuffer[tmr].State = TIMER_STOP;    
}

#ifdef TIMERTASK_ENABLED

/**
  * @brief 设定定时任务,函数返回值等于零时，函数执行完毕后将自动清除该任务, 返回255表示设置失败
  * @param ms : 执行任务函数前的时间计数值
  * @param f :任务函数指针, 类型为u8 (*)(u8,u16)
  * @param param :任务函数需要的参数
  * @param short_enable :是否支持缩时, 传入零，表示不能所示，非零值，表示可以缩时
  * @retval 定时任务的索引值
  */
u8 setTimerTask(u16 ms,u8 (*f)(u8,u16),u16 param,u8 short_enable)
{
    u8 idx;
    
    for(idx=0; idx<TIMERTASK_COUNT; ++idx)
    {
        if(TimerTasks[idx].Enabled==0)
        {
            TimerTasks[idx].Enabled=1;
            TimerTasks[idx].Counter_25ms=(ms+12)/25;
            TimerTasks[idx].Running=1;
            TimerTasks[idx].ShortEnable=short_enable;
            TimerTasks[idx].Res=0;
            TimerTasks[idx].Param=param;
            TimerTasks[idx].Handler=f;
            return idx;
        }
    }
    return 255;
}

/**
  * @brief 取消并清除定时任务
  * @param idx : 定时任务的索引值
  * @retval 无 
  */
void clearTimerTask(u8 idx)
{
    if(idx<TIMERTASK_COUNT)
    {
        TimerTasks[idx].Enabled=0;
        TimerTasks[idx].Running=0;
        TimerTasks[idx].ShortEnable=0;
        TimerTasks[idx].Res=0;
        TimerTasks[idx].Param=0;
        TimerTasks[idx].Handler=0;
        TimerTasks[idx].Counter_25ms=0;
    }
}

/**
  * @brief 重新设定任务定时器
  * @param idx : 定时任务的索引值
  * @param ms : 执行任务函数前的时间计数值
  * @retval 无
  */
void setTimerTaskValue(u8 idx,u16 ms)
{
    if(idx<TIMERTASK_COUNT && TimerTasks[idx].Enabled!=0)
    {
        TimerTasks[idx].Counter_25ms=(ms+12)/25;
        TimerTasks[idx].Running=1;
    }
}

/**
  * @brief 设定任务参数
  * @param idx : 定时任务的索引值
  * @param param : 任务函数的参数值
  * @retval 无
  */
void setTimerTaskParam(u8 idx,u16 param)
{
    if(idx<TIMERTASK_COUNT && TimerTasks[idx].Enabled!=0)
        TimerTasks[idx].Param=param;
}

/**
  * @brief 恢复定时任务
  * @param idx : 定时任务的索引值
  * @retval 无
  */
void resumeTimerTask(u8 idx)
{
    if(idx<TIMERTASK_COUNT && TimerTasks[idx].Enabled!=0)
        TimerTasks[idx].Running=1;
}

/**
  * @brief 挂起定时任务
  * @param idx : 定时任务的索引值
  * @retval 无
  */
void suspendTimerTask(u8 idx)
{
    if(idx<TIMERTASK_COUNT && TimerTasks[idx].Enabled!=0)
        TimerTasks[idx].Running=0;
}

#endif