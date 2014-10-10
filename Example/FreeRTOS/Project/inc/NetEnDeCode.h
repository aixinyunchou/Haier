/*=========================================================
* 文 件 名: NetEnDeCode.h
* 版    权: 青岛海尔科技有限公司  
* 功能描述: 本文件相关函数及宏定义 

* 修改日期			修改人				修改说明 
* 2013-03-18		吴恩豪 			完成基本功能 
========================================================= */

#ifndef _CIPHER_H
#define _CIPHER_H

#define  CODE_KEY_NUM     			16                 //密钥数量 



// 以下为内部函数定义 
void f_GetCodeKey(unsigned char i_CodeKeyIndex,unsigned char i_CodeKeyNum);
void f_GetEncodeData(unsigned char *pEncodeData,
      unsigned int i_EnCodeDataNum,unsigned char i_CodeKeyNum);
void f_GetDecodeData(unsigned char *pDecodeData,
      unsigned int i_DeCodeDataNum,unsigned char i_CodeKeyNum);
                   
#endif