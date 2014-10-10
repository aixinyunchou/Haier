#ifndef __MODECMD_H
#define __MODECMD_H

#ifndef MODECMD_GLOBAL
#define MODECMD_EXT extern
#else
#define MODECMD_EXT
#endif

#include "typedefine.h"

typedef enum{
    CID_NONE    = 0,   //无
    CID_WC      = 1,   //线控器
    CID_RC      = 2,   //遥控器(A/B码)
    CID_CC      = 3,   //集控器
    CID_OU      = 4,   //室外机设定
    CID_RMCD    = 5,   //房卡
    CID_COACT   = 6,   //联动输入
    CID_PRCL    = 7,   //停电补偿
	CID_KEY     = 8,   //遥控接收板按键
    CID_ONOFFSW = 9,   //开关信号
	CID_PC      = 10,   //Pc
	CID_UHOME = 11
} COMMANDERID;

typedef enum{
    MD_STOP = 0,
    MD_COOL = 1,
    MD_HEAT = 2,
    MD_AUTO = 3,
} MODE;

typedef enum{
    MDCC_AUTO        = 0,
    MDCC_BLOW        = 1,
    MDCC_COOL        = 2,
    MDCC_DRY         = 3,
    MDCC_HEAT        = 4,  //普通制热
} CCMODECMD;

typedef enum{
    CMS_NONE = 0, //无集中控制器
    CMS_FIFO = 1, //后入优先
    CMS_CCTL = 2, //集中控制
    CMS_LOCK = 3  //锁定
} CMODESELCMD;

typedef enum{
    FN_AUTO = 0,
    FN_LOW  = 1,
    FN_MID  = 2,
    FN_HIGH = 3,
    FN_FIXED= 4,
    FN_UND5 = 5,
    FN_UND6 = 6,
    FN_UND7 = 7
} FANCMD;

typedef enum{
    FA_NONE = 0,  //无
    FA_AUTO = 1,  //自动换气
    FA_HEAT = 2,  //全热换气
    FA_COMM = 3   //普通换气
} FRESHAIRCMD;

typedef enum{
    PL_STOP = 0,  //停止
    PL_UP = 1,    //上升
    PL_DN = 2,    //下降
    PL_STDBY = 3  //待机
} PNLFTCMD;

typedef enum{
    TSW_ON = 0,      //定时开
    TSW_OFF = 1,     //定时关
    TSW_ONOFF = 2,   //定时开关
    TSW_RPT = 3      //定时开关循环
} TMRSWCMD;

typedef enum{
	HSW_POS1 = 0,//位置1
	HSW_POS2 = 1,//位置2
	HSW_POS3 = 2,//位置3
	HSW_POS4 = 3,//位置4
	HSW_POS5 = 4,//位置5
	HSW_POS6 = 5,//位置6
	HSW_POS7 = 6,//位置7
	HSW_AUTO = 7,//自动
	HSW_NONE = 10//
} HSWINGTYPE;

typedef enum{
	VSW_NONE    =  0,
	VSW_HLTH_UP =  1,//健康气流上吹风
	VSW_POS1    =  2,//位置1
	VSW_HLTH_DN =  3,//健康气流下吹风
	VSW_POS2    =  4,//位置2
	VSW_POS3    =  6,//位置3
	VSW_POS4    =  8,//位置4
	VSW_POS5    = 10,//位置5
	VSW_AUTO    = 12 //自动
} VSWINGTYPE;

//---------cmd---------------------
MODECMD_EXT COMMANDERID    CommanderId;
MODECMD_EXT bool           CCTurnOnCmd;        //CCTurnOnCmd、CCModeCmd配合得到开关机和模式（集中器对应）
MODECMD_EXT MODE           ModeCmd;            //ModeCmd、BlowCmd、DryCmd配合得到开关机和模式（线控器遥控器对应）
MODECMD_EXT CCMODECMD      CCModeCmd;
MODECMD_EXT CMODESELCMD    CcModeSelCmd;       //集中控制模式选择
MODECMD_EXT s16            TaiCmd;             //设定温度（用于各个控制器通讯）
MODECMD_EXT FANCMD         FanSpeedCmd;
MODECMD_EXT VSWINGTYPE     VSwingCmd;          //竖直摆风

