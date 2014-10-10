#ifndef __WIFI_H_
#define __WIFI_H_
#include "includes.h"

#include "typedefine.h"
#include "modecmd.h"
#include "Gcom.h"
#include "NetEnDeCode.h"
#ifndef UHOMECOM_GLOBAL
#define UHOMECOM_EXT extern
#else
#define UHOMECOM_EXT
#endif

#define RECV_DATA_NOT_OPERATING 0
#define RECV_DATA_OPERATING 1

#define UART_OPERATE_OK       1
#define UART_OPERATE_ERROR    0

#define WIFI_RECV_NOT_EXCEED 0
#define WIFI_RECV_EXCEED 1

#define UHomeCom_BUSFREETMR (50*5) 


#define WIFI_UART             			 USART1
#define WIFI_UART_RCCCLK      			 RCC_APB2Periph_USART1
#define WIFI_UART_GPIO_AF    				 GPIO_AF_USART1
#define WIFI_UART_IRQ         			 USART1_IRQn
//#define WIFI_UART_IRQHandler				 USART1_IRQHandler

#define WIFI_UART_TX_GPIO      			 GPIOA
#define WIFI_UART_TX_PINS 			 		 GPIO_PinSource9
#define WIFI_UART_TX_PIN       			 GPIO_Pin_9
#define WIFI_UART_TX_RCCCLK    			 RCC_AHB1Periph_GPIOA
	
#define WIFI_UART_RX_GPIO       		 GPIOA
#define WIFI_UART_RX_PINS 			 		 GPIO_PinSource10
#define WIFI_UART_RX_PIN        		 GPIO_Pin_10
#define WIFI_UART_RX_RCCCLK    			 RCC_AHB1Periph_GPIOA

#define UHOMEMAX_IUNUM 16

extern bool isWifiValid;				// 判断WIFI 模块有效

typedef enum{
	UHOMECOM_INIT = 0,	// 初始化
	UHOMECOM_IDLE,		// 空闲
	UHOMECOM_CFG,			// 配置
	UHOMECOM_NORMAL, //正常模式
	UHOMECOM_RX,			// 接收
	UHOMECOM_TX		// 正常发送
}UHOMECOMSTATE;

typedef enum{
	FRM_NONE = 0,				//无应答帧
	FRM_REPLYCONFIRM,		// 回复确认帧
	FRM_REPLYCONFIRM65,	//65确认帧
	FRM_REPORTSTATE,		// 汇报状态帧
	FRM_INVALID,				// 无效帧
	FRM_ALARM,					// 报警帧
	FRM_IDCHK,					//设备识别码应答
	FRM_VERSION,				//设备版本帧应答
	FRM_GETPASS,				//获取秘钥帧
	FRM_REPORTALARM,		//汇报报警帧
	FRM_CONFIG,					//进入配置模式帧
	FRM_NORMAL,					//进入正常模式帧
	FRM_REPORTCONFIG,		//设备汇报配置帧
}UHOMECOMFRAMETYPE;

typedef enum{
	OUTYP_UNDEF = 0,
	OUTYP_ALMPIC = 1,
	OUTYP_5IN1 = 2,
	OUTYP_HX7 = 3,
	OUTYP_SIDEFAN = 4,
	OUTYP_MIT5IN1 = 5,
	OUTYP_KVR = 6
}OUTYPE;

typedef struct {
	u8 mode;
	u8 state;
	u8 temp;
	u8 windspeed;
	u8 windUpDown;
	u8 windLeftRight;
}AirCondition;



