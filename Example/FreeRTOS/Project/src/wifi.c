/*
* Uhome 网络家电协议
* 与WIFI 模块通过UART 通讯
* 9600bps   1 startbit 1 stopbit 无校验
*/
#include "wifi.h"
#include "modecmd.h"
#include "Gcom.h"
#include "error.h"
#include "timer.h"
#include "record.h"

//收发缓冲区
#define UHOMEBUFSIZE 150
#define UHOMEMAXLENGTH 111				// 有效帧长8-111

#define UHOMECOM_FREETIME_200MS (200*5)				
#define UHOMECOM_FREETIME_5000MS (1000*5) 	// 汇报1s静默期

#define UHOMECOM_ALARMTIME_200MS (8)		// 200MS 首次曝出故障 间隔200ms : 单位 25ms
#define UHOMECOM_ALMRMTIME_5000MS (200)		// 报警收到ACK 帧后隔5000 ms重发: 单位 25ms
#define UHOMECOM_CONNECTERR_10MIN (10*60*40)// 内机板10min未收到wifi模块任何数据帧报警


uint8_t get_device[] = {0xFF, 0xFF, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2, 0x00, 0x02, 0xFE};
uint8_t MACADDRESS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
uint8_t WIFICONFIG[12] = {0xFF, 0xFF, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2, 0x00, 0x02};
uint8_t WIFINORMAL[12] = {0xFF, 0xFF, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00};
static u8 UhomeRxBuffer[UHOMEBUFSIZE];
static u8 UhomeTxBuffer[UHOMEBUFSIZE]; //UhomeRxBuffer      //Tx、Rx共用一个缓冲区，节约空间（不能同时收发）
static u8 UhomePreBuffer[UHOMEBUFSIZE];
static u8 UhomeBuffer[UHOMEBUFSIZE];
#define UHOMENAKCOUNT    (200 - 1)   		//无通讯次数或校验和错误计数，连续200次认为该WIFI通信异常（1秒约8次）
#define UHOMECOMNORMALCOUNT    (10)  	//有通讯次数计数 连续10次认为该WIFI存在

static u8 UhomeFrameTxSize= 0;		// 发送帧长度

static u8 UhomeFrmLength = 0;		// 有效帧长度
static u8 UhomeFrameRxSize = 0;       //接收帧长度
static u8 UhomeFrameRxPos = 0;  //接收帧位置
static u8 UhomeDataStartIdx = 0; 	// 数据信息起始位置

bool isWifiValid = FALSE;				// 判断WIFI 模块有效
static u16 InvalidInfoCode_Uhome = 0xffff;	// 无效信息代码
static u16 WifiConnectChkTmr = 0;	
static u8 FLG_CHKSUM_OK = 0;
bool UhomeIsEn=FALSE;      				//收到加密方式
u8 UhomePassMethod = 0;						//串口加密方法
u8 UhomePassParameter[2];					//加密参数
u8 UhomeWifiConfig=0;      //是否进入配置模式
u8 UhomeWifiNormal=0;      //是否进入配置模式
u8 UhomeReport=0;					 //是否主动汇报状态信息

static UHOMECOMSTATE UhomeCom_Sts = UHOMECOM_INIT;
static UHOMECOMFRAMETYPE UhomeTxFrmTyp = 0xff;	// 主板应答帧类型

static u8 UhomeCmdAckAdd = 0;
static bool isRecieveByte = false;
static bool isReply	= false;
static bool isState=false;
uint32_t a=0;
u8 AlarmAck=0;
u8 ReportAck=0;
u8 Ack65=0;
u16 ReportIntTmr;				//汇报间隔时间
u16 ReportIntTmrReceive; //汇报时间间隔   100ms
TIU IU[128];
static u8 UhomeWifiMacBuf[6];				// 模块网关地址存储
static u16 timesecond=0;
AirCondition AirConditionData = {0x08, 0x01, 0x09, 0x05, 0x0c, 0x0c};
AirCondition AirConditionReceiveData = {0x08, 0x01, 0x09, 0x05, 0x0c, 0x0c};
struct
{
	u16 FreeTmr;				// 报警空闲时间
	u16 SetFreeTmr;			// 需设定报警空闲时间
	u8 CodeOld;				// 故障代码备份
	u8 Occured:1;			// 发生过标志
	u8 AlarmEn:1;			// 允许发送报警帧
	u8 res:6;
}UhomeIuErr[128];
//**********************************************************

static void UhomeUnpack(void);
static void UhomePack(void);
static void InvalidPack(void);
static void ReplyConfirmPack(void);
static void ReplyConfirmPack65(void);
static void ReportStatePack(void);
static void GroupControlPack(void);
static void IDChkPack(void);
static void VersionPack(void);
static void GetPassPack(void);
static void AlarmPack(void);
static void ReportConfigPack();
static void ReportAlarmPack(void);
void AlarmTmrRun(void);		// 报警间隔时间计时
void AlarmTmrRun(void);
void UhomeCom_1toX(void);	// 一控多
void UhomeWifi_Config(void);//进入配置模式
void UhomeWifi_Normal(void);//进入工作模式
void UhomeSetMacAddress(void);//设置Mac地址
static void SetAirCondition(void);//设置空调信息
bool IsNeedErrPro(void);		// 有故障需要处理
bool isStateChg(void);			//状态信息是否改变
void UhomeSetIdenty(void);	//设置地址标识
void EnCode(void);					//加密
void DeCode(void);					//解密