MODECMD_EXT bool           TestCmd;
//MODECMD_EXT bool         TimeShortCmd;
MODECMD_EXT bool           FilterCmd;
MODECMD_EXT FRESHAIRCMD    FreshAirCmd;
MODECMD_EXT bool           HealthCmd;
MODECMD_EXT bool           FcDefrostCmd;       //强制除霜命令
MODECMD_EXT bool           FcDefrostSTS;       //强制除霜状态（收到强制除霜命令True后，状态在1分钟内True，向外机发送除霜命令）
MODECMD_EXT bool           FcRunCmd;
MODECMD_EXT bool           RateRunCmd;
MODECMD_EXT PNLFTCMD       PanelLiftCmd;
MODECMD_EXT TMRSWCMD       TimerSwitchCmd;
MODECMD_EXT bool           TimerSwitchEnable;
MODECMD_EXT bool           DustCatcherCmd;        //集尘
MODECMD_EXT bool           QuickCmd;           //高效
MODECMD_EXT bool           PowerSaveCmd;
MODECMD_EXT bool           SleepCmd;
MODECMD_EXT bool           QuietCmd;
MODECMD_EXT bool           DryCmd;             //除湿
MODECMD_EXT bool           BlowCmd;            //送风
MODECMD_EXT bool           HeaterCmd;
MODECMD_EXT bool           StoreConfigCmd;
MODECMD_EXT bool           LockCmd;
MODECMD_EXT bool           BrouseCmd;
MODECMD_EXT bool           BkLightCmd;
MODECMD_EXT HSWINGTYPE     HSwingCmd;          //水平摆风
MODECMD_EXT bool           BlinkCmd;
MODECMD_EXT u8 			   TimerOffTask_Enabled_old;
MODECMD_EXT u8 		       TimerOnTask_Enabled_old;
MODECMD_EXT bool           SleepCmd_old;
MODECMD_EXT u16 		   TimingBlinkTmr;

// UHOME 增加
MODECMD_EXT bool HumDetectCmd;	// 人感


MODECMD_EXT bool           AddWaterCmd;
MODECMD_EXT bool           ShowTempCmd;
MODECMD_EXT bool           NatualWindCmd;
MODECMD_EXT bool           FreqHoldCmd;
MODECMD_EXT bool           LightSenseCmd;
MODECMD_EXT bool           BodySenseCmd;
MODECMD_EXT bool           GasAlarmCmd;
MODECMD_EXT bool           OxygenCmd;
MODECMD_EXT s16            HumCmd;
MODECMD_EXT u8             SetAirTemp_Sel;  // 选择灯板显示设定温度or显示环境温度，默认[255]显示设定温度，为0时显示环温） */          
MODECMD_EXT u8             NLEDTai_Cmd;
MODECMD_EXT u8             NLDEOFF_Cmd;

MODECMD_EXT s16 Tset;        //内机实际设定温度（TaiCmd + 制冷/制热温度补偿 + 睡眠温度补偿）
MODECMD_EXT s16 Tcomp_CL;    //制冷温度补偿（自动模式、SCODE计算、电加热开关、自动风速判断都起作用）
MODECMD_EXT s16 Tcomp_HT;    //制热温度补偿（自动模式、SCODE计算、电加热开关、自动风速判断都起作用）
MODECMD_EXT s16 Tcomp_Sleep; //睡眠温度补偿
MODECMD_EXT s16 Tdif;        //只在自动模式转换判断上起作用
extern s16 FreshCool_TasR;  //    00C        12    新风机制冷目标温度TasR：[18]，单位℃                 
extern s16 FreshHeat_TasR;  //    00D        16    新风机制热目标温度TasR：[22]，单位℃                 

void StdCmdMk_Reset(void);
void StdCmdMk(void);
void SETTaiCompensation_RC(void);       //遥控器设定温度补偿
void TaiCompensationMk(void);//选择温度补偿

#define HasNewCmd(dst) HasNewCmd_##dst()                // 查询某个通讯模块，是否收到新指令
#define SetNewCmdFlag(dst) SetNewCmdFlag_##dst() // 通知某个通讯模块，其他模块改变了指令，并已被系统接收
#define GetCmd(dst,cmd) Get##cmd##_##dst()              // 查询某个通讯模块收到的指定指令值

#define IsVSwingCmdAUTO() (((PCB_WEDGE_THIN_NEW != PCBType&&VSW_HLTH_UP == VSwingCmd) || (PCB_WEDGE_THIN_NEW != PCBType&&VSW_HLTH_DN == VSwingCmd) || VSW_AUTO == VSwingCmd )? TRUE : FALSE) //71自动摆风
#define IsHSwingCmdAUTO() ((HSW_AUTO==HSwingCmd)? TRUE:FALSE)

#endif