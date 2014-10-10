/** @file    gcom.h
  * @author  刘清明
  * @version V1.0.0
  * @date    2009年10月
  * @brief   通讯共享头文件
  ******************************************************************************
  * @copy
  *
  *     定义了各个通讯公用的类型，函数，宏定义
  *
  * <h2><center>&copy; COPYRIGHT 2009 青岛海尔空调电子有限公司</center></h2>
  */
#ifndef GCOM_INCLUDE_H
#define GCOM_INCLUDE_H
#include "typedefine.h"

/** @addtogroup 共用通讯
  * 通讯共享模块
  * @{
  */

/** @addtogroup GCom_Type
  * 通讯共享类型定义
  * @{
  */
  
/******************************************************************************
*
*             方式1 —— 通讯模块处理帧头帧尾，打包解包数据分散进行
*
*******************************************************************************/

/**
  * @Brief 通讯打包解包函数指针 
  * @param ComID ： 通讯 ID
  * @param FrameID ：帧类型 ID
  * @param DRT ：打包(@ref COM_UNPACK)还是解包(@ref COM_UNPACK)
  * @param buffer : 处理的数据缓冲区
  * @param size ：帧长度
  * @retval 无
  */
typedef void (*Com_DataModule)(u8 ComID,u8 FrameID,u8 DRT,void* buffer,u8 size);

/**
  * @brief 定义与通讯（数据）相关的模块
  */
typedef struct
{
    Com_DataModule Module; //!< 模块对应的打包解包函数指针
    u8 FrameID;            //!< 帧类型 ID
    u8 DRT;                //!< 打包(@ref COM_UNPACK)、解包(@ref COM_UNPACK)还是两者(@ref COM_BOTH)
} ComModuleItem;

/** @} */

/** @addtogroup GCom_Macro
  * 通讯共享宏定义
  * @{
  */
  
//通讯数据方向
#define COM_PACK    0    //!< 数据解包
#define COM_UNPACK  1    //!< 数据打包
#define COM_BOTH    2    //!< 数据解包和打包

/** 
  * @def DECLARE_COMMODULE 
  * 在不同模块头文件中声明存在的打包解包模块 
  * @param name : 模块名称，下列宏中的name必须相同：
  * - @ref DECLARE_COMMODULE
  * - @ref IMPLEMENT_COMMODULE
  * - @ref ADD_COMMODULE
  */
#define DECLARE_COMMODULE(name) void name##_commodule(u8 ComID,u8 FrameID,u8 DRT,void* buffer,u8 size);

/** 
  * @def IMPLEMENT_COMMODULE 
  * 在不同模块实现文件中实现的打包解包模块(开始) 
  * @param name : 模块名称，下列宏中的name必须相同：
  * - @ref DECLARE_COMMODULE
  * - @ref IMPLEMENT_COMMODULE
  * - @ref ADD_COMMODULE
  */
#define IMPLEMENT_COMMODULE(name) void name##_commodule(u8 COMID,u8 FRAMEID,u8 DRT,void* BUFFER,u8 SIZE){
    
/** 
  * @def END_COMMODULE 
  * 在不同模块实现文件中实现的打包解包模块(结束) 
  */
#define END_COMMODULE }

/** 
  * @def CREATE_COMMODULE_TABLE
  * 在通讯实现文件中建立通讯数据打包解包函数登记表,登记与通讯（数据）相关的模块
  * @param ComID ：ComID是该通讯分配的唯一标识，在整个系统中一个通讯模块只有一个标识，并且相互不能重复。
  * 它与@ref PACKDATA 和@ref UNPACKDATA 中的ComID是相同的。
  */
#define CREATE_COMMODULE_TABLE(ComID) \
static u8 Com##ComID##_idx; \
static const ComModuleItem Com##ComID##_ModuleTable[]={
        
/** 
  * @def END_COMMODULE_TABLE 
  * 完成打包解包函数登记表  
  */
#define END_COMMODULE_TABLE {0,0,0}};
    
