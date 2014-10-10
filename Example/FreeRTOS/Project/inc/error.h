#ifndef ERROR_INCLUDE_H
#define ERROR_INCLUDE_H
#include "typedefine.h"

#ifndef ERR_GLOBAL
#define ERR_EXT extern
#else
#define ERR_EXT
#endif

typedef union
{
	u16 Total;
	struct
	{
		u16 E2PROM       :1;   //5
		u16 WCCom        :1;   //7
		u16 Pump         :1;   //8
		u16 DupIOAd      :1;   //9
		u16 Def10        :1;   //10
		u16 Def11        :1;   //11---无
		u16 ZeroChk      :1;   //12---50Hz过零、或者和P板通信故障
		u16 IOCom        :1;   //13
		u16 DCFan        :1;
		u16 Def7         :7;
	} Type;
} ERROR_TYPE;

ERR_EXT ERROR_TYPE  ErrorInfo;        	 //故障标识
ERR_EXT u8 Error;     //故障代码
ERR_EXT u8 SuperErr;  //超级故障代码，与Error 的区别： 关机情况下Error=0，SuperErr=故障；开机情况下Error=SuperErr=故障；

#define ERR_E2PROM   ErrorInfo.Type.E2PROM  
#define ERR_Def6     ErrorInfo.Type.Def6    
#define ERR_WCCom    ErrorInfo.Type.WCCom   
#define ERR_Pump     ErrorInfo.Type.Pump    
#define ERR_DupIOAd  ErrorInfo.Type.DupIOAd 
#define ERR_Def11    ErrorInfo.Type.Def11   
#define ERR_ZeroChk  ErrorInfo.Type.ZeroChk
#define ERR_IOCom    ErrorInfo.Type.IOCom   
#define ERR_DCFan    ErrorInfo.Type.DCFan   

#define MAXZEROLOST 8   //连续n(8相当于5-7次)次找不到过零点, 报故障
#define MAXZEROVALUELOST  MAXZEROLOST*2/5

void CheckError(void);

#endif