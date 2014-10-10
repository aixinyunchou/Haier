/*=========================================================
* 文 件 名: NetEnDeCode.c
* 版    权: 青岛海尔科技有限公司  
* 功能描述: 网络协议2.0加密解密相关程序

* 修改日期			修改人				修改说明 
  2013-3-18       吴恩豪            完成网络加密解密基本功能 
========================================================= */
/**********************************************************
*使用说明：
1.void f_GetCodeKey(unsigned char *pCodeKeyIndex,unsigned char i_CodeKeyNum)
            pCodeKeyIndex 需要加密密钥索引，需要输入索引数组首地址，
            i_CodeKeyNum 密钥数量，返回为u8_CipherkeyBuf 加密密钥
2.void f_GetEncodeData(unsigned char *pEncodeData,
      		unsigned int i_EnCodeDataNum,unsigned char i_CodeKeyNum)  
       pEncodeData 需要加密数据，输入为加密数组首地址，
       i_CodeKeyNum 密钥数量
       i_EnCodeDataNum 需要加密数据数量
3.void f_GetDecodeData(unsigned char *pEncodeData,
      		unsigned int i_EnCodeDataNum,unsigned char i_CodeKeyNum)  
       pEncodeData 需要加密数据，输入为加密数组首地址，
       i_CodeKeyNum 密钥数量
       i_EnCodeDataNum 需要加密数据数量        
**********************************************************/
#include "NetEnDeCode.h"							/* 包含头文件 */
#include "Table_NetCode.h"							/* 包含网络密码表 */

unsigned char u8_NetCodeKeyBuf[CODE_KEY_NUM];

/**********************************************************
;  模块名称:  void f_GetCodeKey(unsigned char *pCodeKeyIndex,
                   unsigned char i_CodeKeyNum)
;	模块功能:  获取加密密钥 
;	输入：	pCodeKeyIndex 需要加密密钥索引，i_CodeKeyNum 密钥数量
;	输出：	u8_CipherkeyBuf 加密密钥  
;**********************************************************/
void f_GetCodeKey(unsigned char i_CodeKeyIndex,unsigned char i_CodeKeyNum)
{
   unsigned int i;
  
   for (i=0; i< i_CodeKeyNum; i++)
   {
      u8_NetCodeKeyBuf[i] = TABLE_CODE_KEY[(unsigned  int)(i_CodeKeyIndex + i) % 256];
   }
}
/**********************************************************
;  模块名称:  void f_GetEncodeData(unsigned char *pEncodeData,
                     unsigned int i_DataNum,unsigned char i_CodeKeyNum)
;	模块功能:  获取加密数据 
;	输入：	pEncodeData 需要加密数据，i_CodeKeyNum 密钥数量
            i_EnCodeDataNum 需要加密数据数量 
;	输出：	加密后数据  
;**********************************************************/
void f_GetEncodeData(unsigned char *pEncodeData,
      unsigned int i_EnCodeDataNum,unsigned char i_CodeKeyNum)
{
   unsigned int i;
   
   for(i=0; i< i_EnCodeDataNum; i++)
   {
     pEncodeData[i] ^= u8_NetCodeKeyBuf[i% i_CodeKeyNum];
   }
}
/**********************************************************
;  模块名称:  void f_GetDecodeData(unsigned char *pDecodeData,
                     unsigned int i_DeCodeDataNum,unsigned char i_CodeKeyNum)
;	模块功能:  获取解密数据 
;	输入：	pEncodeData 需要加密数据，i_CodeKeyNum 密钥数量
            i_DeCodeDataNum 需要加密数据数量 
;	输出：	加密后数据  
;**********************************************************/
void f_GetDecodeData(unsigned char *pDecodeData,
      unsigned int i_DeCodeDataNum,unsigned char i_CodeKeyNum)
{
   unsigned int i;
   
   for(i=0; i< i_DeCodeDataNum; i++)
   {
     pDecodeData[i] ^= u8_NetCodeKeyBuf[i% i_CodeKeyNum];
   }
}