/** 
  * @def ADD_COMMODULE 
  * 在打包解包函数表中增加项  
  * @param name : 模块名称，下列宏中的name必须相同：
  * - @ref DECLARE_COMMODULE
  * - @ref IMPLEMENT_COMMODULE
  * - @ref ADD_COMMODULE
  * @param frameID : 帧类型ID,同一个通讯中帧ID是不重复的，它对应于收发双方的一组数据交互。
  * @param drt ：标识此类型帧适用于本模块的打包(@ref COM_UNPACK)、解包(@ref COM_UNPACK)还是两者(@ref COM_BOTH)操作。
  * 帧ID对应的选定的操作，必须在@ref IMPLEMENT_COMMODULE宏声明的函数中实现。
  */
#define ADD_COMMODULE(name,frameID,drt) {name##_commodule,frameID,drt},

/**
  * @def PACKDATA 
  * 通讯端口发送方数据前。要调用的打包函数,
  * 将接收的数据打包，注意Buffer只是通讯数据缓冲区，不包括帧头和帧尾部分 
  * @param comID : ComID是该通讯分配的唯一标识，在整个系统中一个通讯模块只有一个标识，并且相互不能重复。
  * 它与@ref CREATE_COMMODULE_TABLE 和@ref UNPACKDATA 中的ComID是相同的。
  * @param frameID : 帧类型ID,同一个通讯中帧ID是不重复的，它对应于收发双方的一组数据交互。
  * @param size ：帧长度
  */