typedef struct
{
//-----通讯计数相关--------
    u8 ComSTS;               //内机通信状态：0－本轮无通讯；1－本轮通讯但校验和错误；2－本轮通讯且校验和正确
    u8 ComErrCnt;            //通讯错误计数（包含无通讯或校验错误）
//------监控使用-------------
    u8 _IUType;      //机型
	
    u8 ValidExist       :1;
	u8 _FanSpeed        :2;  //风机转速
	u8 _DrainPump       :1;  //排水泵
	u8 _FloatSW         :1;  //浮子开关
	u8 _Frz_Ovht        :2;  //冻结过负荷
	u8 _res1            :1;
//---------通讯（状态数据）----------------
    u8 _IOAddress;           //内外机通信地址
    u8 _CCAddress;           //D0-集控地址

	u8 _DrvMode         :2;  //D1-驱动模式
	u8 _CCCtlMode       :2;  //D1-集控器控制模式
    u8 _WCAddress       :4;  //D1-成组地址
	
    u8 _D2TsetDef       :3;  //
	u8 _Mode_cmd        :4;  //D2-模式命令
    u8 _OnOff           :1;  //D2-开关机
	
	u8 _HP_CODE         :4;  //D3-HP code	
	u8 _Scode           :4;  //D3-Scode
	
    u8 _D4b10_Undef     :2;  //D4-未定义
    u8 _OutHealth       :1;  //D4-外机健康运转
    u8 _ErrClear1       :1;  //D4-故障状态清除1
    u8 _RatingRun       :1;  //D4-额定运转
    u8 _ForceRun        :1;  //D4-强制运转
    u8 _TimeShort       :1;  //D4-缩时标志
    u8 _ForceDefrost    :1;  //D4-强制除霜标志

    u8 _FreshAir        :2;  //D5-新风
    u8 _D5b2_Undef      :1;  //D5-未定义
    u8 _ErrClear2       :1;  //D5-故障状态清除
    u8 _HealthCmd       :1;  //D5-健康运转
    u8 _SwingCmd        :1;  //D5-摆风
    u8 _FanSpeedCmd     :2;  //D5-设定风速

    u8 _FanChable       :1;  //D6-风量调整允许
    u8 _SwingEnable     :1;  //D6-摆风功能允许
    u8 _HeatModeEnable  :1;  //D6-制热模式允许
    u8 _AutoModeEnable  :1;  //D6-自动模式允许
    u8 _Heater          :1;  //D6-电加热状态
	u8 _PreHeat         :1;  //D6-预热状态
	u8 _StandBy         :1;  //D6-待机状态
	u8 _FilterReq       :1;  //D6-清洗滤网指示

    u8 _ErrCode;             //D7-故障代码
//---------通讯（传感器数据）----------------
    s16 _TaiCmd;             //D2-设定温度
    s16 _Tset;               //D2-设定温度（带补偿）
    s16 _Tai;                 //D2-内机环境温度      
    s16 _Tc2;                 //D3-细管温度
    s16 _Tc1;                 //D4-粗管温度
    u16 _PMV;                 //D5-膨胀阀当前开度
//-------------------------
    u16 _HP;
//	u32 _OnOffTimeSec;

	//------新通讯解码（与旧协议区别部分）----------
	u8 _OpMode            : 2;
	u8 _Quiet             : 1;
	u8 _RmCardDrv         : 1;
	u8 _CoActionDrv       : 1;
	u8 _DrySTS            : 1;
    u8 _Swing             : 1;  //摆风
    u8 _Health            : 1;  //健康运转
	
	u8 _PMVReason         : 4;
	u8 _ADGroupId         : 2;
	u8 _CoAction          : 1; //联动输出
	u8 _Res1              : 1;
	
	u8 _HtrOnReqPR        : 3; //水箱、换热单元的电加热开启请求优先级（0-无开启请求，7-最高优先级开启请求）
	u8 _OuCmdIuHtrOnEn    : 1; //室外机命令：允许室内机开启电加热器信号（特别针对五合一的换热单元）（＝1允许，＝0不允许）
	u8 _ReOuCmdIuHtrOnEn  : 1; //水箱、换热单元对外机的电加热开启允许信号的应答（握手信号）
	u8 _OuCmdThermoOff    : 1; //室外机命令：强制室内机ThermoOff（＝1控制，＝0不控制）
	u8 _ReOuCmdThermoOff  : 1; //所有室内单元对室外机强制ThermoOff的回信（握手信号）
	u8 _ReOuCmdIuHtrOnReq : 1; //水箱、换热单元对外机的电加热开启请求信号的应答（握手信号）
	
	//------换热单元新方案更改后的内容（换热单元接水箱、地暖、烘干机）----------
	u8 _HU_WaterCmd       : 1; //制热水设定命令
	u8 _HU_FloorCmd       : 1; //地暖设定命令
	u8 _HU_DryCmd         : 1; //烘干设定命令
	u8 _HU_SolarCmd       : 1; //太阳能设定命令
  
	u8 _HU_WaterSts       : 1; //制热水状态
	u8 _HU_FloorSts       : 1; //地暖状态
	u8 _HU_DrySts         : 1; //烘干状态
	u8 _HU_SolarSts       : 1; //太阳能状态

	u8 _HU_PowerSaveCmd   : 1; //设定节能
	u8 _HU_SterilizeCmd   : 1; //设定消毒
	u8 _HU_QuietCmd       : 1; //设定静音
	u8 _HU_WaterCmdPower  : 1; //制热水设定命令类型：强力制热水
	u8 _Res7              : 4;
	
    s16 _TfloorCmd;            //D2-地暖设定温度
    s16 _TfloorSet;            //D2-地暖设定温度（带补偿）
    s16 _TdryCmd;              //烘干设定温度
    s16 _TdrySet;              //D2-设定温度（带补偿）
	s16 _Twi;
	s16 _Two;

	s16 _Twe;
	s16 _Tws;

	u8 _HU_D11;
	u8 _HU_D26;
	u8 _HU_D27;
	u8 _HU_D28;
	
    u16 _PMV2;          
    u16 _DCFanSpeed;          
	s16 _SHSCTar;
    s16 _Taf;              
    s16 _Tc22;          
    s16 _Tc12;
	
    u8  _DCFanDuty;          
	u8 _CommanderId;
	u8 _SoftVer;
} TIU;
typedef struct
{
	//OUTDOORTYPE _OuType; //D2-机型识别标志
	//------通讯解码----------
	u8  _OpMode         :2;  //D0-运转模式
	u8  _OuCtlMode      :2;  //D0-外机设定模式：00-通常，01-单冷，02-单热，03-热回收
    u8  _Address        :3;  //D0-地址
    u8  _HOffPMVSCctlFlg:1;  //D0b0 1/0 - 制热感温器Off调阀/不调 开机的室内机总能力小于95Hz==8P调阀
	
	u8  _DefrostFlg     :1;  //除霜运转中
	u8  _XRDefrostFlg   :1;  //蓄热除霜运转中
	u8  _Res1           :6;  //预留
	
	u16 _INVSpeed;           //D1-压缩机运转频率
	
	u8  _ressss         :3;  //
	u8  _TimerShort     :1;  //D2-缩时
	u8  _CompBackupRun  :1;  //D2-压缩机后备运转
	u8  _PdBackupRun    :1;  //D2-高压传感器后备运转中
	u8  _SH_MODE        :2;  //D2-内机过热度控制方式

	u8  _MisMatchCheck  :1;  //D3-误配线检测运转中
	u8  _OilRetFlg      :1;  //D3-回油运转中
	u8  _OilBalFlg      :1;  //D3-均油运转中
	u8  _ForceRun       :1;  //D3-强制运转中
	u8  _QuietRun       :1;  //D3-低噪音运转中
	u8  _PowerSave      :1;  //D3-节能运转
	u8  _AutoAddress    :1;  //D3-地址自动设定中
	u8  _RateRun        :1;  //D3-额定运转中

    u8  _OnIUCtl_PMV_EN :1;  //D4-开机内机（ThermoOn）PMV使能
    u8  _OffIUCtl_PMV_EN:1;  //D6-开机内机（ThermoOff）PMV使能
    u8  _OnIUCtl_PMV_MODE:2; //D5-外机对开机的内机（感温器ON）PMV开度进行修正控制
    u8  _CoolSHoffsetDir :1; //D5-制冷内机过热度修正方向：1表示+，0表示-
    u8  _CoolSHoffsetVal :1; //D5-制冷内机过热度修正值：1表示1，0表示0
    u8  _Res2            :2; //D5-预留
	
	u16 _OnIUCtl_PMV_Step;   //D4-on PMV目标开度
	u16 _OffIUCtl_PMV_Step;  //D6-off PMV目标开度
	
    s16  _Pd_temp;	
    s16  _Ps_temp;
    s16  _Tdi;     
    s16  _Tsi;     
    s16  _Tao;     
    s16  _Tdef;   
    s16  _Toili;  	
    s16  _Tfin;
	
    u8  _ErrCode;
	u8 _ComStage;  //通讯过程解码  =0，通常；=1，地址竞争开始；=2，地址竞争结束

	//------新通讯解码（与旧协议区别部分）----------
	u8  _SoftVer;
	u8  _RunSts;
	u8  _RunStsStep;
    u8  _ErrCodeSub;
    s16 _SHoffset_IU;
	u16 _FanSpeed;
	u16 _LEVa;
	
	u8  _4WV    :1;
	u8  _SV1    :1;
	u8  _SV2    :1;
	u8  _CHi    :1;
	u8  _CHa    :1;
	u8  _SVx    :1;
	u8  _Res3   :2;

	u8  _CmdIUTHermoOff     : 1;   //强制室内机ThermoOff（＝1控制，＝0不控制）
	u8  _5in1HeaterOnEnable : 1;   //允许五合一的水箱、板换开启电加热器信号（＝1允许，＝0不允许）
	u8  _5in1HeaterOnReq    : 1;   //请求五合一的水箱、板换开启电加热器信号（＝1请求，＝0无）
	u8  _res5               : 5;
	
	
	/*下为新增外机参数*/	
	u8  _E2Ver;
	u8  _CT;          
	u8  _Icm;         /*  压机马达电流  */
	u8  _INVDBV;      /*  (TSL)压机DC Bus voltage，单位：4V */  //高低压开关
	u16 _FanSpeed2;
	u8  _Data1;
	u8  _Data2;
	u8  _Data3;
	u8  _Data4;
	u8  _Data5;
	
	//室外机高落差:控制全部/单台内机PMV 测试专用
	u8 _PMVPosEnCmd	    	:1;  //PMV目标开度使能位
	u8 _HeatTC2TempEnCmd	:1;  //制热内机Tc2目标温度使能位
	u8 _HeatTarSCEnCmd	    :1;  //制热内机目标过冷度使能位
	u8 _COOLTarSHEnCmd	    :1;  //制冷内机目标过热度使能位
	u8 _ThermoOnPMVRateEnCmd:1;  //ThermoOn内机 PMV目标开度为当前开度的比例使能位
	u8 _Ret5EnCmd 	        :3;  //预留
	
	u16 _PMVPosCmd;              //PMV目标开度
	s16 _Tc2Cmd;                 //制热内机Tc2目标温度
	s16 _TarSHSCCmd;             //制冷内机目标过热度（Tc1-Tc2）/制热内机目标过冷度（Pd-Tc2）
	u8  _ThermoOnPMVRateCmd;     //ThermoOn内机 PMV目标开度为当前开度的比例（范围50-200）  （50表示50%，200表示200%）
	
	u8 _FanCtlEnCmd	    	:1;  //内机风机控制使能位
	u8 _FanCtlCmd	    	:2;  //bit 2-1: 内机风速 （0-停  1-低风 2-中风 3-高风）
	u8 _Ret6EnCmd 	        :5;  //预留
	
} TOU;
uint8_t Wifi_Data_Ready(void);
void Wifi_GPIO_Init(void);
void Wifi_Uart_Init(void);
void Wifi_Uart_SendByte(uint8_t c);
void Wifi_Uart_SendData(uint8_t *DAta, uint32_t DAtaNUm);
void Wifi_UART_IRQHandler(void);

void Wifi_Uart(void);
void Wifi_Test(void);
void RxData_Deal(void);
void Test_All(void);
void UhomeCom_Reset(void);		// uhome通讯初始化
void AlarmTmrRun();
extern OUTYPE OutDoorUnitType;		// 外机类型，根据机型报对应故障代码

bool HasNewCmd_UHOME(void);

void SetNewCmdFlag_UHOME(void);

#endif //__WIFI_H