static const char menu[] = 
  "*-command-*--- function -------*\r\n"
  "* s/S     * Enter Config Mode  *\r\n"
  "* a/A     * Report Alarm       *\r\n"
	"* m/M     * Set The Mode       *\r\n"
	"* t/T     * Set The Temp       *\r\n"
	"* w/W     * Set The WindSpeed  *\r\n";

void Test_All(void)
{
	uint8_t a,s,n,r,m,t,w,d;
	Tenms_Timer_Init();
	while(1)
	{
		UhomeCom_1toX();
		if(a!=0)
		{
			debug_printf(menu);
			printf("Mode:%x Temp:%x WindSpeed:%x\n",AirConditionData.mode,AirConditionData.temp,AirConditionData.windspeed);
			debug_printf("Press any key to stop/quit current test.\r\n");
			debug_printf ("\nCmd> ");                       /* display prompt             */
		}
		//a = debug_getchar (0);
		//debug_printf("%d\r\n", c);
		
		switch (a)
		{
			case 0:
				break;
			case 'S':
			case 's':
			{
				
			  UhomeWifiConfig=1;
				break;
			}
			case 'n':
			case 'N':
			{
				UhomeWifiNormal=1;
				break;
			}
			case 'm':
			case 'M':
			{
				debug_printf ("\nPlease Input Mode:");                       /* display prompt             */
				//m = debug_getchar (1);
				switch(m)
				{
					case '1':
						AirConditionReceiveData.mode=0x01;
						break;
					case '2':
						AirConditionReceiveData.mode=0x02;
						break;
					case '3':
						AirConditionReceiveData.mode=0x04;
						break;
					case '4':
						AirConditionReceiveData.mode=0x08;
						break;
				}
				break;
			}
			case 't':
			case 'T':
			{
				debug_printf ("\nPlease Input Temp:");                       /* display prompt             */
			//	t = debug_getchar (1);
				switch(t)
				{
					case '0':
						AirConditionReceiveData.temp=0x00;
						break;
					case '1':
						AirConditionReceiveData.temp=0x01;
						break;
					case '2':
						AirConditionReceiveData.temp=0x02;
						break;
					case '3':
						AirConditionReceiveData.temp=0x03;
						break;
					case '4':
						AirConditionReceiveData.temp=0x04;
						break;
					case '5':
						AirConditionReceiveData.temp=0x05;
						break;
					case '6':
						AirConditionReceiveData.temp=0x06;
						break;
					case '7':
						AirConditionReceiveData.temp=0x07;
						break;
					case '8':
						AirConditionReceiveData.temp=0x08;
						break;
					case '9':
						AirConditionReceiveData.temp=0x09;
						break;
					case 'a':
						AirConditionReceiveData.temp=0x0a;
						break;
					case 'b':
						AirConditionReceiveData.temp=0x0b;
						break;
					case 'c':
						AirConditionReceiveData.temp=0x0c;
						break;
					case 'd':
						AirConditionReceiveData.temp=0x0d;
						break;
					case 'e':
						AirConditionReceiveData.temp=0x0e;
						break;
				}
				break;
			}
			case 'w':
			case 'W':
			{
				debug_printf ("\nPlease Input Wind:");                       /* display prompt             */
			//	w = debug_getchar (1);
				switch(w)
				{
					case '1':
						AirConditionReceiveData.windspeed=0x01;
						break;
					case '2':
						AirConditionReceiveData.windspeed=0x02;
						break;
					case '3':
						AirConditionReceiveData.windspeed=0x03;
						break;
					case '4':
						AirConditionReceiveData.windspeed=0x05;
						break;
				}
				break;
			}
			case 'a':
			case 'A':
				UhomeIuErr[0].CodeOld=0;
				IU[0]._ErrCode=1;
				break;
			default:
			{
				debug_printf("Udefined command, check and input again!\r\n");
				break;
			}
		}
	}
}
/**************************************************************************************
 * 函数：Wifi_GPIO_Init()
 * 描述：tx、rx引脚设置
 * 输入：无
 * 输出：无
 * 返回：无
**************************************************************************************/
void Wifi_GPIO_Init(void)
{
  /* 定义GPIO、UART初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 使能GPIOA、USART1时钟 */
  RCC_AHB1PeriphClockCmd(WIFI_UART_TX_RCCCLK | WIFI_UART_RX_RCCCLK, ENABLE);
  RCC_APB2PeriphClockCmd(WIFI_UART_RCCCLK, ENABLE);
  /* 配置PA9、PA10链接到USART1 */
  GPIO_PinAFConfig(WIFI_UART_TX_GPIO, WIFI_UART_TX_PINS, WIFI_UART_GPIO_AF);
  GPIO_PinAFConfig(WIFI_UART_RX_GPIO, WIFI_UART_RX_PINS, WIFI_UART_GPIO_AF);
  /* 初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
  GPIO_InitStructure.GPIO_Pin = WIFI_UART_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WIFI_UART_TX_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
  GPIO_InitStructure.GPIO_Pin = WIFI_UART_RX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WIFI_UART_RX_GPIO, &GPIO_InitStructure);
}
/**************************************************************************************
 * 函数：Wifi_Uart_Init()
 * 描述：wifi串口初始化，包括接收中断，参数配置
 * 输入：无
 * 输出：无
 * 返回：无
**************************************************************************************/
void Wifi_Uart_Init(void)
{
   USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	 Wifi_GPIO_Init();
	  /**************************************************************************** 
   *串口配置： 
   *    波特率：9600
   *    字长：  8bit
   *    停止位：1bit
   *    流控制：不使用
   ****************************************************************************
   */
	USART_InitStructure.USART_BaudRate = 9600;//9600
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//USART_WordLength_8b
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;// USART_Parity_Even
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(WIFI_UART, &USART_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = WIFI_UART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0C;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);
  USART_ITConfig(WIFI_UART, USART_IT_RXNE, ENABLE);
  USART_Cmd(WIFI_UART, ENABLE);
}
/**************************************************************************************
 * 函数：Wifi_Uart_SendByte()
 * 描述：wifi串口发送一个字节到wifi模块
 * 输入：c--待发送字节
 * 输出：无
 * 返回：无
**************************************************************************************/
void Wifi_Uart_SendByte(uint8_t c)
{
	//uint8_t ret = 0, retry = 0;
	USART_SendData(WIFI_UART, c);
	while (USART_GetFlagStatus(WIFI_UART, USART_FLAG_TXE) == RESET)
	{
		;
	}
}
/**************************************************************************************
 * 函数：Wifi_Uart_SendData()
 * 描述：wifi串口发送一个特定长度的数组到wifi模块，该函数稍加封装即可得到给wifi模块发送命令的函数，一般情况下是根据wifi端设定的
				 协议（即命令格式），添加包头、包类型、校验等信息。
 * 输入：DAta--待发送数组首地址
				 DAtaNUm--待发送数组长度
 * 输出：无
 * 返回：无
**************************************************************************************/
void Wifi_Uart_SendData(uint8_t *DAta, uint32_t DAtaNUm)
{
	while ((DAtaNUm)--)
	{
			Wifi_Uart_SendByte(*DAta);
			DAta++;
	}
}
/**************************************************************************************
 * 函数：Wifi_UART_IRQHandler()
 * 描述：wifi串口接收中断处理函数，一旦有数据从wifi模块传来，硬件自动进入该函数，每次处理一个字节，按字节往WifiUartRxBuffer存入，
				 直到当前数据包（或称帧）接收完毕，接收的数据长度为WifiRecvNum，每接收完一个数据包都要将其从WifiUartRxBuffer中取出进行解析，
				 并清除WifiUartRxBuffer中数据，等待下一次接收。
 * 输入：DAta--待发送数组首地址
				 DAtaNUm--待发送数组长度
 * 输出：无
 * 返回：无
**************************************************************************************/
void USART1_IRQHandler(void)
{
	uint8_t RxData;
	if(USART_GetITStatus(WIFI_UART, USART_IT_RXNE) != RESET)
	{
		RxData = USART_ReceiveData(WIFI_UART);
		if(UhomeFrameRxPos < UHOMEBUFSIZE)
		{
			if(UhomeFrameRxPos<=1)
			{
				if(RxData==0xFF)
				{
					UhomeRxBuffer[UhomeFrameRxPos] = RxData;
					UhomeFrameRxPos++;
				}
				else
				{
					UhomeFrameRxPos=0;
				}
			}
			else
			{
				UhomeRxBuffer[UhomeFrameRxPos] = RxData;
				UhomeFrameRxPos++;
			}
		}
		SetFastTmrVal(Uhome_BusFreeTmr,UHomeCom_BUSFREETMR);
		isRecieveByte = true;
	}
}
/************************************************************************************
   * @函数名	Mermory_Clear
   * @函数功能	将模块内存清零
   * @输入参数	pMErmoryDAta-内存首地址
				DAtaLEngth-需清零长度
   * @输出参数	无
   * @函数返回	无
************************************************************************************/			 
void Mermory_Clear(uint8_t *pBuf, uint8_t *pLen)
{
	uint8_t *pdata=pBuf;
	while(pdata < pBuf + *pLen)
		*pdata++=0;
	*pLen = 0;
}
/************************************************************************************
   * @函数名	RxData_Deal()
   * @函数功能	处理接收到的数据
   * @输入参数	无
   * @输出参数	无
   * @函数返回	bool
************************************************************************************/	
void RxData_Deal(void)
{
	if(UhomeFrameRxSize>10)
	{
		UhomeWifiMacBuf[0] = UhomeRxBuffer[3];	
		if(UhomeWifiMacBuf[0]==0x80)
		{
			UhomeIsEn=true;
		}
		else
		{
			UhomeIsEn=false;
		}
		UhomeUnpack();
	}
	Mermory_Clear(UhomeRxBuffer,&UhomeFrameRxSize);
	UhomeFrameRxSize=UhomeFrmLength+2;
	Mermory_Clear(UhomePreBuffer,&UhomeFrameRxSize);
	UhomeCom_Sts = UHOMECOM_IDLE;
	isRecieveByte=false;
}
void Wifi_Uart(void)
{
	Tenms_Timer_Init();
	while(1)
	{
		UhomeCom_1toX();
	}
}

void UhomeCom_1toX(void)
{
	AlarmTmrRun();
	switch(UhomeCom_Sts)
	{
		case UHOMECOM_INIT:
			Wifi_Uart_Init();
			UhomeCom_Sts = UHOMECOM_IDLE;
			break;
		case UHOMECOM_IDLE:
				if(isRecieveByte!=FALSE)			// 接收指令
				{
					UhomeCom_Sts = UHOMECOM_RX;
				}
				else if((UhomeWifiConfig!=0)&&(FALSE!=isWifiValid))			// (检测在配置状态)&&(wifi模块有效)
				{
					//uhomeConfigFrmTmr = getGTimerValue(UhomeStsReportTmr);
					UhomeCom_Sts = UHOMECOM_CFG;						// 处于配置状态
				}
				else if((UhomeWifiNormal!=0)&&(FALSE!=isWifiValid))
				{
					UhomeCom_Sts = UHOMECOM_NORMAL;
				}
				else if(IsNeedErrPro()!=false)
				{
					UhomeTxFrmTyp=FRM_REPORTALARM;
					AlarmAck=1;
					UhomeCom_Sts = UHOMECOM_TX;
				}
				else if(isStateChg()!=false)
				{
					UhomeTxFrmTyp=FRM_REPORTSTATE;
					ReportAck=1;
					UhomeCom_Sts = UHOMECOM_TX;
				}
			break;
		case UHOMECOM_RX:
			if(GetFastTmrVal(Uhome_BusFreeTmr)==0)
			{
				UhomeFrameRxSize=UhomeFrameRxPos;
				UhomeFrameRxPos=0;
				RxData_Deal();
				if(isReply==true)
				{
					UhomeCom_Sts = UHOMECOM_TX;
					isReply=false;
				}
				else
				{
					UhomeCom_Sts = UHOMECOM_IDLE;
				}
			}
			break;
		case UHOMECOM_TX:
			if(GetFastTmrVal(Uhome_BusFreeTmr)==0)
			{
				UhomePack();
  			Wifi_Uart_SendData(UhomeTxBuffer,UhomeFrameTxSize);
				if(AlarmAck==1)
				{
					UhomeIuErr[0].Occured = 0;
					UhomeIuErr[0].FreeTmr = UhomeIuErr[0].SetFreeTmr;
				}
				if(ReportAck==1)
				{
					ReportIntTmr = 40;
				}
				SetFastTmrVal(Uhome_BusFreeTmr,UHOMECOM_FREETIME_200MS);
				UhomeCom_Sts = UHOMECOM_IDLE;
			}	
			break;
		case UHOMECOM_CFG:	
			UhomeTxFrmTyp=FRM_CONFIG;
			UhomeCom_Sts = UHOMECOM_TX;
			break;
		case UHOMECOM_NORMAL:	
			UhomeTxFrmTyp=FRM_NORMAL;
			UhomeCom_Sts = UHOMECOM_TX;
			break;
		default:
			UhomeCom_Sts = UHOMECOM_INIT;
			break;
	}
}
static void UhomeUnpack(void)
{	
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u16 uhomeMiuCCaddress = 0;
	u8 uhomePreFrmLength = 0;
	u8 uhomeRxBuf55Cnt = 0;
	u8 uhomeChkSum = 0;
	u8 uhomeRxFrmTyp = 0;
	u16 uhomeAckAdd = 0;

	if((0xff==UhomeRxBuffer[0])&&(0xff==UhomeRxBuffer[1]))
	{
		UhomeDataStartIdx = 2;//10;	// 由模块地址就开始 55 规则
		UhomeFrmLength = UhomeRxBuffer[2];

		if((UhomeFrmLength>=8)&&(UhomeFrmLength<=111))		// 协议规定有效帧长度8~111
		{
			j = 0;
			k = 0;
			uhomeRxBuf55Cnt = 0;

			for(j = 0;j<UhomeFrameRxSize;j++)	// 数据信息，0XFF 0X55组合中剔除0X55
			{
				if(j<=UhomeDataStartIdx)	UhomePreBuffer[k++] = UhomeRxBuffer[j];
				else
				{
					if(0xff==UhomePreBuffer[k-1])
					{
						if(0x55!=UhomeRxBuffer[j])
						{
							UhomePreBuffer[k++] = UhomeRxBuffer[j];
						}
						else
						{
							if(k<(UhomeFrmLength+3-1))uhomeRxBuf55Cnt++;
						}
					}
					else
					{
						UhomePreBuffer[k++] = UhomeRxBuffer[j];
					}
				}
			}
			if((UhomeIsEn==true)&&(Ack65==0))
			{
				DeCode();
			}
			uhomeChkSum = 0;
			for(j=2;j<(UhomeFrmLength+2);j++)			// 计算校验和
			{
				uhomeChkSum+=UhomePreBuffer[j];
			}
			if((UhomePassMethod==0)||(UhomeIsEn==false))
			{
				uhomeChkSum += 0x55*uhomeRxBuf55Cnt;
			}

			if(uhomeChkSum == UhomePreBuffer[UhomeFrmLength+3-1])	// PreBuffer[]里是剔除55后真正的数据
			{
				FLG_CHKSUM_OK = 1;	// 校验正确
				isWifiValid = TRUE; // WIFI 模块有效
				//WifiConnectChkTmr = UHOMECOM_CONNECTERR_10MIN;	// wifi模块10min检测
			}
			else
			{
				FLG_CHKSUM_OK = 0;	// 校验错误
			}
		}
	}
	else
	{
		
	}
	if(FLG_CHKSUM_OK)
	{
		FLG_CHKSUM_OK = 0;
		UhomeWifiMacBuf[0] = UhomePreBuffer[3];	// WIFI MAC
		UhomeWifiMacBuf[1] = UhomePreBuffer[4];
		UhomeWifiMacBuf[2] = UhomePreBuffer[5];
		UhomeWifiMacBuf[3] = UhomePreBuffer[6];
		UhomeWifiMacBuf[4] = UhomePreBuffer[7]; // 位对应机号
		UhomeWifiMacBuf[5] = UhomePreBuffer[8]; // 位对应机号

		uhomeRxFrmTyp = UhomePreBuffer[9];

		switch(uhomeRxFrmTyp)
		{
			case 0x03:
				Ack65=0;
				break;
			case 0x05:											// ACK帧
				//UhomeWifiConfig=0;
				if(Ack65==1)
				{
					if(AlarmAck==1)
					{
						UhomeIuErr[0].SetFreeTmr = UHOMECOM_ALMRMTIME_5000MS;
						UhomeIuErr[0].FreeTmr = UhomeIuErr[0].SetFreeTmr;
						AlarmAck=0;
					}
					else if(ReportAck==1)
					{
						ReportAck=0;
						isState=false;
					}
					Ack65=0;
				}
				break;
			case 0x09:											// 停止故障报警帧
				UhomeIuErr[0].AlarmEn = 0;
				Ack65=0;
				break;
			case 0x60:											
				SetAirCondition();
				Ack65=0;
				break;
			case 0x61:											//查询设备版本帧
					UhomeTxFrmTyp=FRM_VERSION;
					isReply=true;
				break;
			case 0x64:											//设置密码方法及参数	
				//回复确认帧
				UhomeTxFrmTyp=FRM_REPLYCONFIRM;
				isReply=true;
				UhomePassMethod=UhomePreBuffer[10];								//串口加密方法
				UhomePassParameter[0]=UhomePreBuffer[11];					//加密参数
				UhomePassParameter[1]=UhomePreBuffer[12];					//加密参数
				break;
			case 0x65:											//组地址组命令帧
				GroupControlPack();
				break;
			case 0x70:											// 设备识别码查询帧
				UhomeTxFrmTyp=FRM_IDCHK;
				isReply = true;
				break;
			case 0x73:											//查询报警状态帧
				UhomeTxFrmTyp=FRM_ALARM;
				isReply = true;
				Ack65=0;
				break;
			case 0x7C:											//设备汇报配置帧
				UhomeTxFrmTyp=FRM_REPORTCONFIG;
				isReply=true;
			  ReportIntTmrReceive=((u16)UhomePreBuffer[10]<<8)|UhomePreBuffer[11];
				break;
			case 0x8A:											//下行分包透传
				break;
			case 0xF1:											//网络状态应答帧
				break;
			case 0xF3:											//Wifi进入设置模式应答帧
				UhomeWifiConfig=0;
				break;
			case 0xF5:											//Wifi进入工作模式应答帧
				UhomeWifiNormal=0;
				break;
			case 0xF6:											//出发设备向本地WIfi模块发送查询设置及应答
				break;
			case 0xF7:											//主动汇报网络状态
				UhomeTxFrmTyp=FRM_REPLYCONFIRM;
				isReply = true;
				break;
			default:
				UhomeTxFrmTyp=FRM_INVALID;
				isReply = true;
				break;
		}
	}
	else
	{
		isReply = FALSE;	//　校验失败  不应答
	}
}
static void SetAirCondition(void)
{
		//回复确认帧；
	u8 cmd;
	cmd=UhomePreBuffer[11];
	if(cmd==0x01)
	{
		//暂时不处理
	}
	else if(cmd==0x02)
	{
		if(UhomePreBuffer[13]==0x00)
		{
			AirConditionData.state =0;
			printf("close the condition.\n");
		}
		else if(UhomePreBuffer[13]==0x01)
		{
			AirConditionData.state =1;
			printf("open the condition.\n");
		}
	}
	else if(cmd==0x03)
	{
		AirConditionData.temp =UhomePreBuffer[13];
		AirConditionData.windspeed =UhomePreBuffer[14];
		AirConditionData.windUpDown =UhomePreBuffer[17];
		AirConditionData.windLeftRight =UhomePreBuffer[18];
		
		AirConditionData.state = UhomePreBuffer[21];
		AirConditionData.mode = UhomePreBuffer[22];
		printf("mode:%x state:%x temp:%x windspeed:%x windUD:%x windFR:%x",
		AirConditionData.mode,
		AirConditionData.state,
		AirConditionData.temp,
		AirConditionData.windspeed,
		AirConditionData.windUpDown ,
		AirConditionData.windLeftRight
		);
	}
	else
	{
	}
	UhomeTxFrmTyp=FRM_REPLYCONFIRM65;
	isReply=true;
}
static void UhomePack(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 uhomeDataLength = 0;
	u8 uhomeChkSum = 0;
	u8 isReport=false;

	switch(UhomeTxFrmTyp)									// 一控多打包
	{
		case FRM_INVALID: InvalidPack();isReport=true; break;				// 无效帧应答
		case FRM_REPLYCONFIRM: ReplyConfirmPack(); isReport=true;break;		// 应答确认帧
		case FRM_REPLYCONFIRM65: ReplyConfirmPack65();isReport=true; break;		// 应答65确认帧
		case FRM_REPORTSTATE: ReportStatePack(); break;		// 汇报(主动 查询 单控)状态帧  对一台或多台内机进行了单控指令的应答帧  其他状态可能不一致 分帧应答
		case FRM_IDCHK: IDChkPack();isReport=true; break;					   // 设备码应答
		case FRM_VERSION: VersionPack();isReport=true; break;				// 设备版本应答
		case FRM_GETPASS: GetPassPack(); break;				// 获取秘钥应答
		case FRM_ALARM: AlarmPack();isReport=true; break;					 // 应答报警帧
		case FRM_REPORTALARM: ReportAlarmPack(); break;					 // 报警帧
		case FRM_REPORTCONFIG: ReportConfigPack(); break;					 // 报警帧
		case FRM_CONFIG: UhomeWifi_Config(); break;					 // 进入配置模式
		case FRM_NORMAL: UhomeWifi_Normal(); break;					 // 正常模式
		default:	ReplyConfirmPack(); break;
	}
	
	UhomeFrameTxSize=UhomeTxBuffer[2]+3;
	uhomeChkSum = 0;
	for(i= 2;i<UhomeFrameTxSize-1;i++)
	{
		uhomeChkSum += UhomeTxBuffer[i];
	}
	if(isReport==true)
	{
		if(UhomeIsEn==true)
		{
			EnCode();
		}
	}
	else
	{
		if(UhomePassMethod==1)
		{
			EnCode();
		}
	}
	uhomeDataLength = UhomeTxBuffer[2]-1;	// 此长度只表示有效负荷

	for(i=3;i<uhomeDataLength+3;i++)
	{
		if(0xff==UhomeTxBuffer[i])
		{
			j++;
		}
	}

	UhomeFrameTxSize = (uhomeDataLength+j+4);	// 整帧数据的长度
	if(j>0)
	{
		for(i=uhomeDataLength+2;i>2;i--)
		{
			if(0xff==UhomeTxBuffer[i])
			{
				UhomeTxBuffer[i+j] = 0x55;	// 0xff后加0x55
				UhomeTxBuffer[i+j-1] = UhomeTxBuffer[i];

				if(j>0)
				{
					j--;
				}

				if(0==j)	// 查找0xff结束
				{
					break;
				}
			}
			else
			{
				UhomeTxBuffer[i+j] = UhomeTxBuffer[i];
			}
		}
	}
	if((UhomePassMethod==0)||(UhomeIsEn==false))
	{
		uhomeChkSum = 0;
		for(i= 2;i<UhomeFrameTxSize-1;i++)
		{
			uhomeChkSum += UhomeTxBuffer[i];
		}
	}

	UhomeTxBuffer[UhomeFrameTxSize-1] = uhomeChkSum;

	if(0XFF==UhomeTxBuffer[UhomeFrameTxSize-1])
	{
		UhomeTxBuffer[UhomeFrameTxSize++] = 0x55;
	}
}

static void ReportStatePack(void)
{
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 50;		// 帧长 :

	UhomeSetIdenty();
	
	UhomeTxBuffer[9] = 0x65;
	UhomeSetMacAddress();
	
	UhomeTxBuffer[27] = 0x06;
	
	UhomeTxBuffer[28] = 0x6D;
	UhomeTxBuffer[29] = 0x01;
	
	UhomeTxBuffer[30] = 0x00;
	UhomeTxBuffer[31] = AirConditionData.temp;
	UhomeTxBuffer[32] = AirConditionData.windspeed;
	UhomeTxBuffer[33] = 0x00;
	UhomeTxBuffer[34] = 0x00;
	UhomeTxBuffer[35] = AirConditionData.windUpDown;
	UhomeTxBuffer[36] = AirConditionData.windLeftRight;
	UhomeTxBuffer[37] = 0x00;
	
	UhomeTxBuffer[38] = 0x00;
	UhomeTxBuffer[39] = AirConditionData.state;
	
	UhomeTxBuffer[40] = AirConditionData.mode;
	UhomeTxBuffer[41] = 0x00;
	
	UhomeTxBuffer[42] = 0x00;
	UhomeTxBuffer[43] = 0x00;
	
	UhomeTxBuffer[44] = 0x10;
	UhomeTxBuffer[45] = 0x10;
	UhomeTxBuffer[46] = 0x10;
	UhomeTxBuffer[47] = 0x00;
	UhomeTxBuffer[48] = 0x00;
	UhomeTxBuffer[49] = 0x00;
	UhomeTxBuffer[50] = 0x00;
	UhomeTxBuffer[51] = 0x00;
}
static void GroupControlPack(void)
{
	int i=0;
	for(;i<17;i++)
	{
		MACADDRESS[i]=UhomePreBuffer[i+10];
	}
	for(i=0;i<UhomeFrameRxSize-27;i++)
	{
		UhomePreBuffer[i+9]=UhomePreBuffer[i+27];
	}
	UhomePreBuffer[2]-=18;
	UhomeFrameRxSize=UhomeFrameRxSize-18;
	UhomePreBuffer[UhomeFrameRxSize-1]=0;
	for(i=2;i<UhomeFrameRxSize-1;i++)
	{
		UhomePreBuffer[UhomeFrameRxSize-1]+=UhomePreBuffer[i];
	}
	for(i=0;i<UhomeFrameRxSize;i++)
	{
		UhomeRxBuffer[i]=UhomePreBuffer[i];
	}
	Ack65=1;
	UhomeUnpack();
}
static void InvalidPack(void)
{
	if(FALSE!=isReply)		isReply = FALSE;		// 接收到无效指令应答后 地址清零 防止此时进入汇报状态 发送02状态应答帧

	UhomeTxBuffer[0] = 0xFF;					// 帧头
	UhomeTxBuffer[1] = 0xFF;
	UhomeTxBuffer[2] = 10;					// 帧长度:  0xff 0x55  其中0x55 不计入长度

	UhomeTxBuffer[3] = UhomeWifiMacBuf[0];	// WIFI MAC
	UhomeTxBuffer[4] = UhomeWifiMacBuf[1];
	UhomeTxBuffer[5] = UhomeWifiMacBuf[2];
	UhomeTxBuffer[6] = UhomeWifiMacBuf[3];
	UhomeTxBuffer[7] = UhomeWifiMacBuf[4];
	UhomeTxBuffer[8] = UhomeWifiMacBuf[5];


	UhomeTxBuffer[9] = 0X03;		// 帧类型

	UhomeTxBuffer[10] = (u8)(InvalidInfoCode_Uhome>>8);			// 无效代码16 bit
	UhomeTxBuffer[11] = (u8)InvalidInfoCode_Uhome;
}
static void ReplyConfirmPack(void)
{
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 8;		// 帧长 :

	UhomeSetIdenty();

	UhomeTxBuffer[9] = 0x05;
}

static void ReplyConfirmPack65(void)
{
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 26;		// 帧长 :

	UhomeSetIdenty();

	
	UhomeTxBuffer[9] = 0x65;	
	UhomeSetMacAddress();

	UhomeTxBuffer[27] = 0x05;	
}
static void IDChkPack(void)
{
	int i=25;
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 40;		// 帧长 :

	UhomeSetIdenty();

	UhomeTxBuffer[9] = 0x71;
	
	UhomeTxBuffer[10] = 0x11;
	UhomeTxBuffer[11] = 0x1c;
	UhomeTxBuffer[12] = 0x12;
	UhomeTxBuffer[13] = 0x00;
	UhomeTxBuffer[14] = 0x24;
	UhomeTxBuffer[15] = 0x00;
	UhomeTxBuffer[16] = 0x08;
	UhomeTxBuffer[17] = 0x10;
	UhomeTxBuffer[18] = 0x0d;
	UhomeTxBuffer[19] = 0x81;
	UhomeTxBuffer[20] = 0x01;
	UhomeTxBuffer[21] = 0x50;
	UhomeTxBuffer[22] = 0x40;
	UhomeTxBuffer[23] = 0x18;
	UhomeTxBuffer[24] = 0x03;
	for(i=25;i<42;i++)
	{
		UhomeTxBuffer[i] = 0x00;
	}
}
static void VersionPack(void)
{
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 35;		// 帧长 :

	UhomeSetIdenty();

	UhomeTxBuffer[9] = 0x62;
	
	
	UhomeTxBuffer[10] = 0x45;
	UhomeTxBuffer[11] = 0x2b;
	UhomeTxBuffer[12] = 0x2b;
	UhomeTxBuffer[13] = 0x32;
	UhomeTxBuffer[14] = 0x2e;
	UhomeTxBuffer[15] = 0x31;
	UhomeTxBuffer[16] = 0x36;
	UhomeTxBuffer[17] = 0x00;
	
	UhomeTxBuffer[18] = 0x30;
	UhomeTxBuffer[19] = 0x30;
	UhomeTxBuffer[20] = 0x30;
	UhomeTxBuffer[21] = 0x30;
	UhomeTxBuffer[22] = 0x30;
	UhomeTxBuffer[23] = 0x30;
	UhomeTxBuffer[24] = 0x30;
	UhomeTxBuffer[25] = 0x31;
	
	UhomeTxBuffer[26] = UhomePassMethod;
	UhomeTxBuffer[27] = UhomePassParameter[0];
	UhomeTxBuffer[28] = UhomePassParameter[1];
	
	UhomeTxBuffer[29] = 0x30;
	UhomeTxBuffer[30] = 0x30;
	UhomeTxBuffer[31] = 0x30;
	UhomeTxBuffer[32] = 0x30;
	UhomeTxBuffer[33] = 0x30;
	UhomeTxBuffer[34] = 0x30;
	UhomeTxBuffer[35] = 0x30;
	UhomeTxBuffer[36] = 0x31;
}
static void GetPassPack(void)
{
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 32;		// 帧长 :

	UhomeSetIdenty();

	UhomeTxBuffer[9] = 0x63;
	
	UhomeTxBuffer[10] = 0x30;
	UhomeTxBuffer[11] = 0x45;
	UhomeTxBuffer[12] = 0x2b;
	UhomeTxBuffer[13] = 0x2b;
	UhomeTxBuffer[14] = 0x32;
	UhomeTxBuffer[15] = 0x2e;
	UhomeTxBuffer[16] = 0x31;
	UhomeTxBuffer[17] = 0x36;
	UhomeTxBuffer[18] = 0x30;
	UhomeTxBuffer[19] = 0x30;
	UhomeTxBuffer[20] = 0x30;
	UhomeTxBuffer[21] = 0x30;
	UhomeTxBuffer[22] = 0x30;
	UhomeTxBuffer[23] = 0x30;
	UhomeTxBuffer[24] = 0x30;
	UhomeTxBuffer[25] = 0x31;
	
	UhomeTxBuffer[26] = 0x30;
	UhomeTxBuffer[27] = 0x30;
	UhomeTxBuffer[28] = 0x30;
	UhomeTxBuffer[29] = 0x30;
	UhomeTxBuffer[30] = 0x30;
	UhomeTxBuffer[31] = 0x30;
	UhomeTxBuffer[32] = 0x30;
	UhomeTxBuffer[33] = 0x31;
}
static void ReportConfigPack()
{
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 10;		// 帧长 :

	UhomeSetIdenty();
	UhomeTxBuffer[9] = 0x7d;
	UhomeTxBuffer[10] = (u8)(ReportIntTmrReceive>>8);
	UhomeTxBuffer[11] = (u8)ReportIntTmrReceive;
}
void UhomeWifi_Config(void)//进入配置模式
{
	int i=0;
	for(i=0;i<12;i++)
	{
		UhomeTxBuffer[i] = WIFICONFIG[i];
	}
	if(UhomePassMethod==1)
	{
		UhomeTxBuffer[3]=0x80;
	}
}
void UhomeWifi_Normal(void)
{
	int i=0;
	for(i=0;i<12;i++)
	{
		UhomeTxBuffer[i] = WIFINORMAL[i];
	}
	if(UhomePassMethod==1)
	{
		UhomeTxBuffer[3]=0x80;
	}
}
//设置Mac地址
void UhomeSetMacAddress(void)
{
	int i=0;
	for(i=0;i<17;i++)
	{
		UhomeTxBuffer[i+10] = MACADDRESS[i];
	}
}
//设置地址标识
void UhomeSetIdenty(void)
{
	int i=0;
	
	if((UhomePassMethod==0)||(UhomeIsEn==false))
	{
		UhomeTxBuffer[i+3]=0x00;
	}
	else if((UhomePassMethod==1)&&(UhomeIsEn==true))
	{
		UhomeTxBuffer[i+3]=0x80;
	}
	for(i=1;i<6;i++)
	{
		UhomeTxBuffer[i+3]=0x00;
	}
}
static void ReportAlarmPack(void)
{
	u16 i;
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 42;		// 帧长 :

	UhomeSetIdenty();
	
	UhomeTxBuffer[9] = 0x65;
	
	UhomeSetMacAddress();
	
	UhomeTxBuffer[27] = 0x04;
	
	for(i=28;i<43;i++)
	{
		UhomeTxBuffer[i] = 0x00;
	}
	UhomeTxBuffer[43] = 0x01;
}

static void AlarmPack(void)
{
	u16 i;
	UhomeTxBuffer[0] = 0xff;		// 帧头
	UhomeTxBuffer[1] = 0xff;
	UhomeTxBuffer[2] = 22;		// 帧长 :

	UhomeSetIdenty();

	UhomeTxBuffer[9] = 0x74;
	
	for(i=10;i<26;i++)
	{
		UhomeTxBuffer[i] = 0x00;
	}
}
void StateCheck(void)
{
	if(AirConditionData.temp!=AirConditionReceiveData.temp)
	{
		AirConditionData.temp=AirConditionReceiveData.temp;
		isState =true;
	}
	else if(AirConditionData.state!=AirConditionReceiveData.state)
	{
		AirConditionData.state=AirConditionReceiveData.state;
		isState =true;
	}
	else if(AirConditionData.mode!=AirConditionReceiveData.mode)
	{
		AirConditionData.mode=AirConditionReceiveData.mode;
		isState =true;
	}
	else if(AirConditionData.windLeftRight!=AirConditionReceiveData.windLeftRight)
	{
		AirConditionData.windLeftRight=AirConditionReceiveData.windLeftRight;
		isState =true;
	}
	else if(AirConditionData.windspeed!=AirConditionReceiveData.windspeed)
	{
		AirConditionData.windspeed=AirConditionReceiveData.windspeed;
		isState =true;
	}
	else if(AirConditionData.windUpDown!=AirConditionReceiveData.windUpDown)
	{
		AirConditionData.windUpDown=AirConditionReceiveData.windUpDown;
		isState =true;
	}
}
void AlarmTmrRun(void)
{
	u8 i = 0;
	
	if(0!=MainTimerFlag.MS25)
	{
		if(ReportIntTmr!=0)
		{	
			ReportIntTmr--;
		}
		for(i=0;i<UHOMEMAX_IUNUM;i++)
		{
			if((0!=UhomeIuErr[i].FreeTmr)&&(0!=UhomeIuErr[i].AlarmEn))
			{
				UhomeIuErr[i].FreeTmr--;
			}
		}
		MainTimerFlag.MS25=0;
	}
}
bool IsNeedErrPro(void)		// 有故障需要处理
{
	u8 i = 0;
	u8 needPro = 0;
	for(i=0;i<1;i++)
	{
		//if(1==IU[i].ValidExist)
		{
			if(UhomeIuErr[i].CodeOld!=IU[i]._ErrCode)
			{
				UhomeIuErr[i].CodeOld = IU[i]._ErrCode;
				UhomeIuErr[i].SetFreeTmr = UHOMECOM_ALARMTIME_200MS;
				UhomeIuErr[i].FreeTmr = UhomeIuErr[i].SetFreeTmr;
				UhomeIuErr[i].AlarmEn = 1;
				UhomeIuErr[i].Occured = 1;			// 内机故障发送 将要发送
				break;
			}
			else if((0==UhomeIuErr[i].FreeTmr)&&(0!=UhomeIuErr[i].AlarmEn))
			{
				UhomeIuErr[i].Occured = 1;			// 内机故障发送 将要发送
				break;
			}
		}
	}
	if(i<1)
	{
		if(0!=UhomeIuErr[i].Occured)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
bool isStateChg(void)
{
	StateCheck();
	if((ReportIntTmr==0)&&(isState==true))
	{
		return true;
	}
	else
	{
		return false;
	}
}
void EnCode(void)
{
	uint16_t BufferLength,i;
	BufferLength = UhomeFrameTxSize-10;
	f_GetCodeKey(UhomePassParameter[0],UhomePassParameter[1]);
	for(i=0;i<BufferLength;i++)
	{
		UhomeBuffer[i]=UhomeTxBuffer[i+9];
	}
	f_GetEncodeData(UhomeBuffer,BufferLength,UhomePassParameter[1]);
	for(i=0;i<BufferLength;i++)
	{
		UhomeTxBuffer[i+9]=UhomeBuffer[i];
	}
}
void DeCode(void)
{
	uint16_t BufferLength,i;
	BufferLength = UhomeFrmLength - 7;
	f_GetCodeKey(UhomePassParameter[0],UhomePassParameter[1]);
	for(i=0;i<BufferLength;i++)
	{
		UhomeBuffer[i]=UhomePreBuffer[i+9];
	}
	f_GetDecodeData(UhomeBuffer,BufferLength,UhomePassParameter[1]);
	for(i=0;i<BufferLength;i++)
	{
		UhomePreBuffer[i+9]=UhomeBuffer[i];
	}
}