#define PACKDATA(comID,frameID,buffer,size) \
{                                                                                                  \
    Com##comID##_idx=0;                                                                            \
    while(Com##comID##_ModuleTable[Com##comID##_idx].Module!=0)                                    \
    {                                                                                              \
        if((Com##comID##_ModuleTable[Com##comID##_idx].FrameID==frameID) &&                        \
           (Com##comID##_ModuleTable[Com##comID##_idx].DRT==COM_PACK ||                            \
            Com##comID##_ModuleTable[Com##comID##_idx].DRT==COM_BOTH))                             \
            Com##comID##_ModuleTable[Com##comID##_idx].Module(comID,frameID,COM_PACK,buffer,size); \
        Com##comID##_idx++;                                                                        \
    }                                                                                              \
}
        
/**
  * @def UNPACKDATA
  * 通讯端口发送方数据前。要调用的解包函数,
  * 将接收的数据解包，注意Buffer只是通讯数据缓冲区，不包括帧头和帧尾部分
  * @param comID : ComID是该通讯分配的唯一标识，在整个系统中一个通讯模块只有一个标识，并且相互不能重复。
  * 它与@ref CREATE_COMMODULE_TABLE 和@ref PACKDATA 中的ComID是相同的。
  * @param frameID : 帧类型ID,同一个通讯中帧ID是不重复的，它对应于收发双方的一组数据交互。
  * @param size ：帧长度
  */
#define UNPACKDATA(comID,frameID,buffer,size) \
{                                                                                                    \
    Com##comID##_idx=0;                                                                              \
    while(Com##comID##_ModuleTable[Com##comID##_idx].Module!=0)                                      \
    {                                                                                                \
        if((Com##comID##_ModuleTable[Com##comID##_idx].FrameID==frameID) &&                          \
           (Com##comID##_ModuleTable[Com##comID##_idx].DRT==COM_UNPACK ||                            \
            Com##comID##_ModuleTable[Com##comID##_idx].DRT==COM_BOTH))                               \
            Com##comID##_ModuleTable[Com##comID##_idx].Module(comID,frameID,COM_UNPACK,buffer,size); \
        Com##comID##_idx++;                                                                          \
    }                                                                                                \
}

//! @}

/******************************************************************************
*
*                方式2 —— 通讯模块做全部工作
*
*******************************************************************************/

/******************************************************************************
*
*                         指令变量定义相关
* 
******************************************************************************/

/** @addtogroup GCom_Type
  * 通讯共享类型定义
  * @{
  */
 
/** 通讯端口收到的控制指令结构 */
typedef struct
{
    u8 Value:7;      //!< 当前的有效指令
    u8 Changed:1;    //!< 数据是否改变了
} COMMINST;

/** 指令更新标志 */
typedef struct
{
    u8 Myself:1; //!< 该通讯收到新指令，需要更新相应内部指令
    u8 Valid:1;  //!< 该通讯有效，可以读写相关数据
    u8 Other:7;  //!< 内部刚更新新指令，该通讯需要做相应更新
} CmdModifiedType;

typedef union
{
     //table第一项key=数据个数，value=默认值
     //最大值127
    u16 all;
    struct
    {
        u8 key;
        u8 value:7;
        u8 dir:1;
    } data;
} ParamMapTableItem;


/*********************************************************************************
*
*                          通讯打包解包相关
*
*********************************************************************************/

//数据转换函数
typedef u16 (*TransformParam)(void);     //转换函数指针类型
typedef struct
{
    u8 *buffer;
    TransformParam transform;
    //far u16 **table;
    u8 byte_idx;
    u8 bit_idx;
} FrameMacroType;

//! @}

/** @addtogroup GCom_Functions
  * 通讯共享函数
  * @{
  */
u16 AddParam(u16 var,u16 offset);        //加减,index=1
u16 SubParam(u16 var,u16 offset);        //加减,index=2
u16 MulParam(u16 var,u16 m);             //乘法,index=3
u16 DivParam(u16 var,u16 d);             //除法,index=4
u16 LShiftParam(u16 var,u8 bit_count);   //移位,index=5
u16 RShiftParam(u16 var,u8 bit_count);   //移位,index=6
u16 ToASC(u16 var,u8 most_bit8);         //整数转为ASCII，可选转换高8位还是低8位,index=7
u16 FromASC(u16 var);                    //ASCII转为整数,index=8
u16 MapParam(u8 var,u16 *table,u8 dir);  //查表,index=9


u16 frame_get_func(u8 size,FrameMacroType* frame_param);
void frame_set_func(u16 value,u8 size,FrameMacroType* frame_param);

//! @}

/** @addtogroup GCom_Macro
  * 通讯共享宏定义
  * @{
  */
#define frame_start(buffer,offset,transform,table) FrameMacroType frame_param={buffer+offset,transform,table,0,0}
#define frame_change_flag(flag) CmdModifiedType* this_frame_change_flag=&flag
#define frame_get(com_var,size) com_var=frame_get_func(size,&frame_param)
#define frame_set(com_val,size) frame_set_func(com_val,size,&frame_param)
#define frame_blank(size) \
    { \
        frame_param.bit_idx+=size;\
        if(frame_param.bit_idx>7) \
        { \
            frame_param.byte_idx+=(frame_param.bit_idx>>3);\
            frame_param.bit_idx -=((frame_param.bit_idx>>3)<<3); \
        } \
    }
#define frame_set_blank(size,val) \
    { \
        if(val==0)frame_set_func(0,size,&frame_param); \
        else frame_set_func((1<<size)-1,size,&frame_param); \
    }
#define frame_end()
#define frame_chk(old_var,new_var,flag) \
    { \
        if(old_var!=new_var) \
        { \
            flag=1; \
            if(this_frame_change_flag->Other==0)this_frame_change_flag->Myself=1; \
        } \
    }
#define frame_cmd_chk(var_cmd,var_com) \
    { \
        if(var_com.Value!=var_cmd) \
        { \
            var_com.Changed=1; \
            if(this_frame_change_flag->Other==0)this_frame_change_flag->Myself=1; \
        } \
    }
#define frame_get_cmd_chk(var_cmd,com_var,size) \
    { \
        frame_get(com_var.Value,size); \
        if(com_var.Value!=var_cmd) \
        { \
            com_var.Changed=1; \
            if(this_frame_change_flag->Other==0)this_frame_change_flag->Myself=1; \
        } \
    }
#define frame_get_cmd(com_var,size) frame_get(com_var.Value,size)

//! @}
//! @}
    
#endif